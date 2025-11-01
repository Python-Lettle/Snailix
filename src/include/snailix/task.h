/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-29 13:10:18
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-01 10:25:35
 * @Copyright: MIT License
 * @Description: 
 */
#ifndef SNAILIX_TASK_H
#define SNAILIX_TASK_H

#include <snailix/types.h>

typedef u32 target_t();

typedef struct task_t
{
    u32 *stack; // Kernel stack
} task_t;

typedef struct task_frame_t
{
    u32 edi;
    u32 esi;
    u32 ebx;
    u32 ebp;
    void (*eip)(void);
} task_frame_t;

void task_init();

#endif //SNAILIX_TASK_H