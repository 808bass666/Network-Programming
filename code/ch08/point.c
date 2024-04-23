#include <stdio.h>

int main() {
    // 定义一个字符串数组
    char str1[] = "Hello";
    char str2[] = "World";
    char str3[] = "Example";

    // 定义一个指向字符串的指针数组
    char *strings[] = {str1, str2, str3};
	int i;
    // 访问指针数组中的每个字符串，并打印它们
    for (i = 0; i < 3; ++i) {
        printf("String %d: %s\n", i + 1, strings[i]);
    }

    return 0;
}

