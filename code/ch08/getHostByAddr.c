#include <stdio.h>     
#include <stdlib.h>   
#include <string.h>     
#include <unistd.h>    
#include <arpa/inet.h>
//#include <sys/socket.h> 
#include <netdb.h>

void error_handling(char* message); 

int main(int argc, char* argv[]) {
	//argv[1]是ip
    struct hostent *host; 
    struct sockaddr_in addr;

    if(argc != 2) { 
        printf("Usage : %s <IP>\n", argv[0]);
        exit(1);
    }   
   
    //初始化ipv4结构体地址变量,将传进来的参数ip填进去
	memset(&addr, 0, sizeof(addr));
	addr.sin_addr.s_addr = inet_addr(argv[1]);

	//传递含有ip信息的in_addr结构体指针将ip转换为域名
	host = gethostbyaddr((char*)&addr.sin_addr, 4, AF_INET);
	if(!host) error_handling("gethost... error");
	
	//打印地址结构体中的每个变量
	//(1) 打印h_name官方域名
	printf("Official name: %s \n", host->h_name);
    int i;
	//(2) 打印h_aliases除官方域名外的其它域名
	for(i = 0; host->h_aliases[i]; ++i) printf("Aliases %d: %s \n", i + 1, host->h_aliases[i]);
    //(3) 打印h_addr_list中的ip地址的地址族
	printf("Address Type: %s \n", (host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6");
	//(4) 省略打印h_length ip地址长度,IPv4地址是4，IPv6是16
	//(5) 打印h_addr_list 域名多个对应的IP地址
	for(i = 0; host->h_addr_list[i]; ++i) printf("IP addr %d: %s \n", i + 1, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
	return 0;
}
void error_handling(char* message) {
    herror(message);  
    exit(1);  
}
