#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h> 
#define BUF_SIZE 100 //实践中通常使用4096B的缓冲区

//low_cp.c : first open,then read and write

int main(void) {
    int src, dst;//定义源文件描述符,目的文件描述符
    int read_cnt;//用于接收read返回值
    char buf[BUF_SIZE];
	//buf:是一个字符数组(或缓冲区)，用于暂存从源文件读取的数据，然后将这些数据写入目标文件。
	//BUF_SIZE表示缓冲区的大小,即一次文件I/O操作能读取或写入的最大的数据块大小,由BUF_SIZE指定。
	//过小的缓冲区可能导致频繁的 I/O 操作，降低性能；而过大的缓冲区可能导致内存占用增加。
    
	//1.以只读方式打开源文件
    src = open("src.txt", O_RDONLY);
    //2.打开目标文件 dst.txt，如果不存在则创建，以写入方式打开，如果文件已存在则截断文件
    dst = open("dst.txt",O_CREAT|O_WRONLY|O_TRUNC);
    //3.检查文件是否成功打开
    if (src == -1) {
        puts("src.txt open error");
        return -1;
    }
    if (dst == -1) {
        puts("dst.txt open error");
        return -1;
    }
    //4.连续不断地从源文件读取数据块，并将其写入目标文件，直到源文件的末尾(read函数返回0时,说明读取的内容长度为0了，表示已经读取完整个文件，循环就会终止，整个文件复制过程完成)
    //检查 read_cnt 是否不等于0，即是否成功读取了数据。如果成功读取了数据，循环条件为真，继续执行循环体。
    while ((read_cnt = read(src, buf, BUF_SIZE)) != 0) { //read_cnt接收read返回值（成功时返回接收到的字节数，失败返回-1）
        write(dst, buf, read_cnt);
    }
    //5.关闭打开的文件描述符
    close(src);
    close(dst);
    return 0;
}
