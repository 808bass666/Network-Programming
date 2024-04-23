#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig){ 
    //当信号SIGALRM发生时，会调用对应的handler(timeout函数)
    //信号SIGALRM什么时候会发生？当通过alarm()预约信号的时间到了时
    if (sig == SIGALRM) puts("Time Out!");
    alarm(2);
}
int main(int argc, char *argv[]) {
    //声明sigaction结构体变量act
    struct sigaction act;
    //初始化结构体变量
    act.sa_handler = timeout;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    
    //通过sigaction函数注册SIGALRM信号对应的handler
    sigaction(SIGALRM, &act, 0);
    
    //通过alarm()预约2s后发生SIGALRM信号
    alarm(2);
    
    //调用sleep函数使进程进入阻塞状态
    int i;
    for (i = 0; i < 3; ++i) {
        puts("wait...");
        sleep(100);//调用sleep后进程会进入阻塞状态
        //因为前面的预约，sleep没多久便会产生SIGALRM信号
        //为了调用对应的handler，OS将唤醒由于sleep而阻塞的进程
        //进程被唤醒后就不会再接着这次sleep了，即使规定的100s还没到。且下一次循环之前会一直处于被唤醒的状态，直到下一次循环调用sleep
    }
    //三次循环结束，会马上执行retunr 0而不会有两秒的时间等到alarm预约的信号发生
    return 0;
}
