#include <stdio.h>    
#include <stdlib.h>      
#include <string.h>     
#include <unistd.h>    
#include <arpa/inet.h>  
#include <sys/socket.h>  
#define BUF_SIZE 100
void error_handling(char*buf);  

int main(int argc, char* argv[]) { 
    int serv_sock, clnt_sock; 
    char buf[BUF_SIZE];
  	struct sockaddr_in serv_adr, clnt_adr; 
    socklen_t clnt_adr_sz;  

    fd_set reads, cpy_reads;
    struct timeval timeout;
    int fd_max, fd_num, i, str_len;
  
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
     
    int on = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)  error_handling("setsockopt() error");
     
    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1) error_handling("bind() error"); 
     
    if(listen(serv_sock, 5) == -1) error_handling("listen() error");

    FD_ZERO(&reads);//给read所有位初始化为0
    FD_SET(serv_sock, &reads);//在reads中注册文件描述符serv_sock的信息
    fd_max = serv_sock;//表示fd监视范围的右端点

    while (1) {
      cpy_reads = reads;

      timeout.tv_sec = 5;
      timeout.tv_usec = 5000;

      fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout);
      if (fd_num == -1) break;//发生错误返回1
      if (fd_num == 0) continue;//超时返回0
      
      for (i = 0; i < fd_max + 1; ++i) {
        if (FD_ISSET(i, &cpy_reads)) { //若fd=i的文件描述符发生变化
          if (i == serv_sock) { //若服务器端套接字发生变化，则受理连接请求
            clnt_adr_sz = sizeof(clnt_adr); 
            clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &clnt_adr_sz); 
            if(clnt_sock == -1) error_handling("accept() error");

            FD_SET(clnt_sock, &reads);//注册与客户端连接的套接字文件描述符
          
            if (fd_max < clnt_sock) fd_max=clnt_sock;
            printf("connected client: %d \n", clnt_sock);
          }
          else { //发生变化的并非服务器端套接字，此时需要确认接收的数据是字符串还是代表断开连接的EOF
            str_len = read(i, buf, BUF_SIZE);
            if (str_len == 0) { //接收的是EOF时，close request!
              FD_CLR(i, &reads);//从reads中删除该套接字的信息
              close(i);//关闭套接字
              printf("closed client: %d \n", i);
            }
            else write(i, buf, str_len);
          }
        }
      }
    }
    close(serv_sock); 
    return 0;
}
void error_handling(char *buf) {
    perror(buf);   
    exit(1);  
}
