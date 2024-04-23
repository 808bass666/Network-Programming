#include <stdio.h>
#include <unistd.h>

int gval = 10;
int main(int argc, char* argv[]) {
	pid_t pid;
	int lval = 20;
	gval++, lval += 5; //gval = 11, lval = 25

	pid = fork();//创建子进程

	if (pid == 0) { // if child process
		gval += 2, lval += 2; 
		printf("Child Proc: [%d, %d] \n", gval, lval);//[13,27]
	}
	else {
		gval -= 2, lval -= 2;//if parent process
		printf("Parent Proc: [%d, %d] \n", gval, lval);//[9,23]
	}
	return 0;
}

