/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-01 10:26:58
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-15 14:47:46
 * @Copyright: MIT License
 * @Description: Some threads required by the kernel are defined here.
 */
#include <snailix/task.h>
#include <snailix/printk.h>
#include <snailix/debug.h>
#include <snailix/stdlib.h>
#include <snailix/interrupt.h>
#include <snailix/syscall.h>

void idle_thread()
{
    set_interrupt_state(true);
    while(true)
    {
        asm volatile(
            "sti\n" // Enable interrupts
            "hlt\n" // Halt the CPU, waiting for an external interrupt
        );
        // Let other threads run.
        yield();
    }
}

void sleep_thread()
{
    set_interrupt_state(true);
    while(true)
    {
        // kernel_info("Sleep thread is running!\n");
        // sleep(1000);
        yield();
    }
}

extern u32 keyboard_read(char *buf, u32 count);

void user_mode_thread()
{
    char ch;
    while(true)
    {
        sleep(100);
    }
}

void init_thread()
{
    char temp [100];
    task_to_user_mode(user_mode_thread);
    // while(true);
}