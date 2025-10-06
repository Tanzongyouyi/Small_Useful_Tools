#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

// 全局变量
volatile sig_atomic_t pomodoro_count = 1; // 当前番茄钟序号
volatile sig_atomic_t completed_pomodoros = 0; // 已完成的番茄钟数
volatile sig_atomic_t current_session = 0; // 0:专注, 1:短休息, 2:长休息

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

        printf("%s", MOVE_CURSOR_TO_START);
        printf("%s[", COLOR_GREEN);
        printf("%s", progress_bar);
        printf("]%s", COLOR_RESET);
        printf(" %d%%", (int)(progress * 100));
        printf("%s", CLEAR_LINE);
        fflush(stdout);
        sleep(1);
    }
    printf("\n");
}

// 信号处理函数
void handle_signal(int sig) {
    if (sig == SIGINT) {
        printf("\n");
        // 如果当前正在专注阶段，当前番茄钟不算完成
        // 如果正在休息阶段，当前番茄钟算完成
        int total_completed = completed_pomodoros;
        if (current_session != 0) {
            total_completed++;
        }
        printf("坚持了%d个番茄钟，很厉害呢！\n", total_completed);
        exit(0);
    }
}

int main() {
    signal(SIGINT, handle_signal);

    printf("番茄钟已启动！按 Ctrl+C 可随时退出。\n");

    while (1) {
        printf("\n--- 第%d个番茄钟 ---\n", pomodoro_count);
        
        // 专注阶段
        current_session = 0;
        show_progress_bar(25 * 60, "专注工作 (25分钟)");
        
        // 专注完成，增加已完成番茄钟计数
        completed_pomodoros++;
        
        // 检查是否需要长休息 (每完成4个番茄钟后长休息)
        if (completed_pomodoros % 4 == 0) {
            printf("恭喜完成一组！奖励自己长休息 (20分钟)...\n");
            current_session = 2; // 进入长休息阶段
            show_progress_bar(20 * 60, "长休息 (20分钟)");
        } else {
            current_session = 1; // 进入短休息阶段
            show_progress_bar(5 * 60, "是时候休息了 (5分钟)");
        }
        
        // 准备下一个番茄钟
        pomodoro_count++;
        current_session = 0;
    }

    return 0;
}
