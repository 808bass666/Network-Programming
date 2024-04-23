#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
	int status; 
    pid_t child_pid;
	pid_t pid = fork();//创建子进程
	if (pid == 0){
        printf("我是子进程,我的PID=%d\n",getpid());
		sleep(10);//暂停10s后子进程再执行return并返回值给OS
		return 24;//子进程返回值
	}
	else { //父进程不会等待子进程，若没有已终止的子进程则返回0，由于父进程用了while循环不断的收集子进程，所以会一直收集直到有终止的子进程
        printf("我是父进程,我要等的进程id是%d\n",pid);
        do {
			child_pid = waitpid(pid, &status, WNOHANG);
            if (child_pid == 0) {
                puts("没有收集到子进程! sleep 3sec.");
				sleep(3); 
			}
        }while(child_pid == 0);//等不到就继续等 
        if (pid == child_pid) {
            puts("等到了子进程");
            if (WIFEXITED(status)) printf("我是父进程，我收集的子进程的返回值是: %d \n", WEXITSTATUS(status));//应该显示24
        }
        else puts("出错了");
	}
	return 0;
}
