#include <stdio.h>     
#include <stdlib.h>   
#include <string.h>     
#include <unistd.h>    
#include <arpa/inet.h>
#include <sys/socket.h> 
#define BUF_SIZE 30
void error_handling(char* message); 

int main(int argc, char* argv[]) {
    int sock;
    char message[BUF_SIZE];
    struct sockaddr_in my_adr, your_adr;
    socklen_t adr_sz;
    if(argc != 2) { 
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }   

    sock = socket(PF_INET, SOCK_DGRAM, 0); 
    if(sock == -1) error_handling("socket() error");

    memset(&my_adr, 0, sizeof(my_adr)); 
    my_adr.sin_family = AF_INET;
    my_adr.sin_addr.s_addr = htonl(INADDR_ANY); 
    my_adr.sin_port = htons(atoi(argv[1])); 
    
    if (bind(sock, (struct sockaddr*) &my_adr, sizeof(my_adr)) == -1) error_handling("bind() error");
    
    int str_len, i;
    for (i = 0; i < 3; ++i) { //接收三次
        sleep(5); //delay 5 sec
        // 为了验证UDP传输特性，每隔5秒调用一次recvfrom()。
        // 对方调用3次sendto()发送数据，本端调用3次recvfrom()接收数据，因为每次recvfrom的调用都延迟了5秒，因此调用recvfrom前对方已经调用完了3次sendto，也就是说此时数据已经传输到了本端
        // 如果是TCP，则只需要调用一次recvfrom就可以读取所有数据；UDP则必须要对应地调用3次recvfrom才能接收完全部数据
        adr_sz = sizeof(your_adr);
        str_len = recvfrom(sock, message, BUF_SIZE , 0, (struct sockaddr*)&your_adr, &adr_sz);
        printf("Message %d: %s \n", i + 1, message);
    }
    close(sock);
    return 0;
}
void error_handling(char* message) {
    perror(message);  
    exit(1); 
}
