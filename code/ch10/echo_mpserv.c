#include <stdio.h>       
#include <stdlib.h>     
#include <string.h>      
#include <unistd.h>      
#include <signal.h>
#include <sys/wait.h>     
#include <arpa/inet.h>   
#include <sys/socket.h>  
#define BUF_SIZE 1024   
void read_childproc(int sig);
void error_handling(char* buf); 

int main(int argc, char* argv[]) { 
    int  serv_sock, clnt_sock; 
    char buf[BUF_SIZE];
	struct sockaddr_in serv_adr, clnt_adr; 
    socklen_t clnt_adr_sz;  
    pid_t pid;
    int str_len, state;
    
	if(argc != 2) {  
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }   
    // 这一段是注册SIGCHLD信号对应的handler，以在子进程结束产生SIGCHLD信号时系统能通过handler处理回收子进程，防止产生僵尸进程
    //(1)声明sigaction结构体变量act
    struct sigaction act;
    //(2)初始化结构体变量act
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART; 
    //若这里设置为0而不是设置为SA_RESTART话，将会报错accept() error: Interrupted system call。
    //原因是:进程被信号中断导致系统调用被中断。具体在这里是:当信号对应的handler返回时，会中断进程的系统调用，如果此时的系统调用没有使用SA_RESTART标志，那么在信号返回后，它将无法恢复运行 
    //(3)通过sigaction函数注册SIGCHLD信号对应的handler
    state = sigaction(SIGCHLD, &act, 0);
    
    serv_sock = socket(PF_INET, SOCK_STREAM, 0); 
    if(serv_sock == -1) error_handling("socket() error");
    
    memset(&serv_adr, 0, sizeof(serv_adr)); 
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1])); 
    
    //允许地址重用
    int on = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)  error_handling("setsockopt() error");
    
    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1) error_handling("bind() error"); 
     
    if(listen(serv_sock, 5) == -1) error_handling("listen() error");

    while(1) {
        clnt_adr_sz = sizeof(clnt_adr); //initialize clnt_adr_sz
        // 从监听队列中取出一个连接请求进行连接，并返回与对应客户端进行连接的连接socket
        clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &clnt_adr_sz); 
        if(clnt_sock == -1) continue;
        else puts("new client connected...");

        pid = fork();
        if (pid == -1) {
            close(clnt_sock);
            continue;
        }
        if (pid == 0) {
            close(serv_sock);  
            while ((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0) write(clnt_sock, buf, str_len);
            close(clnt_sock); 
            puts("client disconnected..");
            return 0;
        }
        else close(clnt_sock);//如果是父进程则关闭clnt_sock
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
    perror(buf);  // 打印错误信息
    exit(1);  // 退出程序
}
