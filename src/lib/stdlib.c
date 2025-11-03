/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-01 13:47:45
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-01 15:55:13
 * @Copyright: MIT License
 * @Description: 
 */
#include <snailix/stdlib.h>

void delay(u32 count)
{
    while (count--);
}

// Convert BCD to integer
u8 bcd_to_bin(u8 value)
{
    return (value & 0xf) + (value >> 4) * 10;
}

// Convert integer to BCD
u8 bin_to_bcd(u8 value)
{
    return (value / 10) * 0x10 + (value % 10);
}