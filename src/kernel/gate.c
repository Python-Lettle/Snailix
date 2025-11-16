/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-10 09:58:11
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-16 23:54:04
 * @Copyright: MIT License
 * @Description: 
 */
#include <snailix/interrupt.h>
#include <snailix/assert.h>
#include <snailix/printk.h>
#include <snailix/syscall.h>
#include <snailix/task.h>

#define SYSCALL_SIZE 64

handler_t syscall_table[SYSCALL_SIZE];

void syscall_check(u32 nr)
{
    if (nr >= SYSCALL_SIZE)
    {
        panic("syscall nr error!!!");
    }
}

static void sys_default()
{
    panic("syscall not implemented!!!");
}

static u32 sys_test()
{
    kernel_info("syscall test...\n");
    return 255;
}

extern int32 screen_print(char *buf, u32 count);
int32 sys_write(fd_t fd, char *buf, u32 len)
{
    if (fd == stdout || fd == stderr)
    {
        return screen_print(buf, len);
    }
    // todo
    panic("write!!!!");
    return 0;
}

void syscall_init()
{
    for (size_t i = 0; i < SYSCALL_SIZE; i++)
    {
        syscall_table[i] = sys_default;
    }

    syscall_table[SYS_NR_TEST] = sys_test;
    syscall_table[SYS_NR_SLEEP] = task_sleep;
    syscall_table[SYS_NR_YIELD] = task_yield;
    syscall_table[SYS_NR_WRITE] = sys_write;
}
