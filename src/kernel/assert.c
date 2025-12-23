/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-22 23:24:40
 * @LastEditors: Lettle 1071445082@qq.com
 * @LastEditTime: 2025-12-23 13:26:50
 * @Copyright: MIT License
 * @Description: 
 */
#include <snailix/assert.h>
#include <snailix/stdarg.h>
#include <snailix/types.h>
#include <snailix/stdio.h>
#include <snailix/printk.h>

static u8 buf[1024];

// Block the kernel here forever.
static void spin(char *name)
{
    kernel_info("Kernel spinning in %s forever ...\n", name);
    while (true);
}

// Assertion failure.
void assertion_failure(char *exp, char *file, char *base, int line)
{
    kernel_info(
        "assert(%s) failed!!!\n"
        "    --> file: %s \n"
        "    --> base: %s \n"
        "    --> line: %d \n",
        exp, file, base, line);

    spin("assertion_failure()");

    // This will never be executed.
    asm volatile("ud2");
}

// There is something wrong, panic.
void panic(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int i = vsprintf(buf, fmt, args);
    va_end(args);

    kernel_info("!!! panic !!!\n--> %s \n", buf);
    spin("panic()");

    // This will never be executed.
    asm volatile("ud2");
}