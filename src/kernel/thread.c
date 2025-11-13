/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-01 10:26:58
 * @LastEditors: Python-Lettle 1071445082@qq.com
 * @LastEditTime: 2025-11-13 14:44:08
 * @Copyright: MIT License
 * @Description: Some threads required by the kernel are defined here.
 */
#include <snailix/task.h>
#include <snailix/printk.h>
#include <snailix/debug.h>
#include <snailix/stdlib.h>
#include <snailix/interrupt.h>
#include <snailix/syscall.h>

void _ofp idle_thread()
{
    set_interrupt_state(true);
    while(true)
    {
        asm volatile(
            "sti\n" // Enable interrupts
            "hlt\n" // Halt the CPU, waiting for an external interrupt
        );
        // Let other threads run.
        // yield();
    }
}

void _ofp sleep_thread()
{
    set_interrupt_state(true);
    while(true)
    {
        kernel_info("Sleep thread is running!\n");
        sleep(1000);
    }
}

void _ofp init_thread()
{
    set_interrupt_state(true);
    bool finish = false;
    while(true)
    {
        if (!finish)
        {
            kernel_info("Init thread's task is finish!\n");
            finish = true;
        }
        sleep(500);
    }
}