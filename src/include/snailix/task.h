/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-29 13:10:18
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-10 13:41:27
 * @Copyright: MIT License
 * @Description: 
 */
#ifndef SNAILIX_TASK_H
#define SNAILIX_TASK_H

#include <snailix/types.h>

#define KERNEL_USER 0
#define NORMAL_USER 1

#define TASK_NAME_LEN 16

typedef u32 target_t();

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

task_t *running_task();
void schedule();

void task_yield();

#endif //SNAILIX_TASK_H