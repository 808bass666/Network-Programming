#include <stdio.h>     
#include <stdlib.h>    
#include <string.h>     
#include <unistd.h>
#include <arpa/inet.h>  
#include <sys/socket.h> 
#define BUF_SIZE 30 
void error_handling(char* message);

int main(int argc, char* argv[]) {
    int clnt_sock;
    struct sockaddr_in serv_adr; 
    FILE *fp;
    char buf[BUF_SIZE]; //存放从服务器端接收过来的数据
    
    if(argc != 3) { 
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }   
   
    fp = fopen("received.dat","wb");//创建新文件以保存服务器端传输的文件数据 
    clnt_sock = socket(PF_INET, SOCK_STREAM, 0); 
    if(clnt_sock == -1) error_handling("socket() error");
    
    memset(&serv_adr, 0, sizeof(serv_adr)); 
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]); 
    serv_adr.sin_port = htons(atoi(argv[2])); 
    
    if(connect(clnt_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1) error_handling("connect() error");    
    else puts("Connected!");
	
    int read_cnt = 0;
	while ((read_cnt = read(clnt_sock, buf, BUF_SIZE)) != 0) fwrite((void*)buf, sizeof(char), read_cnt, fp);
    puts("received!");
	write(clnt_sock, "Thank you", 10);
	fclose(fp);
    close(clnt_sock);
    return 0;
}
void error_handling(char* message) {
    perror(message); 
    exit(1);  
}
