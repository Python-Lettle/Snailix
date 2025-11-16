/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-17 00:07:37
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-17 00:07:46
 * @Copyright: MIT License
 * @Description: printf function
 */
#include <snailix/stdarg.h>
#include <snailix/stdio.h>
#include <snailix/syscall.h>

static char buf[1024];

int printf(const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);

    i = vsprintf(buf, fmt, args);

    va_end(args);

    write(stdout, buf, i);

    return i;
}