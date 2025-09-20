#include <stdio.h>
#include <stdlib.h> // 用于 atof() 和 atoi()
#include <string.h> // 用于 strcmp()

// 函数声明
void print_about_info();

int main() {
    char input_buffer[100]; // 用于存储用户输入的缓冲区
    double x1, x2, piece, result, current_x;
    int times;

    printf("Input \"about\" for more information\n");

    // C语言中没有直接等效于Python的 `while True`。
    // 通常使用 `while(1)` 或 `for(;;)` 来创建无限循环。
    while (1) {
        printf("x1: ");
        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
            // 处理输入错误或文件结束符 (EOF, 例如 Ctrl+D)
            break; 
        }

        // 移除fgets读取的末尾换行符
        input_buffer[strcspn(input_buffer, "\n")] = 0;

        // 检查用户是否输入了 "about"
        // C语言中字符串比较用 strcmp()，返回0表示相等
        if (strcmp(input_buffer, "about") == 0) {
            print_about_info();
            continue; // 跳过本次循环的剩余部分
        }

        // 将字符串转换为 double 类型
        // atof() 不会进行错误检查，更安全的做法是使用 strtod()
        x1 = atof(input_buffer);

        printf("x2: ");
        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) break;
        input_buffer[strcspn(input_buffer, "\n")] = 0;
        x2 = atof(input_buffer);

        printf("times: ");
        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) break;
        input_buffer[strcspn(input_buffer, "\n")] = 0;
        times = atoi(input_buffer); // 将字符串转换为 int 类型

        // 边界条件检查
        if (x1 >= x2 || x1 * x2 < 0) {
            printf("There is no Math in the world.\n");
            continue;
        }

        // 核心计算逻辑
        piece = (x2 - x1) / times;
        result = 0.0;
        current_x = x1;

        // C语言的for循环结构与Python非常相似
        for (int i = 0; i < times; i++) {
            double y = 1.0 / current_x;
            result += y * piece; // C语言中累加是 +=
            current_x += piece;
        }

        // C语言中打印浮点数使用 %f 格式说明符
        // %.15f 可以打印更多小数位，以匹配Python的精度显示
        printf("S = %.15f\n", result);
    }

    return 0; // main函数返回0表示程序成功执行
}

// 将打印"about"信息的代码封装成一个函数，使main函数更清晰
void print_about_info() {
    printf("Made by Tanzongyouyi@GitHub\n");
    printf("E-mail : hi@tanzong.dpdns.org\n");
    printf("x1 < x2!!!\n");
}
