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
    struct sockaddr_in serv_adr, src_adr;
    socklen_t src_adr_sz;//接收数据时，数据发送端的socket地址的长度
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
    
    int str_len;
    while (1) {
        fputs("Insert message(Q to quit): ", stdout);
        fgets(message, sizeof(message), stdin);
        //communication is terminated when the user enters "q" or "Q"
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) break;
        // Send the message to the server
        sendto(clnt_sock, message, strlen(message), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
        // Receive a response from src
        src_adr_sz = sizeof(src_adr);
        str_len = recvfrom(clnt_sock, message, BUF_SIZE , 0, (struct sockaddr*)&src_adr, &src_adr_sz);
        message[str_len] = 0;
        printf("Message from server: %s", message);
    }
    close(clnt_sock);
    return 0;
}
void error_handling(char* message) {
    perror(message); 
    exit(1);  
}
