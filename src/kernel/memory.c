/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-22 23:55:09
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-10-28 23:13:50
 * @Copyright: MIT License
 * @Description: Initialize the memory management of Snailix.
 */
#include <snailix/printk.h>
#include <snailix/types.h>

void memory_init(u32 memsize_low, u32 memsize_high)
{
    u32 KB = memsize_low / 1024;
    u32 MB = KB / 1024;
    kernel_info("Memory size = %d B (%d KB, %d MB)\n", memsize_low, KB, MB);
}