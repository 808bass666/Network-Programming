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
    char filename[BUF_SIZE]; //存放文件名
    
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
	
	fputs(" filename : ", stdout);
	scanf("%s", filename);//不要加& !!!
    
	//将文件名传输给服务器端
    write(clnt_sock, filename, strlen(filename));
    //while括号内语句的作用：将读到的文件数据写入到保存文件数据的buf缓冲区中
    // fwrite 函数用于将刚刚从服务器端读到的、read_cnt大小的、存在buf中的数据写入fp文件描述符表示的文件中
    int read_cnt = 0;
	while ((read_cnt = read(clnt_sock, buf, BUF_SIZE)) != 0) fwrite((void*)buf, sizeof(char), read_cnt, fp);
    // size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
    // - const void *ptr : 指针指向要写出数据的内存首地址 
    // - size_t size : 要写出数据的 基本单元 的字节大小(单位大小)
    // - size_t nmemb : 要写出数据的 基本单元 的个数 
    // - FILE *stream : 打开的文件指针 
    puts("received!");

	fclose(fp);
    close(clnt_sock);
    return 0;
}
void error_handling(char* message) {
    perror(message); 
    exit(1);  
}
