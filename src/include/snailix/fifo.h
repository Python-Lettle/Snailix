/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-14 08:44:19
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-14 08:44:49
 * @Copyright: MIT License
 * @Description: 
 */
#ifndef SNAILIX_FIFO_H
#define SNAILIX_FIFO_H

#include <snailix/types.h>

typedef struct fifo_t
{
    char *buf;
    u32 length;
    u32 head;
    u32 tail;
} fifo_t;

void fifo_init(fifo_t *fifo, char *buf, u32 length);
bool fifo_full(fifo_t *fifo);
bool fifo_empty(fifo_t *fifo);
char fifo_get(fifo_t *fifo);
void fifo_put(fifo_t *fifo, char byte);

#endif //SNAILIX_FIFO_H