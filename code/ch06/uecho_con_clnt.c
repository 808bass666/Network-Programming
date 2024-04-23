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
    char message[BUF_SIZE];
    //struct sockaddr_in serv_adr, from_adr; //不再需要from_adr！
    struct sockaddr_in serv_adr; 
    //socklen_t src_adr_sz; //多余变量！
    if(argc != 3) { 
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }   

    clnt_sock = socket(PF_INET, SOCK_DGRAM, 0); 
    if(clnt_sock == -1) error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr)); 
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]); 
    serv_adr.sin_port = htons(atoi(argv[2])); 
    
    //添加connect函数使之变为【已连接套接字】！
    connect(clnt_sock, (struct sockaddr *) &serv_adr, sizeof(serv_adr));
    
    int str_len;
    while (1) {
        fputs("Insert message(Q to quit): ", stdout);
        fgets(message, sizeof(message), stdin);
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) break;

        /*sendto(clnt_sock, message, strlen(message), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr)); 
        可以直接用write()了 */
        write(clnt_sock, message, strlen(message));

        /*src_adr_sz = sizeof(src_adr);
        str_len = recvfrom(clnt_sock, message, BUF_SIZE , 0, (struct sockaddr*)&src_adr, &src_adr_sz));
        可以直接用read()了 */
        str_len = read(clnt_sock, message, strlen(message) - 1);
        
        message[str_len] = 0;
        printf("Message from server: %s \n", message);
    }
    close(clnt_sock);
    return 0;
}
void error_handling(char* message) {
    perror(message);  
    exit(1); 
}
