/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-14 00:15:12
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-14 00:19:57
 * @Copyright: MIT License
 * @Description: Mutex related functions are defined.
 */
#include <snailix/mutex.h>
#include <snailix/task.h>
#include <snailix/interrupt.h>
#include <snailix/assert.h>

void mutex_init(mutex_t *mutex)
{
    mutex->value = false; // No one holds the mutex at the beginning
    list_init(&mutex->waiters);
}

// Try to acquire the mutex
void mutex_lock(mutex_t *mutex)
{
    // Close interrupts to ensure atomicity
    bool intr = interrupt_disable();

    task_t *current = running_task();
    while (mutex->value == true)
    {
        // If the mutex is held by another task,
        // then block the current task and add it to the wait queue.
        task_block(current, &mutex->waiters, TASK_BLOCKED);
    }

    // No one holds the mutex, so we can acquire it.
    assert(mutex->value == false);

    // Acquire the mutex
    mutex->value++;
    assert(mutex->value == true);

    // Restore the previous interrupt state
    set_interrupt_state(intr);
}

// Release the mutex
void mutex_unlock(mutex_t *mutex)
{
    // Close interrupts to ensure atomicity
    bool intr = interrupt_disable();

    // Check if the mutex is held by the current task
    assert(mutex->value == true);

    // Release the mutex
    mutex->value--;
    assert(mutex->value == false);

    // If the wait queue is not empty, resume the first task
    if (!list_empty(&mutex->waiters))
    {
        task_t *task = mutex->waiters.tail.prev->data;
        assert(task->magic == SNAILIX_MAGIC);
        task_unblock(task);
        // Make sure the new task can get the mutex, otherwise it may starve.
        task_yield();
    }

    // Restore the previous interrupt state
    set_interrupt_state(intr);
}

// Lock initialization
void lock_init(lock_t *lock)
{
    lock->holder = NULL;
    lock->repeat = 0;
    mutex_init(&lock->mutex);
}

// Acquire lock 
void lock_acquire(lock_t *lock)
{
    task_t *current = running_task();
    if (lock->holder != current)
    {
        mutex_lock(&lock->mutex);
        lock->holder = current;
        assert(lock->repeat == 0);
        lock->repeat = 1;
    }
    else
    {
        lock->repeat++;
    }
}

// Release lock
void lock_release(lock_t *lock)
{
    task_t *current = running_task();
    assert(lock->holder == current);
    if (lock->repeat > 1)
    {
        lock->repeat--;
        return;
    }

    assert(lock->repeat == 1);

    lock->holder = NULL;
    lock->repeat = 0;
    mutex_unlock(&lock->mutex);
}
