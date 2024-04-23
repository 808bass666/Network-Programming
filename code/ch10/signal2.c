#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig){ 
    //当信号SIGALRM发生时，会调用对应的handler(timeout函数)
    //信号SIGALRM什么时候会发生？当通过alarm()预约信号的时间到了时
    if (sig == SIGALRM) puts("Time Out!");
    alarm(2);
}
void keycontrol(int sig){
    //当信号SIGINT发生时，OS就会调用对应的handler(keycontrol函数)
    //信号SIGINT什么时候会发生？输入CTRL+C时
    if (sig == SIGINT) puts("CTRL+C pressed");
}
int main(int argc, char *argv[]) {
    //注册信号及对应的handler
    signal(SIGALRM, timeout);
    signal(SIGINT, keycontrol);
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
    //三次循环结束，又会进到下面的sleep,但因为前面的最后一次循环有alarm，因此不到两秒的时间进程就会被唤醒然后直接执行下一句return 0
	puts("sleep again.................");
	sleep(100);
    return 0;
}
