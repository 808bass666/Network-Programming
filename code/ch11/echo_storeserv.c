#include <stdio.h>       
#include <stdlib.h>     
#include <string.h>      
#include <unistd.h>      
#include <signal.h>
#include <sys/wait.h>     
#include <arpa/inet.h>   
#include <sys/socket.h>  
#define BUF_SIZE 100  
void read_childproc(int sig);
void error_handling(char* buf); 

//将回声客户端传输的字符串按序保存到文件中(在子进程中实现)
//数据的流动是从客户端->服务器端的子进程B->服务器端的子进程A->文件

int main(int argc, char* argv[]) { 
    int  serv_sock, clnt_sock; 
    char buf[BUF_SIZE];
	struct sockaddr_in serv_adr, clnt_adr; 
    socklen_t clnt_adr_sz;  
    int str_len, state;
    pid_t pid;
    int fds[2];
    
	if(argc != 2) {  
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }   

    struct sigaction act;
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART; 
    state = sigaction(SIGCHLD, &act, 0);
    
    serv_sock = socket(PF_INET, SOCK_STREAM, 0); 
    if(serv_sock == -1) error_handling("socket() error");
    
    memset(&serv_adr, 0, sizeof(serv_adr)); 
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1])); 
    
    int on = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)  error_handling("setsockopt() error");
    
    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1) error_handling("bind() error"); 
     
    if(listen(serv_sock, 5) == -1) error_handling("listen() error");

    pipe(fds);
    pid = fork();//第一个子进程A

    if (pid == 0){
		FILE *fp = fopen("echomsg.txt", "wt");
		char msgbuf[BUF_SIZE];
		int i, len;
		for(i = 0; i < 3; ++i) { //共3次的fwrite调用完成后，可以打开echomsg.txt验证保存的字符串,无论有多少个客户端链接，echomsg中都只会有三次传输的结果
			len = read(fds[0], msgbuf, BUF_SIZE);
			fwrite((void*)msgbuf, 1, len, fp);//将从管道读出的信息(保存在msgbuf中)写入文件
			//fwrite第二个参数是要写出数据的 基本单元 的字节大小，这里是sizeof(char)
		}
		fclose(fp);
	}
    
    while(1) {
        clnt_adr_sz = sizeof(clnt_adr); //initialize clnt_adr_sz
        // 从监听队列中取出一个连接请求进行连接，并返回与对应客户端进行连接的连接socket
        clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &clnt_adr_sz); 
        if(clnt_sock == -1) continue;
        else puts("new client connected...");

        pid = fork();//第二个子进程B
       
        if (pid == 0) {
            close(serv_sock);  
            while ((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0) {
                write(clnt_sock, buf, str_len);
                write(fds[1], buf, str_len);//通过管道将传输的数据传输给子进程A
            }
            close(clnt_sock); 
            puts("client disconnected..");
            return 0;
        }
        else close(clnt_sock);
    }
    close(serv_sock);  
    return 0;
}
void read_childproc(int sig){ 
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    printf("Removed child_PID: %d \n", pid);
}
void error_handling(char* buf) {
    perror(buf);   
    exit(1);   
}
