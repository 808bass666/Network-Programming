#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
	int status; 
	pid_t pid = fork();//创建第一个子进程
	if (pid == 0) return 3;//第一个子进程在main中执行return并返回值给操作系统，此时父进程还未向OS请求获取子进程返回值(exit的参值数、main的return的返回值)，该状态下的子进程就是僵尸进程
	else {
		printf("child PID: %d \n", pid);
		pid = fork();//创建第二个子进程
		if (pid == 0) exit(7);//第二个子进程调用exit()并传参给操作系统，此时父进程还未向OS···，该状态下的子进程就是僵尸进程
		else {
			printf("child PID: %d \n", pid);
			//利用wait()，父进程主动向OS请求获取子进程的返回值，销毁僵尸进程
			//销毁第一个子进程(僵尸进程)
			wait(&status); 
			if (WIFEXITED(status)) printf("Child send one: %d \n", WEXITSTATUS(status));
			//销毁第二个子进程(僵尸进程)
			wait(&status); 
			if (WIFEXITED(status)) printf("Child send two: %d \n", WEXITSTATUS(status));
			sleep(30); // sleep 30 sec
		}
	}
	return 0;
}

