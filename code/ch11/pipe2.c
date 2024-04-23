#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int argc, char *argv[]){
	int fds[2];
	char str[]="Who are you?";
	char str2[]="Thank you for your message";
	char buf[BUF_SIZE];
	pid_t pid;

	pipe(fds);

	pid=fork();

	if(pid==0){
		write(fds[1], str, sizeof(str));
		sleep(2);//若不加这一行则子进程会把本来要通过管道传给父进程的数据先取走,导致父进程取不到
		read(fds[0], buf, BUF_SIZE);
		printf("Child proc output: %s \n", buf);
	}
	else{
		read(fds[0], buf, BUF_SIZE);
		printf("Parent proc output: %s \n", buf);
		write(fds[1], str2, sizeof(str2));
		sleep(3);//这里和前面子进程中sleep(2)的作用不一样，这里只是为了防止父进程先结束，导致父进程输出完到子进程打印输出(终止前)的这中间，会出现终端的命令提示符显示（也就是并非必要但是加上最好）
	}
	return 0;
}
