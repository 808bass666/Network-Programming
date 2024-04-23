#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char* message);

int main(int argc, char* argv[]) {
    int serv_sock, clnt_sock;//套接字

    struct sockaddr_in serv_addr, clnt_addr;//地址信息
	socklen_t clnt_addr_size;

    char message[] = "hello world!";//要写入（传输）的数据
    
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }
   //创建服务器端套接字
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) error_handling("socket() errror");
    //地址信息初始化
    memset(&serv_addr, 0, sizeof(serv_addr));//给serv_addr结构体全初始化为0
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

	//在TCP服务器端套接字调用bind之前设置SO_REUSEADDR选项实现端口复用,否则close之后会有一个WAIT_TIME状态，使得该ip和端口号仍被占用，产生bind() error.
	int on = 1;
	if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0) error_handling("setsockopt() error");
	
	//分配地址信息
    if (bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }
    //准备好了之后，套接字就转换为可接受连接的监听状态,并定义可接受连接请求的队列长度
    if (listen(serv_sock, 5) == -1) error_handling("listen() error");
    
    clnt_addr_size = sizeof(clnt_addr);
    //接收连接请求
    clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1) error_handling("accept() error");
    
    //向客户端套接字中多次write()写入（传输）数据
	
	int inc = 0, len = strlen(message);
	while (len--) {
		//写成if (write(clnt_sock, message++, 1) == -1)会报错error: lvalue required as increment operand，
		//错在message++,因为左侧被赋值的数必须是变量，而不能是常量，除了显而易见的常量之外，
		//数组的首地址（比如这里的message），函数地址...都是常量
		if (write(clnt_sock, message + inc++, 1) == -1) error_handling("write() error");
	}
    
	close(serv_sock);
    close(clnt_sock);
    return 0;
}
void error_handling(char* message) {
	perror(message);//使用perro能显示更详细的错误信息
	exit(1);
}
