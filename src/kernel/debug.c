/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-13 11:25:52
 * @LastEditors: Python-Lettle 1071445082@qq.com
 * @LastEditTime: 2025-11-13 12:44:38
 * @Copyright: MIT License
 * @Description: 
 */
#include <snailix/debug.h>
#include <snailix/stdarg.h>
#include <snailix/string.h>
#include <snailix/stdio.h>
#include <snailix/printk.h>

extern void screen_print(char *buf, u32 count);

void print_prefix(const char * prefix, const char * fmt, ...)
{
    char buf[1024];

    va_list args;
    int i;

    va_start(args, fmt);

    i = vsprintf(buf, fmt, args);

    va_end(args);

    printk(prefix);
    screen_print(" ", 1);
    printk(buf);
}

#define PREFIX "[Snailix Kernel] "
#define PREFIX_LEN 17
static char buf[1024];
int kernel_info(const char *fmt, ...)
{
    if (!KERNEL_INFO_ON) return 0;
    
    va_list args;
    int i;

    va_start(args, fmt);

    i = vsprintf(buf, fmt, args);

    va_end(args);

    screen_print(PREFIX, PREFIX_LEN);
    screen_print(buf, i);

    return i;
}