#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int argc, char *argv[]){
	int fds1[2], fds2[2];
	char buf[BUF_SIZE];

	char str1[] = "Do you like coffee?";
	char str2[] = "I like coffee";
	char str3[] = "I like bread";
	char *str_arr[] = {str1, str2, str3};

	pipe(fds1), pipe(fds2);

	pid_t pid = fork();

	int i;
	if(pid == 0) {  //将三个字符串分三次传给父进程
		for(i = 0; i < 3; i++) {
			write(fds1[1], str_arr[i], strlen(str_arr[i]) + 1);
			read(fds2[0], buf, BUF_SIZE);
			printf("Child proc output: %s \n",  buf);
		}
	}
	else {  //分三次接收三个字符串
		for(i=0; i<3; i++) {
			read(fds1[0], buf, BUF_SIZE);
			printf("Parent proc output: %s \n", buf);
			write(fds2[1], str_arr[i], strlen(str_arr[i]) + 1);
		}
	}
	return 0;
}
