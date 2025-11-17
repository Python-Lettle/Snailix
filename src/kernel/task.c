/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-29 13:13:52
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-17 14:03:58
 * @Copyright: MIT License
 * @Description: 
 */
#include <snailix/assert.h>
#include <snailix/task.h>
#include <snailix/debug.h>
#include <snailix/printk.h>
#include <snailix/string.h>
#include <snailix/syscall.h>
#include <snailix/interrupt.h>
#include <snailix/assert.h>
#include <snailix/memory.h>
#include <snailix/global.h>
#include <snailix/bitmap.h>

#define NR_TASKS  64
extern u32 volatile jiffies;
extern u32 jiffy;
extern bitmap_t kernel_map;
extern tss_t tss;

extern void task_switch(task_t *next);

// Task table
static u32 running_index = 0;
static u32 task_count = 0;
static task_t *task_table[NR_TASKS];
static LinkedList block_list;           // Default blocked list
static LinkedList sleep_list;           // Default sleep list
// Idle task, do nothing. When task_search returns NULL, it will be scheduled.
static task_t *idle_task;

// Get a free task from the task table.
static task_t *get_free_task()
{
    for (size_t i = 0; i < NR_TASKS; i++)
    {
        if (task_table[i] == NULL)
        {
            task_table[i] = (task_t *)alloc_kpage(1); // todo free_kpage
            return task_table[i];
        }
    }
    panic("No more tasks");
}

// Search for a task with the specified state, except the current running task.
static task_t *task_search(task_state_t state)
{
    // Make sure interrupts are disabled
    assert(!get_interrupt_state());

    task_t *task = NULL;
    task_t *current = running_task();

    // Search for a task with the specified state
    static size_t i = 0;        // A global index to make sure all task will be scheduled equally.
    size_t i_backup = i;
    // Force i++ to make sure it will not be stucked in 0 forever.
    i = (i + 1) % NR_TASKS;
    while ((i = (i+1)%NR_TASKS) != i_backup)
    {
        task_t *ptr = task_table[i];
        if (ptr == NULL)
            continue;

        if (ptr->state != state)
            continue;
        
        // If enabled, skip the current running task.
        if (current == ptr)
            continue;
        if (task == NULL || task->ticks < ptr->ticks || ptr->jiffies < task->jiffies)
        {
            task = ptr;
            break;
        }
        
        i = (i + 1) % NR_TASKS;
    } 

    // If no task found, return idle task.
    if (task == NULL)
        task = idle_task;

    return task;
}

void task_activate(task_t *task)
{
    assert(task->magic == SNAILIX_MAGIC);

    if (task->uid != KERNEL_USER)
    {
        tss.esp0 = (u32)task + PAGE_SIZE;
    }
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

    assert(next != NULL);

    next->ticks = 150;
    next->state = TASK_RUNNING;
    if (current->state == TASK_RUNNING)
    {
        current->state = TASK_READY;
    }
    // print_prefix("[Schedule]", "Switch task [%s]-> [%s]\n", current->name, next->name);
    task_activate(next);
    task_switch(next);
}

static task_t *task_create(target_t target, const char * name, u32 priority, u32 uid)
{    
    task_t *task = get_free_task();
    u32 stack = (u32)task + PAGE_SIZE;

    stack -= sizeof(task_frame_t);
    task_frame_t *frame = (task_frame_t *)stack;
    frame->ebx = 0x11111111;
    frame->esi = 0x22222222;
    frame->edi = 0x33333333;
    frame->ebp = 0x44444444;
    frame->eip = (void *)target;

    task->stack = (u32 *)stack;

    node_init(&task->block_node, task);

    task->state = TASK_READY;
    task->priority = priority;
    task->ticks = 150;
    strcpy(task->name, name);
    task->uid = uid;
    task->pde = KERNEL_PAGE_DIR;  // Set to kernel page directory physical address
    task->vmap = NULL;   // No user space yet
    task->brk = 0;

    task->magic = SNAILIX_MAGIC;

    task_count++;
    return task;
}

