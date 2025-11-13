/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-01 10:26:58
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-10 20:54:38
 * @Copyright: MIT License
 * @Description: Some threads required by the kernel are defined here.
 */
#include <snailix/task.h>
#include <snailix/printk.h>
#include <snailix/stdlib.h>
#include <snailix/interrupt.h>

extern void schedule();

extern task_t *block_task;
extern u8 block_time;
void _ofp idle_thread()
{
    set_interrupt_state(true);
    u32 counter = 1;
    while(true)
    {
        // kernel_info("Idle thread runned %d times\n", counter++);
        if (block_task->state == TASK_BLOCKED && block_time >= 3)
        {
            set_interrupt_state(false);
            task_unblock(block_task);
            set_interrupt_state(true);
        }
        // some delay
        delay(100000000);
    }
}

void _ofp self_block_thread()
{
    set_interrupt_state(true);
    u32 counter = 0;
    while(true)
    {
        counter++;
        if (counter == 2) {
            counter = 0;
            set_interrupt_state(false);
            task_block(block_task, NULL, TASK_BLOCKED);
            set_interrupt_state(true);
        }
        else
        {
            // This thread will be blocked after running twice
            delay(100000000);
        }
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
    }
}