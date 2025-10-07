#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define BUFFER_SIZE 4096

// 函数声明
long count_lines(FILE *file);
int skip_lines(FILE *file, long n, FILE *output);
int copy_remaining(FILE *input, FILE *output, long lines_to_keep);
void print_usage(const char *program_name);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        print_usage(argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    long n = atol(argv[2]);  // 从前面删除的行数
    long m = atol(argv[3]);  // 从后面删除的行数

    // 参数验证
    if (n < 0 || m < 0) {
        fprintf(stderr, "错误: 参数不能为负数\n");
        return 1;
    }

    if (n == 0 && m == 0) {
        fprintf(stderr, "提示: 两个参数都为0，输出文件将与输入文件相同\n");
    }

    FILE *input = fopen(filename, "rb");
    if (!input) {
        fprintf(stderr, "错误: 无法打开文件 '%s'\n", filename);
        return 1;
    }

    // 创建输出文件名
    char output_filename[MAX_PATH];
    const char *dot = strrchr(filename, '.');
    const char *base_name = filename;
    
    if (dot) {
        size_t base_len = dot - filename;
        strncpy(output_filename, filename, base_len);
        output_filename[base_len] = '\0';
        sprintf(output_filename + base_len, "_cut_%ld_%ld%s", n, m, dot);
    } else {
        sprintf(output_filename, "%s_cut_%ld_%ld", filename, n, m);
    }

    FILE *output = fopen(output_filename, "wb");
    if (!output) {
        fprintf(stderr, "错误: 无法创建输出文件 '%s'\n", output_filename);
        fclose(input);
        return 1;
    }

    printf("处理文件: %s\n", filename);
    printf("输出文件: %s\n", output_filename);
    printf("删除前 %ld 行，删除后 %ld 行\n", n, m);

    // 第一步：跳过前n行
    if (n > 0) {
        printf("跳过前 %ld 行...\n", n);
        if (!skip_lines(input, n, NULL)) {
            fprintf(stderr, "错误: 文件行数不足 %ld 行\n", n);
            fclose(input);
            fclose(output);
            remove(output_filename);
            return 1;
        }
    }

    // 如果不需要删除后面的行，直接复制剩余内容
    if (m == 0) {
        printf("复制剩余内容...\n");
        char buffer[BUFFER_SIZE];
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, input)) > 0) {
            fwrite(buffer, 1, bytes_read, output);
        }
    } else {
        // 需要删除后m行，先计算总行数
        printf("计算文件行数...\n");
        
        // 获取当前位置
        long current_pos = ftell(input);
        
        // 计算剩余行数
        long remaining_lines = 0;
        char buffer[BUFFER_SIZE];
        
        while (fgets(buffer, BUFFER_SIZE, input)) {
            remaining_lines++;
        }
        
        // 回到跳过前n行后的位置
        fseek(input, current_pos, SEEK_SET);
        
        printf("剩余行数: %ld\n", remaining_lines);
        
        if (remaining_lines <= m) {
            fprintf(stderr, "错误: 剩余行数 %ld 不足要删除的后 %ld 行\n", remaining_lines, m);
            fclose(input);
            fclose(output);
            remove(output_filename);
            return 1;
        }
        
        // 复制前 (remaining_lines - m) 行
        long lines_to_keep = remaining_lines - m;
        printf("保留前 %ld 行...\n", lines_to_keep);
        
        if (!copy_remaining(input, output, lines_to_keep)) {
            fprintf(stderr, "错误: 复制文件时发生错误\n");
            fclose(input);
            fclose(output);
            remove(output_filename);
            return 1;
        }
    }

    fclose(input);
    fclose(output);

    // 验证输出文件大小
    FILE *check = fopen(output_filename, "rb");
    if (check) {
        fseek(check, 0, SEEK_END);
        long size = ftell(check);
        fclose(check);
        printf("输出文件大小: %ld 字节\n", size);
    }

    printf("文件切割完成!\n");
    return 0;
}

// 计算文件总行数
long count_lines(FILE *file) {
    long lines = 0;
    char buffer[BUFFER_SIZE];
    long current_pos = ftell(file);
    
    fseek(file, 0, SEEK_SET);
    
    while (fgets(buffer, BUFFER_SIZE, file)) {
        lines++;
    }
    
    fseek(file, current_pos, SEEK_SET);
    return lines;
}

// 跳过n行
int skip_lines(FILE *file, long n, FILE *output) {
    char buffer[BUFFER_SIZE];
    long lines_skipped = 0;
    
    while (lines_skipped < n && fgets(buffer, BUFFER_SIZE, file)) {
        lines_skipped++;
        if (output) {
            fputs(buffer, output);
        }
    }
    
    return lines_skipped == n;
}

// 复制指定行数
int copy_remaining(FILE *input, FILE *output, long lines_to_keep) {
    char buffer[BUFFER_SIZE];
    long lines_copied = 0;
    
    while (lines_copied < lines_to_keep && fgets(buffer, BUFFER_SIZE, input)) {
        fputs(buffer, output);
        lines_copied++;
    }
    
    return lines_copied == lines_to_keep;
}

// 打印使用说明
void print_usage(const char *program_name) {
    printf("文件切割工具\n");
    printf("用法: %s 文件名 前删除行数 后删除行数\n", program_name);
    printf("参数说明:\n");
    printf("  文件名      : 要处理的文件路径\n");
    printf("  前删除行数  : 从文件开头删除的行数 (0表示不删除)\n");
    printf("  后删除行数  : 从文件末尾删除的行数 (0表示不删除)\n");
    printf("示例:\n");
    printf("  %s data.txt 10 5    删除前10行和后5行\n", program_name);
    printf("  %s data.txt 0 100   只删除后100行\n", program_name);
    printf("  %s data.txt 50 0    只删除前50行\n", program_name);
}