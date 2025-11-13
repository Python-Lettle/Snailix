/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-10 09:53:37
 * @LastEditors: Python-Lettle 1071445082@qq.com
 * @LastEditTime: 2025-11-13 14:22:52
 * @Copyright: MIT License
 * @Description: 
 */
#include <snailix/syscall.h>

static _inline u32 _syscall0(u32 nr)
{
    u32 ret;
    asm volatile(
        "int $0x80\n"
        : "=a"(ret)
        : "a"(nr));
    return ret;
}

static _inline u32 _syscall1(u32 nr, u32 arg)
{
    u32 ret;
    asm volatile(
        "int $0x80\n"
        : "=a"(ret)
        : "a"(nr), "b"(arg));
    return ret;
}

u32 test()
{
    return _syscall0(SYS_NR_TEST);
}

void yield()
{
    _syscall0(SYS_NR_YIELD);
}

void sleep(u32 ms)
{
    _syscall1(SYS_NR_SLEEP, ms);
}