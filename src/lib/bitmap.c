/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-30 00:21:10
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-10-31 00:23:58
 * @Copyright: MIT License
 * @Description: 
 */
#include <snailix/printk.h>
#include <snailix/bitmap.h>
#include <snailix/string.h>
#include <snailix/assert.h>

// Initialize the bitmap, allocate memory and set the capacity
bool bitmap_init(bitmap_t *bitmap, char *buffer, u32 capacity, u32 start)
{
    memset(buffer, 0, capacity);
    bitmap->bits = buffer;
    bitmap->capacity = capacity;
    bitmap->offset = start;
    return true;
}

// Set the bit at the specified index to 1
int bitmap_set(bitmap_t *bitmap, u32 index, bool value)
{
    // Get the index of the bitmap
    u32 idx = index - bitmap->offset;

    // Get the byte index
    u32 bytes = idx / 8;

    // Get the bit index
    u32 bits = idx % 8;
    
    if (value)
    {
        bitmap->bits[bytes] |= (1 << bits);
    }
    else
    {
        bitmap->bits[bytes] &= ~(1 << bits);
    }
}

// Get the bit at the specified index
int bitmap_get(bitmap_t *bitmap, u32 index)
{
    assert(index >= bitmap->offset);

    // Get the index of the bitmap
    u32 idx = index - bitmap->offset;

    // Get the byte index
    u32 bytes = idx / 8;

    // Get the bit index
    u32 bits = idx % 8;

    return (bitmap->bits[bytes] >> bits) & 1;
}
