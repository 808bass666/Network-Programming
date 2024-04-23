#include <stdio.h>      // Standard input/output functions
#include <stdlib.h>     // Standard library functions
#include <string.h>     // String handling functions
#include <unistd.h>     // Unix standard functions
#include <arpa/inet.h>  // IP address conversion functions
#include <sys/socket.h> // Socket functions
#define BUF_SIZE 1024   // Define a constant for the buffer size
void error_handling(char* message); // Function prototype for error handling

int main(int argc, char* argv[]) {
    //argv[0] is the program name,argv[1] and argv[2] are the IP and port for the target server socket (gatekeeper Socket)
    int clnt_sock;
    char message[BUF_SIZE]; //Buffer to hold messages
    struct sockaddr_in serv_adr; //Server address structure
    
    // Check if the correct number of command line arguments is provided
    if(argc != 3) { //Three parameters are required, one is the program name, and the other two are the IP and port number
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }   
    //Creates a TCP socket.
    clnt_sock = socket(PF_INET, SOCK_STREAM, 0); 
    if(clnt_sock == -1) error_handling("socket() error");
    //Initialize the server address structure
    memset(&serv_adr, 0, sizeof(serv_adr)); //Clears the structure for server address.   
	serv_adr.sin_family = AF_INET; //Sets the address family to IPv4
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]); //Sets the server IP address
    serv_adr.sin_port = htons(atoi(argv[2])); //Sets the server port.
    // Connects to the server,and connect() will also assign address info to client socket        
    if(connect(clnt_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1) error_handling("connect() error");    
    else puts("Connected!");
    int str_len;
    // Main loop for communication:
    while (1) {
        fputs("Input message(Q to quit): ", stdout);//standard output
        fgets(message, BUF_SIZE, stdin);//User input is read with fgets and sent to the server with write.        
		//communication is terminated when the user enters "q" or "Q"
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) break;
        // Send the message to the server
        write(clnt_sock, message, strlen(message));
        // Receive a response from the server
        str_len = read(clnt_sock, message, BUF_SIZE - 1);
        message[str_len] = 0;//将message中下标str_len的指设置为0(null),通过在接收到的数据后追加一个null终止符，可以确保对该数字的后续处理不会访问到未定义的内存区域
        printf("Message from server: %s", message);
    }
    close(clnt_sock);
    return 0;
}
// Function definition for error handling
void error_handling(char* message) {
    perror(message);  // print error info
    exit(1);  // exit the program
}
