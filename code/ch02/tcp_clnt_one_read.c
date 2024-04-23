#include <stdio.h>  // 包含标准输入输出库
#include <stdlib.h>  // 包含标准库
#include <string.h>  // 包含字符串处理函数库
#include <unistd.h>  // 包含Unix标准函数库
#include <arpa/inet.h>  // 包含IP地址转换函数
#include <sys/socket.h>  // 包含套接字函数库
void error_handling(char* message); //错误处理函数

int main(int argc, char* argv[]) {
    //argv[0]是程序名，argv[1]和argv[2]是目标服务器端套接字（门卫socket）的ip和端口号
    int clnt_sock;
    struct sockaddr_in serv_addr; //地址信息
    char message[30]; //保存从服务器端读取（接收）过来的数据
    if (argc != 3) { // 如果参数数量不为3（程序名称也是一个参数，还需要服务器门卫socket的ip和端口号两个参数）
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);  // 退出程序
    }
    //创建客户端socket
    clnt_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (clnt_sock == -1) error_handling("socket() errror");
    //初始化目标服务器套接字（门卫socket）的地址信息信息
    //不用初始化客户端套接字的地址信息，因为它们在之后调用connect()时会自动分配
    memset(&serv_addr, 0, sizeof(serv_addr)); //初始化serv_addr结构体为0
    serv_addr.sin_family = AF_INET; // 设置地址族
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]); //设置IP
    serv_addr.sin_port = htons(atoi(argv[2])); //设置端口
    //向服务器端发送连接请求，同时connect()自动会给客户端socket分配ip和端口号
    if (connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) error_handling("connect() error");
    //完成连接后，从与之对应的服务器连接socket中读取（接收）数据
    int i;
    for (i = 0; i < 100; ++i) printf("Wait time %d \n", i); 
    //由于服务器端是多次调用write()传输数据，客户端调用一次read()读取，而TCP套接字传输的数据是不存在数据边界的，因此客户端需延迟调用read()等待服务器端传输所有数据
    //这种让CPU执行多余认为以延迟代码运行的方式称为"Busy Waiting",使用得当就能成功延迟客户端read()函数的调用
    read(clnt_sock, &message, sizeof(message));
    printf("Message from server: %s \n", message); //打印读取（接收）的数据
    close(clnt_sock);
    return 0;
}
void error_handling(char* message) {
    perror(message);  // 打印错误信息
    exit(1);  // 退出程序
}
