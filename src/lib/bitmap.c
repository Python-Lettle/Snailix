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

// Construct bitmap
void bitmap_make(bitmap_t *map, char *bits, u32 length, u32 offset)
{
    map->bits = bits;
    map->length = length;
    map->offset = offset;
}

// Initialize bitmap, set all bits to 0
void bitmap_init(bitmap_t *map, char *bits, u32 length, u32 start)
{
    memset(bits, 0, length);
    bitmap_make(map, bits, length, start);
}

// Test if a specific bit is 1
bool bitmap_test(bitmap_t *map, idx_t index)
{
    assert(index >= map->offset);

    // Get bitmap index
    idx_t idx = index - map->offset;

    // Byte in bitmap array
    u32 bytes = idx / 8;

    // Bit position in that byte
    u8 bits = idx % 8;

    assert(bytes < map->length);

    // Return whether that bit is 1
    return (map->bits[bytes] & (1 << bits));
}

// Set a specific bit in bitmap
void bitmap_set(bitmap_t *map, idx_t index, bool value)
{
    // value must be binary (0 or 1)
    assert(value == 0 || value == 1);

    assert(index >= map->offset);

    // Get bitmap index
    idx_t idx = index - map->offset;

    // Byte in bitmap array
    u32 bytes = idx / 8;

    // Bit position in that byte
    u8 bits = idx % 8;
    if (value)
    {
        // Set to 1
        map->bits[bytes] |= (1 << bits);
    }
    else
    {
        // Set to 0
        map->bits[bytes] &= ~(1 << bits);
    }
}

// Get consecutive 'count' bits from bitmap
int bitmap_scan(bitmap_t *map, u32 count)
{
    int start = EOF;                 // Mark start position of target
    u32 bits_left = map->length * 8; // Remaining bits
    u32 next_bit = 0;                // Next bit
    u32 counter = 0;                 // Counter

    // Search from beginning
    while (bits_left-- > 0)
    {
        if (!bitmap_test(map, map->offset + next_bit))
        {
            // If next bit is free, increment counter
            counter++;
        }
        else
        {
            // Otherwise reset counter and continue searching
            counter = 0;
        }

        // Move to next bit
        next_bit++;

        // Found required count, set start position and exit
        if (counter == count)
        {
            start = next_bit - count;
            break;
        }
    }

    // If not found, return EOF (END OF FILE)
    if (start == EOF)
        return EOF;

    // Otherwise set all found bits to 1
    bits_left = count;
    next_bit = start;
    while (bits_left--)
    {
        bitmap_set(map, map->offset + next_bit, true);
        next_bit++;
    }

    // Return index
    return start + map->offset;
}
