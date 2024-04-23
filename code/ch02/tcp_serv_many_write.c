#include <stdio.h>  // 包含标准输入输出库
#include <stdlib.h>  // 包含标准库
#include <string.h>  // 包含字符串处理函数库
#include <unistd.h>  // 包含Unix标准函数库
#include <arpa/inet.h>  // 包含IP地址转换函数
#include <sys/socket.h>  // 包含套接字函数库
void error_handling(char* message); //错误处理函数

int main(int argc, char* argv[]) {
    //argv[0]是程序名，argv[1]是端口号
    int serv_sock, clnt_sock; //serv_sock是服务端socket(门卫), clnt_sock是要与客户端通信的连接socket
    struct sockaddr_in serv_addr, clnt_addr; //保存地址信息的结构体变量
    socklen_t clnt_addr_size = sizeof(clnt_addr); //初始化 与客户端通信的连接socket的地址结构体的大小
    char message[] = "hello world!"; //要写入（传输）的数据
    if (argc != 2) { // 如果参数数量不为2（程序名称也是一个参数，还要有端口号参数）
        printf("Usage: %s <port>\n", argv[0]);
        exit(1); // 退出程序
    }
    //创建门卫socket
    serv_sock = socket(PF_INET, SOCK_STREAM, 0); 
    if (serv_sock == -1) error_handling("socket() errror");
    //初始化门卫socket的地址信息
    memset(&serv_addr, 0, sizeof(serv_addr)); //初始化serv_addr结构体为0
    serv_addr.sin_family = AF_INET; // 设置地址族
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //设置IP
    serv_addr.sin_port = htons(atoi(argv[1])); //设置端口，argv[1]是传进来的第一个参
    //在门卫socket调用bind之前设置socket选项SO_REUSEADDR
    //表示允许地址重用, 否则close之后会有一个WAIT_TIME状态，使得该ip和端口仍被占用，产生bind() error.
    int on = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)  error_handling("setsockopt() error");
    //命名socket（绑定地址）
    if (bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)  error_handling("bind() error");
    //准备好了之后，门卫socket设置监听队列长度，开始监听连接请求
    if (listen(serv_sock, 5) == -1) error_handling("listen() error"); 
    //接受客户端连接请求，创建新的套接字clnt_sock用于与客户端通信
    //这个新的套接字负责和【特定的】客户端进行数据交换，而原始的服务器套接字(serv_sock)继续监听新的连接请求
    clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
    //clnt_addr是成功与服务器建立连接的客户端socket地址结构体，clnt_addr_size是其长度（传入时为sizeof(clnt_addr)，传出为实际大小，会被填入客户端socket地址结构体中）
    if (clnt_sock == -1) error_handling("accept() error");    
    //向客户端套接字中多次write()写入（传输）数据
    //write(clnt_sock, message, 4);
    //write(clnt_sock, message + 4, 4);
    //write(clnt_sock, message + 8, 4);
    //write(clnt_sock, message + 12, sizeof(message) - 12);

    //或者写成：
    int inc = 0, len = strlen(message);
    while (len--) 
        if (write(clnt_sock, message + inc++, 1) == -1) error_handling("write() error");
    //如果第二个参数写成message++则会报错error: lvalue required as increment operand，因为++左侧被赋值的数必须是变量，而不能是常量，除了显而易见的常量之外，数组的首地址（比如这里的message），函数地址...都是常量
    
    close(clnt_sock); //关闭与客户端通信的连接socket
	close(serv_sock); //关闭服务端socket(门卫)
    return 0;
}
void error_handling(char* message) {
    perror(message);  // 打印错误信息
    exit(1);  // 退出程序
}
