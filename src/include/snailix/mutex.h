/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-14 00:13:58
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-14 00:18:53
 * @Copyright: MIT License
 * @Description: Mutex related macros are defined.
 */
#ifndef SNAILIX_MUTEX_H
#define SNAILIX_MUTEX_H

#include <snailix/types.h>
#include <snailix/list.h>

typedef struct mutex_t
{
    bool value;         // Semaphore
    LinkedList waiters; // Waiting threads
} mutex_t;

void mutex_init(mutex_t *mutex);   // Initialize mutex
void mutex_lock(mutex_t *mutex);   // Acquire mutex
void mutex_unlock(mutex_t *mutex); // Release mutex

typedef struct lock_t
{
    struct task_t *holder;
    mutex_t mutex;
    u32 repeat;            // Repeat count
} lock_t;

void lock_init(lock_t *lock);   // Lock initialization
void lock_acquire(lock_t *lock);   // Acquire lock
void lock_release(lock_t *lock); // Release lock

#endif // SNAILIX_MUTEX_H