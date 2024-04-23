#include <stdio.h>
#include <stdlib.h>

int main() {
    // 创建了一个3x5的二维字符数组
    int rows = 3;
    int cols = 5;
	int i,j;
    // 分配内存以存储二维字符数组
    char **matrix = (char **)malloc(rows * sizeof(char *));
    //matrix 是一个指向指针的指针，用于存储一个二维字符数组
    
    // 分配内存以存储每行的字符数组
    for (i = 0; i < rows; ++i) matrix[i] = (char *)malloc(cols * sizeof(char));
  
    // 初始化二维字符数组
    for (i = 0; i < rows; ++i) {
        for (j = 0; j < cols; ++j) matrix[i][j] = 'A' + i * cols + j;
    }

    // 访问并打印二维字符数组的内容
    for (i = 0; i < rows; ++i) {
        for (j = 0; j < cols; ++j) {
            printf("%c ", matrix[i][j]);
        }
        printf("\n");
    }

    // 释放分配的内存
    for (i = 0; i < rows; ++i) {
        free(matrix[i]);
    }
    free(matrix);

    return 0;
}
