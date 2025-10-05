#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

// 全局变量，用于记录番茄钟数量
volatile sig_atomic_t pomodoro_count = 1;

// ANSI 转义码定义
#define COLOR_GREEN  "\033[0;32m"
#define COLOR_RESET  "\033[0m"
#define MOVE_CURSOR_TO_START "\r"
#define CLEAR_LINE    "\033[K"

// 进度条函数
void show_progress_bar(int duration, const char* task_str) {
    const int bar_width = 20;
    char progress_bar[bar_width + 1];

    printf("%s\n", task_str);

    for (int i = 0; i <= duration; ++i) {
        float progress = (float)i / duration;
        int pos = (int)(progress * bar_width);

        memset(progress_bar, '\0', sizeof(progress_bar));
        for (int j = 0; j < bar_width; ++j) {
            progress_bar[j] = (j < pos) ? '#' : ' ';
        }

        // --- 最终、最可靠的修正 ---
        // 将所有部分拆开，逐一打印，杜绝任何格式化错误
        printf("%s", MOVE_CURSOR_TO_START); // 1. 移动光标
        printf("%s[", COLOR_GREEN);         // 2. 打印绿色开括号
        printf("%s", progress_bar);         // 3. 打印进度条内容
        printf("]%s", COLOR_RESET);         // 4. 打印重置颜色的闭括号
        printf(" %d%%", (int)(progress * 100)); // 5. 打印百分比
        printf("%s", CLEAR_LINE);           // 6. 清除行尾
        // --- 修正结束 ---

        fflush(stdout);
        sleep(1);
    }
    printf("\n");
}

// 信号处理函数
void handle_signal(int sig) {
    if (sig == SIGINT) {
        printf("坚持了%d个番茄钟，很厉害呢！\n", pomodoro_count - 1);
        exit(0);
    }
}

int main() {
    signal(SIGINT, handle_signal);

    printf("番茄钟已启动！按 Ctrl+C 可随时退出。\n");

    while (1) {
        printf("\n--- 第%d个番茄钟 ---\n", pomodoro_count);
        
        show_progress_bar(25 * 60, "专注工作 (25分钟)");
        show_progress_bar(5 * 60, "是时候休息了 (5分钟)");

        if (pomodoro_count % 4 == 0) {
            printf("恭喜完成一组！奖励自己长休息 (20分钟)...\n");
            show_progress_bar(20 * 60, "长休息 (20分钟)");
        }
        
        pomodoro_count++;
    }

    return 0;
}