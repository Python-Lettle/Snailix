/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-29 13:13:52
 * @LastEditors: Python-Lettle 1071445082@qq.com
 * @LastEditTime: 2025-11-13 10:02:36
 * @Copyright: MIT License
 * @Description: 
 */
#include <snailix/task.h>
#include <snailix/debug.h>
#include <snailix/printk.h>
#include <snailix/string.h>
#include <snailix/syscall.h>
#include <snailix/interrupt.h>
#include <snailix/assert.h>
#include <snailix/memory.h>

#define NR_TASKS  64

extern void task_switch(task_t *next);

// Task table
static u32 running_index = 0;
static u32 task_count = 0;
static task_t *task_table[NR_TASKS];
static LinkedList block_list;           // Default blocked list
// Idle task, it may do some initialization.
// TODO: Temporarily allocate some memory.
static task_t *init_task = (task_t *)0x1000;
// Idle task, do nothing.
static task_t *idle_task = (task_t *)0x2000;
task_t *block_task = (task_t *)0x3000;
u8 block_time = 0;

// Search for a task with the specified state, except the current running task.
static task_t *task_search(task_state_t state)
{
    assert(!get_interrupt_state());
    task_t *task = NULL;
    task_t *current = running_task();

    for (size_t i = 0; i < NR_TASKS; i++)
    {
        task_t *ptr = task_table[i];
        if (ptr == NULL)
            continue;

        if (ptr->state != state)
            continue;
        if (current == ptr)
            continue;
        if (task == NULL || task->ticks < ptr->ticks || ptr->jiffies < task->jiffies)
            task = ptr;
    }

    return task;
}

task_t *running_task()
{
    task_t *task;
    asm volatile(
        "movl %%esp, %%eax\n"
        "andl $0xfffff000, %%eax\n"
        "movl %%eax, %0" : "=a"(task));
    return task;
}

void schedule()
{
    // Make sure interrupts are disabled
    assert(!get_interrupt_state());

    // Get current task
    task_t *current = running_task();

    // Find next ready task, round-robin scheduling.
    task_t *next = task_search(TASK_READY);

    assert(next != NULL && next->state == TASK_READY && next->magic == SNAILIX_MAGIC);

    if (list_empty(&block_list))
    {
        block_time = 0;
    }
    else
    {
        block_time++;
    }

    next->ticks = 150;
    next->state = TASK_RUNNING;
    current->state = current->state == TASK_BLOCKED ? TASK_BLOCKED : TASK_READY;
    printk("[Schedule] Switch task [%s]-> [%s]\n", current->name, next->name);
    task_switch(next);
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
    task->state = TASK_READY;
    task->priority = priority;
    task->ticks = 150;
    strcpy(task->name, name);
    task->uid = uid;
    task->pde = 0x1000;  // Set to kernel page directory physical address
    task->vmap = NULL;   // No user space yet
    task->brk = 0;

    task->magic = SNAILIX_MAGIC;

    task_count++;
    return task;
}

static void task_setup()
{
    task_t *task = running_task();
    task->magic = SNAILIX_MAGIC;
    task->ticks = 100;

    memset(task_table, 0, sizeof(task_table));
}


void task_block(task_t *task, LinkedList *blist, task_state_t state)
{
    // Make sure interrupts are disabled
    assert(!get_interrupt_state());
    // And make sure list node is not in any list. ( NULL <-- block_node --> NULL )
    assert(task->block_node.prev == NULL && task->block_node.next == NULL);

    if (blist == NULL) blist = &block_list;

    list_lpush(blist, &task->block_node);

    task->state = state;

    kernel_info("Task [%s] is blocked!\n", task->name);

    task_t *current = running_task();
    if (current == task) schedule();
}

void task_unblock(task_t *task)
{
    // Make sure interrupts are disabled
    assert(!get_interrupt_state());

    list_remove(&task->block_node);

    assert(task->block_node.prev == NULL && task->block_node.next == NULL);

    task->state = TASK_READY;

    kernel_info("Task [%s] is unblocked!\n", task->name);

    task_t *current = running_task();
    if (current == task) schedule();
}

void task_yield()
{
    schedule();
}

extern void idle_thread();
extern void init_thread();
extern void self_block_thread();

void task_init()
{
    list_init(&block_list);

    task_create(init_task, "init_task", 1, KERNEL_USER, init_thread);
    task_create(idle_task, "idle_task", 5, KERNEL_USER, idle_thread);
    task_create(block_task, "block_task", 5, KERNEL_USER, self_block_thread);

    task_table[0] = init_task;
    task_table[1] = idle_task;
    task_table[2] = block_task;

    schedule();
}