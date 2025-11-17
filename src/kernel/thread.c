/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-01 10:26:58
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-17 14:01:34
 * @Copyright: MIT License
 * @Description: Some threads required by the kernel are defined here.
 */
#include <snailix/task.h>
#include <snailix/printk.h>
#include <snailix/debug.h>
#include <snailix/stdlib.h>
#include <snailix/interrupt.h>
#include <snailix/syscall.h>
#include <snailix/stdio.h>
#include <snailix/arena.h>

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
        // printf("Thread is in user mode!\n");
        sleep(10);
    }
}

void init_thread()
{
    char temp [100];
    task_to_user_mode(user_mode_thread);
    // while(true);
}

void test_thread()
{
    u32 counter = 0;

    while (true)
    {
        // LOGK("test task %d....\n", counter++);
        void *ptr = kmalloc(1200);
        kernel_info("kmalloc 0x%p....\n", ptr);
        kfree(ptr);

        ptr = kmalloc(1024);
        kernel_info("kmalloc 0x%p....\n", ptr);
        kfree(ptr);

        ptr = kmalloc(54);
        kernel_info("kmalloc 0x%p....\n", ptr);
        kfree(ptr);

        sleep(5000);
    }
}