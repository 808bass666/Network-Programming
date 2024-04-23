#include <stdio.h>     
#include <stdlib.h>    
#include <string.h>     
#include <unistd.h>
#include <arpa/inet.h>  
#include <sys/socket.h> 
#define BUF_SIZE 1024 
#define RLT_SIZE 4 //结果是4字节int整型数，占4字节
#define OPSZ 4  //每个操作数都是int整型数，都占4字节
void error_handling(char* message);

int main(int argc, char* argv[]) {
    int clnt_sock;
    char opMsg[BUF_SIZE]; 
    //要想在同一数组保存并传输多种数据类型，应当把数组声明为char类型，并且之后需要额外做一些指针转换
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
    
	//从用户那边读取数据到opMsg
    int res, opcnt, i; 
    fputs("Operand count : ", stdout); //操作数的个数
    scanf("%d", &opcnt);
    //`scanf` 用于读取用户输入的数据，而 `fputs` 用于向标准输出打印提示信息或其他字符串
    //因为协议第一条规定客户端以1字节整数形式传递操作数个数
    opMsg[0] = (char) opcnt; //因此要将int强转成char存到字符数组，占一个字节
    
	for (i = 0; i < opcnt ; ++i) { //挨个输入操作数
        printf("Operand %d: ", i + 1);
		//将用户输入的操作数保存到opMsg中
        scanf("%d", (int*)&opMsg[i * OPSZ + 1]);//协议第二条规定操作数为整型数，因此强转(char*)为(int*)
        //scanf 函数中 %d 格式说明符 要求传递一个指向整数的指针，以便将输入的整数值存储到该指针指向的内存位置。而opMsg是一个字符数组
    }
    fgetc(stdin); 
    //读取并丢弃标准输入缓冲区中的一个字符。这通常是为了消耗掉用户输入中的换行符或其他潜在的剩余字符。
    //在这里是为了处理用户在输入操作数后按下 Enter 键留下的换行符'\n'。
    fputs("Operator : ", stdout); 
    //协议第二条规定操作符占用1字节(char)，不用转换
    scanf("%c", &opMsg[opcnt * OPSZ + 1]); 
    //传输opMsg中的数据到服务器
    write(clnt_sock, opMsg, opcnt * OPSZ + 2); 
    //将从服务器端得到的结果(4字节)读到res变量中
    read(clnt_sock, &res, RLT_SIZE); //RLT_SIZE=4
    
    printf("Operation result: %d \n", res);    
    close(clnt_sock);
    return 0;
}
void error_handling(char* message) {
    perror(message); 
    exit(1);  
}
