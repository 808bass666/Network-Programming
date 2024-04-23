#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void read_childproc(int sig){ 
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    if (WIFEXITED(status)) {
        printf("Removed child_PID: %d \n", pid);
        printf("child return: %d \n", WEXITSTATUS(status));
    }
}
int main(int argc, char *argv[]) {
    pid_t pid;
    //声明sigaction结构体变量act
    struct sigaction act;
    //初始化结构体变量act
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    
    //通过sigaction函数注册SIGALRM信号对应的handler
    sigaction(SIGCHLD, &act, 0);
    
    pid = fork(); //fork第一个子进程A
    if(pid == 0) {
        printf("Hi！I'm first child %d \n", getpid());
        sleep(10);
        return 12;
    }
    else {
        printf("first child_PID: %d \n", pid);
        pid = fork(); //fork第二个子进程B
        if(pid == 0) {
            printf("Hi！I'm second child %d \n", getpid());
            sleep(10);
            exit(24);
        }
        else {
            int i;
            printf("second child_PID: %d \n", pid);
            for (i = 0; i < 5; ++i) {
                puts("wait...");
                sleep(5);
            }
        }
    }
    return 0;
}
