/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-01 13:38:53
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-03 17:31:50
 * @Copyright: MIT License
 * @Description: 
 */
#ifndef SNAILIX_STDLIB_H
#define SNAILIX_STDLIB_H

#include <snailix/types.h>

void delay(u32 count);

u8 bcd_to_bin(u8 value);
u8 bin_to_bcd(u8 value);

u32 div_round_up(u32 num, u32 size);

#endif //SNAILIX_STDLIB_H