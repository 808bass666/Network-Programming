#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	pid_t pid = fork();//创建子进程

	if (pid == 0) { // if child process
		puts("Hi, I am a child process");
	}
	else {
		printf("child process ID: %d \n", pid);
		sleep(30); // sleep 30 sec
	}
	if (pid == 0) puts("End child process");
	else puts("End parent process");
	return 0;
}

