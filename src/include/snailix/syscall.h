/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-10 09:54:02
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-16 23:46:09
 * @Copyright: MIT License
 * @Description: 
 */
#ifndef ONIX_SYSCALL_H
#define ONIX_SYSCALL_H

#include <snailix/types.h>

typedef enum syscall_t
{
    SYS_NR_TEST,
    SYS_NR_WRITE,
    SYS_NR_SLEEP,
    SYS_NR_YIELD,
} syscall_t;

u32 test();
void sleep(u32 ms);
void yield();

int32 write(fd_t fd, char *buf, u32 len);

#endif