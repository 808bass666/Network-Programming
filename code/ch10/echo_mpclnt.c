#include <stdio.h>       
#include <stdlib.h>     
#include <string.h>      
#include <unistd.h>      
#include <arpa/inet.h>   
#include <sys/socket.h>  
#define BUF_SIZE 30
void read_routine(int sock, char* buf); 
void write_routine(int sock, char* buf); 
void error_handling(char* message); 

int main(int argc, char* argv[]) {
    int clnt_sock;
    char buf[BUF_SIZE];  
    struct sockaddr_in serv_adr; 
    if(argc != 3) {  
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }   
    
    clnt_sock = socket(PF_INET, SOCK_STREAM, 0); 
    if(clnt_sock == -1) error_handling("socket() error");
    
    memset(&serv_adr, 0, sizeof(serv_adr));  
    serv_adr.sin_family = AF_INET;  
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);  
    serv_adr.sin_port = htons(atoi(argv[2])); 
    
    if(connect(clnt_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1) error_handling("connect() error");    
    else puts("Connected!");

    pid_t pid = fork();
    //下面就是I/O分离的实现
    if (pid == 0) write_routine(clnt_sock, buf); //子进程负责写(发送)
    else read_routine(clnt_sock, buf); //父进程负责读(接收)
    
    close(clnt_sock);
    return 0;
}
void read_routine(int sock, char *buf) {
    while (1) {
        int str_len = read(sock, buf, BUF_SIZE - 1);
        if(str_len == 0) return;
        buf[str_len] = 0;
        printf("Message from server: %s", buf);
    }
}
void write_routine(int sock, char *buf) {
    while (1) {
        fgets(buf, BUF_SIZE, stdin); 
        if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n")) {
            shutdown(sock, SHUT_WR);
            return;
        }
        write(sock, buf, strlen(buf));
    }
}
void error_handling(char* message) {
    perror(message); 
    exit(1);  
}
