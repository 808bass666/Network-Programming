#include <stdio.h>      // Standard input/output functions
#include <stdlib.h>     // Standard library functions
#include <string.h>     // String handling functions
#include <unistd.h>     // Unix standard functions
#include <arpa/inet.h>  // IP address conversion functions
#include <sys/socket.h> // Socket functions
#define BUF_SIZE 1024   // Define a constant for the buffer size
void error_handling(char* message); // Function prototype for error handling

int main(int argc, char* argv[]) {
    //argv[0] is the program name,argv[1] is port number
    int  serv_sock, clnt_sock;
    //serv_sock: server socket(gatekeeper)，
    //clnt_sock: connection socket to communicate with the client
    char message[BUF_SIZE];
	struct sockaddr_in serv_adr, clnt_adr; 
    socklen_t clnt_adr_sz; //initialize clnt_adr_sz
    
	if(argc != 2) { //Two parameters are required, one is the program name, and the other one is port number
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }   
    
	// Creates a (gatekeeper) socket.
    serv_sock = socket(PF_INET, SOCK_STREAM, 0); 
    if(serv_sock == -1) error_handling("socket() error");
    
	//Initialize the server address structure (gatekeeper)
    memset(&serv_adr, 0, sizeof(serv_adr)); 
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1])); 
    
	//在门卫socket调用bind之前设置socket选项SO_REUSEADDR
    //表示允许地址重用, 否则close之后会有一个WAIT_TIME状态，使得该ip和端口仍>被占用，产生bind() error.
    int on = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)  error_handling("setsockopt() error");
    
	// Bind the (gatekeeper) socket, also listening socket later.
    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1) error_handling("bind() error"); 
    
	//Listen for incoming connections，set backlog queue of size 5.
    if(listen(serv_sock, 5) == -1) error_handling("listen() error");
	
	int i, str_len;
    clnt_adr_sz = sizeof(clnt_adr); //initialize clnt_adr_sz
    // Accept up to 5 client connections
    for (i = 0; i < 5; ++i) {
        // 从监听队列中取出一个连接请求进行连接，并返回与对应客户端进行连接的连接socket
        clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &clnt_adr_sz); 
		if(clnt_sock == -1) error_handling("accept() error");
        else printf("the %dth connected client \n", i + 1);
       
	   	// Read data from the client and write it back
        while ((str_len = read(clnt_sock, message, BUF_SIZE)) != 0) 
            write(clnt_sock, message, str_len);
        close(clnt_sock); //关闭与客户端通信的连接socket
    }
    close(serv_sock); // Close the server socket (gatekeeper)
    return 0;
}
void error_handling(char* message) {
    perror(message);  // 打印错误信息
    exit(1);  // 退出程序
}
