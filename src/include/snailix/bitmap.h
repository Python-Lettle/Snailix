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
    u8 *bits;       // Bitmap buffer
    u32 capacity;   // Maximum index
    u32 offset;     // Offset of the first bit
} bitmap_t;

// Initialize the bitmap, allocate memory and set the capacity
bool bitmap_init(bitmap_t *bitmap, char *buffer, u32 capacity, u32 start);

// Set the bit at the specified index to 'value'
int bitmap_set(bitmap_t *bitmap, u32 index, bool value);

// Get the bit at the specified index
int bitmap_get(bitmap_t *bitmap, u32 index);
// Free the bitmap memory
void bitmap_free(bitmap_t *bitmap);

#endif // SNAILIX_BITMAP_H
