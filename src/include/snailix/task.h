/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-29 13:10:18
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-14 23:43:36
 * @Copyright: MIT License
 * @Description: 
 */
#ifndef SNAILIX_TASK_H
#define SNAILIX_TASK_H

#include <snailix/types.h>
#include <snailix/list.h>

#define KERNEL_USER 0
#define NORMAL_USER 1

#define TASK_NAME_LEN 16

typedef void target_t();

typedef enum task_state_t
{
    TASK_INIT,     // Initialized
    TASK_RUNNING,  // Running
    TASK_READY,    // Ready
    TASK_BLOCKED,  // Blocked
    TASK_SLEEPING, // Sleeping
    TASK_WAITING,  // Waiting
    TASK_DIED,     // Dead
} task_state_t;


typedef struct task_t
{
    u32 *stack;              // Kernel stack
    LinkedNode block_node;   // Blocked list node (for kernel blocked list)
    task_state_t state;      // Task state
    u32 priority;            // Task priority
    u32 ticks;               // Remaining time slices
    u32 jiffies;             // Global time slice at last execution
    char name[TASK_NAME_LEN];// Task name
    u32 uid;                 // User ID
    u32 pde;                 // Page directory physical address
    struct bitmap_t *vmap;   // Process virtual memory bitmap
    u32 brk;                 // Process heap memory highest address
    u32 magic;               // Kernel magic number for stack overflow detection
} task_t;


typedef struct task_frame_t
{
    u32 edi;
    u32 esi;
    u32 ebx;
    u32 ebp;
    void (*eip)(void);
} task_frame_t;

// Interrupt frame
typedef struct intr_frame_t
{
    u32 vector;

    u32 edi;
    u32 esi;
    u32 ebp;
    u32 esp_dummy;

    u32 ebx;
    u32 edx;
    u32 ecx;
    u32 eax;

    u32 gs;
    u32 fs;
    u32 es;
    u32 ds;

    u32 vector0;

    u32 error;

    u32 eip;
    u32 cs;
    u32 eflags;
    u32 esp;
    u32 ss;
} intr_frame_t;

task_t *running_task();
void schedule();

void task_yield();
void task_block(task_t *task, LinkedList *blist, task_state_t state);
void task_unblock(task_t *task);

void task_sleep(u32 ms);
void task_wakeup();

void task_to_user_mode(target_t target);

#endif //SNAILIX_TASK_H