
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h> 
#define BUF_SIZE 100 

int main(int argc, char *argv[]) {
    int src, dst;
    int read_cnt;
    char buf[BUF_SIZE];

    src = open(argv[1], O_RDONLY);
    dst = open(argv[2],O_CREAT|O_WRONLY|O_TRUNC);

    if (src == -1) {
        puts("src.txt open error");
        return -1;
    }
    if (dst == -1) {
        puts("dst.txt open error");
        return -1;
    }
    while ((read_cnt = read(src, buf, BUF_SIZE)) != 0) { //read_cnt接收read返回值（成功时返回接收到的字节数，失败返回-1）
        write(dst, buf, read_cnt);
    }
    close(src);
    close(dst);
    return 0;
}
