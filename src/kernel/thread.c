/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-01 10:26:58
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-10 13:55:19
 * @Copyright: MIT License
 * @Description: Some threads required by the kernel are defined here.
 */
#include <snailix/task.h>
#include <snailix/printk.h>

extern void schedule();

void _ofp idle_thread()
{
    asm volatile("sti\n");
    u32 counter = 1;
    while(true)
    {
        // kernel_info("Idle thread runned %d times\n", counter++);
        
        // some delay
        for (u32 i=0; i<100000000; i++) {}
    }
}

void _ofp init_thread()
{
    asm volatile("sti\n");

    char temp[100];
    bool finish = false;
    while(true)
    {
        if (!finish)
        {
            kernel_info("Init thread's task is finish!\n");
            finish = true;
        }
    }
}