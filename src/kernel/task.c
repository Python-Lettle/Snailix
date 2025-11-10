/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-29 13:13:52
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-10 14:09:56
 * @Copyright: MIT License
 * @Description: 
 */
#include <snailix/task.h>
#include <snailix/printk.h>
#include <snailix/string.h>
#include <snailix/syscall.h>
#include <snailix/interrupt.h>

#define NR_TASKS  3
#define PAGE_SIZE 0x1000

extern void task_switch(task_t *next);

// Task table
static u32 running_index = 0;
static task_t *task_table[NR_TASKS];
// Idle task, it may do some initialization.
// TODO: Temporarily allocate some memory.
static task_t *init_task = (task_t *)0x1000;
// Idle task, do nothing.
static task_t *idle_task = (task_t *)0x2000;
static task_t *idle_task2 = (task_t *)0x3000;

task_t *running_task()
{
    asm volatile(
        "movl %esp, %eax\n"
        "andl $0xfffff000, %eax\n");
}

void schedule()
{
    // Make sure interrupts are disabled
    set_interrupt_state(false);
    
    task_t *current = running_task();
    task_t *next = task_table[running_index];

    running_index = (running_index + 1) % NR_TASKS;

    next->ticks = 100;
    task_switch(next);
    printk("Switch task from %s to %s\n", current->name, next->name);

    set_interrupt_state(true);
}

static task_t *task_create(task_t *task, const char * name, u32 priority, u32 uid, target_t target)
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

    task->magic = SNAILIX_MAGIC;
    strcpy(task->name, name);
    task->priority = priority;
    task->uid = uid;

    task->ticks = 50;

    return task;
}

void task_yield()
{
    schedule();
}

extern void idle_thread();
extern void init_thread();

void task_init()
{
    task_create(init_task, "init_task", 1, KERNEL_USER, init_thread);
    task_create(idle_task, "idle_task", 5, KERNEL_USER, idle_thread);
    task_create(idle_task2, "idle_task2", 5, KERNEL_USER, idle_thread);

    task_table[0] = init_task;
    task_table[1] = idle_task;
    task_table[2] = idle_task2;

    schedule();
}