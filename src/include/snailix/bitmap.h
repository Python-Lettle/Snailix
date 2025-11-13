/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-29 20:21:18
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-10-31 00:13:55
 * @Copyright: MIT License
 * @Description: 
 */
#ifndef SNAILIX_BITMAP_H
#define SNAILIX_BITMAP_H

#include <snailix/types.h>

typedef struct bitmap_t
{
    u8 *bits;   // bitmap buffer
    u32 length; // bitmap buffer length
    u32 offset; // bitmap start offset
} bitmap_t;

// Initialize bitmap
void bitmap_init(bitmap_t *map, char *bits, u32 length, u32 offset);

// Construct bitmap
void bitmap_make(bitmap_t *map, char *bits, u32 length, u32 offset);

// Test if a bit in the bitmap is 1
bool bitmap_test(bitmap_t *map, u32 index);

// Set the value of a bit in the bitmap
void bitmap_set(bitmap_t *map, u32 index, bool value);

// Get consecutive count bits from the bitmap
int bitmap_scan(bitmap_t *map, u32 count);

#endif // SNAILIX_BITMAP_H
