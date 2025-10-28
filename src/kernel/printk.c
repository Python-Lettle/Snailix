/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-22 23:47:22
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-10-28 22:53:52
 * @Copyright: MIT License
 * @Description: 
 */
#include <snailix/stdarg.h>
#include <snailix/stdio.h>
#include <snailix/types.h>

static char * prefix = "[Snailix Kernel] ";
static int prefix_len = 17;

static char buf[1024];

extern void screen_print(char *buf, u32 count);

int printk(const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);

    i = vsprintf(buf, fmt, args);

    va_end(args);

    screen_print(buf, i);

    return i;
}

int kernel_info(const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);

    i = vsprintf(buf, fmt, args);

    va_end(args);

    screen_print(prefix, prefix_len);
    screen_print(buf, i);

    return i;
}