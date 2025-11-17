/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-17 13:41:52
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-17 13:47:15
 * @Copyright: MIT License
 * @Description: 
 */
#ifndef SNAILIX_ARENA_H
#define SNAILIX_ARENA_H

#include <snailix/types.h>
#include <snailix/list.h>

#define DESC_COUNT 7

typedef LinkedNode block_t; // Memory block

// Arena descriptors
typedef struct arena_descriptor_t
{
    u32 total_block;  // A page divided into how many blocks
    u32 block_size;   // Block size
    LinkedList free_list; // Free block list
} arena_descriptor_t;

// A page or multiple pages of memory
typedef struct arena_t
{
    arena_descriptor_t *desc; // Arena descriptor
    u32 count;                // Current remaining blocks or pages
    u32 large;                // Whether it exceeds 1024 bytes
    u32 magic;                // Magic number
} arena_t;

void *kmalloc(size_t size);
void kfree(void *ptr);

#endif
