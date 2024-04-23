#include <stdio.h>     
#include <stdlib.h>    
#include <string.h>     
#include <unistd.h>
#include <arpa/inet.h>  
#include <sys/socket.h> 
#define BUF_SIZE 1024 
#define OPSZ 4  //每个操作数都是int整型数，都占4字节
void error_handling(char* message);
int calculate(int opcnt, int opinfo[], char operator);
//参数分别为：操作数的个数，保存操作数据的字符数组，操作符

int main(int argc, char* argv[]) {
    int  serv_sock, clnt_sock;
    char opinfo[BUF_SIZE];
    struct sockaddr_in serv_adr, clnt_adr; 
    socklen_t clnt_adr_sz; 
    if(argc != 2) { 
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }   
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
    clnt_adr_sz = sizeof(clnt_adr); 
    int i, res, recv_len, recv_cnt, opcnt;
    // Accept up to 5 client connections
    for (i = 0; i < 5; ++i) {
        // 从监听队列中取出一个连接请求进行连接，并返回与对应客户端进行连接的连接socket
        clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &clnt_adr_sz); 
        if(clnt_sock == -1) error_handling("accept() error");
        else printf("the %dth connected client \n", i + 1);
        // Read data from the client and write it back
        opcnt = 0, recv_len = 0; //接收的数据的总字节数
        //首先接收操作数的个数
        read(clnt_sock, &opcnt, 1); //将读到的头个字符保存到opcnt变量中
        //再读操作数以及操作符
		while (recv_len < (opcnt * OPSZ + 1)) { 
            //从第一个操作数开始，将数据读到opinfo数组中，一次最多读BUF_SIZE-1
            recv_cnt = read(clnt_sock, &opinfo[recv_len], BUF_SIZE - 1);
            recv_len += recv_cnt; 
        } 
        //做运算并将结果用res保存，再传给客户端
        res = calculate(opcnt, (int*)opinfo, opinfo[recv_len - 1]);
        //cal()第三个参数就已经传了操作数了，因此直接将opinfo强转为int型就可以，方便运算
        write(clnt_sock, (char*) &res, sizeof(res));
        //将指向整型数res的指针转换为指向char型的指针（指针即地址）
        close(clnt_sock); 
    }
    close(serv_sock); 
    return 0;
}
//参数分别为：操作数的个数，保存数据的字符数组，操作符
int calculate(int opcnt, int opinfo[], char operator) {
    int res = opinfo[0], i; //opinfo中没有存操作数的个数，直接从操作数开始存的
    switch(operator) {
        case '+':
            for (i = 1; i < opcnt; ++i) res += opinfo[i];
            break;
        case '-':
            for (i = 1; i < opcnt; ++i) res -= opinfo[i];
            break;
        case '*':
            for (i = 1; i < opcnt; ++i) res *= opinfo[i];
            break;
    }
    return res;
}
void error_handling(char* message) {
    perror(message);  
    exit(1);  
}
