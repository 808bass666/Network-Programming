#include <stdio.h>
#include <stdlib.h>

int main(void) {
    FILE*src,*dst;
    int read_cnt;
    src = fopen("src.txt", "r");
    dst = fopen("dst.txt","w");

    if (src == NULL) {
        puts("src.txt open error");
        return -1;
    }
    if (dst == NULL) {
        puts("dst.txt open error");
        return -1;
    }
    while ((read_cnt = fgetc(src)) != EOF) {
        fputc(read_cnt,dst);
    }
    fclose(src);
    fclose(dst);
    return 0;
}
