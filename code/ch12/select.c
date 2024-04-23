#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#define BUF_SIZE 30

int main(int argc, char *argv[]){
  int res, str_len;
  char buf[BUF_SIZE];

  fd_set reads, temps;
  struct timeval timeout;

  FD_ZERO(&reads);
  FD_SET(0, &reads);//fd=0表示标准输入的文件描述符

  while (1) {
    temps = reads;//复制fd_set变量
    
    timeout.tv_sec = 5;//设置超时时间为5 s + 0 ms
    timeout.tv_usec = 0;
    
    res = select(1, &temps, 0, 0, &timeout);
    // 将所有需要关注"是否存在待读取数据"的fd注册到tmp(这里第一个参数为1，只有一个要关注的文件描述符:fd=0)

    
    if (res == -1) { //发生错误select返回-1
      puts("select() error!");
      break;
    }
    //超时select返回0
    else if (res == 0) puts("Time-out!");
    else { //引关注的事件返回时select返回>0的值
      if (FD_ISSET(0, &temps)) { //若temps中文件描述符fd=0(标准输入)发生变化(这里的变化即存在待读取数据)
        // 则从fd=0(标准输入)中读取数据并输出
        str_len = read(0, buf, BUF_SIZE); 
        buf[str_len] = '\0';//read不会在读取的数据后自动添加字符串结束符，因此我们要在读取到的数据后加上字符串结束符(\0)，确保输出的字符串正确结束。
        printf("message from console: %s", buf);
      }
    }
  }
}
