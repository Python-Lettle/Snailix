/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-29 13:13:52
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-01 13:13:09
 * @Copyright: MIT License
 * @Description: 
 */
#include <snailix/task.h>
#include <snailix/printk.h>

#define NR_TASKS  64
#define PAGE_SIZE 0x1000

extern void task_switch(task_t *next);

// Task table
static task_t *task_table[NR_TASKS];
// Idle task, it may do some initialization.
// TODO: Temporarily allocate some memory.
static task_t *init_task = (task_t *)0x1000;
// Idle task, do nothing.
static task_t *idle_task = (task_t *)0x2000;

task_t *running_task()
{
    asm volatile(
        "movl %esp, %eax\n"
        "andl $0xfffff000, %eax\n");
}

void schedule()
{
    task_t *current = running_task();
    task_t *next = current==init_task ? idle_task : init_task;
    task_switch(next);
}

static task_t *task_create(task_t *task, target_t target)
{    
    u32 stack = (u32)task + PAGE_SIZE;

    stack -= sizeof(task_frame_t);
    task_frame_t *frame = (task_frame_t *)stack;
    frame->ebx = 0x11111111;
    frame->esi = 0x22222222;
    frame->edi = 0x33333333;
    frame->ebp = 0x44444444;
    frame->eip = (void *)target;

    task->stack = (u32 *)stack;

    return task;
}

extern void idle_thread();
extern void init_thread();

void task_init()
{
    task_create(init_task, init_thread);
    task_create(idle_task, idle_thread);
    schedule();
}