void task_to_user_mode(target_t target)
{
    task_t *task = running_task();

    // Ensure task's page directory is set to the kernel page directory
    // which now has user access enabled for the first 16 pages
    task->pde = KERNEL_PAGE_DIR;
    
    // Set CR3 to the kernel page directory
    // This is necessary to ensure the page table with user access bits is active
    asm volatile("movl %0, %%cr3" : : "a"(KERNEL_PAGE_DIR));

    u32 addr = (u32)task + PAGE_SIZE;

    addr -= sizeof(intr_frame_t);
    intr_frame_t *iframe = (intr_frame_t *)(addr);

    iframe->vector = 0x20;
    iframe->edi = 1;
    iframe->esi = 2;
    iframe->ebp = 3;
    iframe->esp_dummy = 4;
    iframe->ebx = 5;
    iframe->edx = 6;
    iframe->ecx = 7;
    iframe->eax = 8;

    iframe->gs = 0;
    iframe->ds = USER_DATA_SELECTOR;
    iframe->es = USER_DATA_SELECTOR;
    iframe->fs = USER_DATA_SELECTOR;
    iframe->ss = USER_DATA_SELECTOR;
    iframe->cs = USER_CODE_SELECTOR;

    iframe->error = SNAILIX_MAGIC;

    u32 stack3 = alloc_kpage(1); // todo replace to user stack

    iframe->eip = (u32)target;
    // Set EFLAGS with IF=1 (enable interrupts) and IOPL=0 (no I/O privilege)
    iframe->eflags = (1 << 9); // IF=1 only, simpler and safer
    iframe->esp = stack3 + PAGE_SIZE;

    asm volatile(
        "movl %0, %%esp\n"
        "jmp interrupt_exit\n" ::"m"(iframe));
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

    // kernel_info("Task [%s] is blocked!\n", task->name);

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

    // kernel_info("Task [%s] is unblocked!\n", task->name);

    task_t *current = running_task();
    if (current == task) schedule();
}

void task_sleep(u32 ms)
{
    // Make sure interrupts are disabled
    assert(!get_interrupt_state());

    u32 ticks = ms / jiffy;        // Number of ticks to sleep
    ticks = ticks > 0 ? ticks : 1; // Sleep at least one tick

    // Record target global tick when the task should be woken up
    task_t *current = running_task();
    current->ticks = jiffies + ticks;

    // Find the first task in the sleep list whose wakeup tick is later than current task's, for insert-sort
    LinkedList *list = &sleep_list;
    LinkedNode *anchor = &list->tail;

    for (LinkedNode *ptr = list->head.next; ptr != &list->tail; ptr = ptr->next)
    {
        task_t *task = ptr->data;

        if (task->ticks > current->ticks)
        {
            anchor = ptr;
            break;
        }
    }

    assert(current->block_node.next == NULL);
    assert(current->block_node.prev == NULL);

    // Insert into the list
    list_insert_before(anchor, &current->block_node);

    // Block state is sleeping
    current->state = TASK_SLEEPING;

    // Schedule another task to run
    schedule();
}

void task_wakeup()
{
    assert(!get_interrupt_state()); // Must not be interruptible

    // Find tasks in the sleep list whose ticks are less than or equal to jiffies and resume them
    LinkedList *list = &sleep_list;
    for (LinkedNode *ptr = list->head.next; ptr != &list->tail;)
    {
        task_t *task = ptr->data;
        if (task->ticks > jiffies)
        {
            break;
        }

        // task_unblock will clear the pointer
        ptr = ptr->next;

        task->ticks = 0;
        task_unblock(task);
    }
}

/**
 * @brief Yield the current running task.
 */
void task_yield()
{
    schedule();
}

extern void idle_thread();
extern void init_thread();
extern void sleep_thread();
extern void test_thread();

void task_init()
{
    list_init(&block_list);
    list_init(&sleep_list);

    task_create(init_thread, "init_task", 5, NORMAL_USER);
    idle_task = task_create(idle_thread, "idle_task", 1, KERNEL_USER);
    task_create(sleep_thread, "sleep_task", 5, KERNEL_USER);
    task_create(test_thread, "test_task", 5, KERNEL_USER);

    schedule();
}