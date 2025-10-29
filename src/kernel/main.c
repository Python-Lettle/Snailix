/*
 * @Author: Lettle 1071445082@qq.com
 * @Date: 2025-10-28 00:12:03
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-10-29 14:27:13
 * @Copyright: MIT License
 * @Description: Kernel main function
 */
#include <snailix/types.h>
#include <snailix/string.h>
#include <snailix/printk.h>
#include <snailix/assert.h>
#include <snailix/list.h>

extern void screen_init();
extern void memory_init(u32 memsize_low, u32 memsize_high);
extern void gdt_init();
extern void screen_print(char *buf, u32 count);

void kernel_main(u32 size, u32 size_high)
{
    screen_init();
    gdt_init();
    memory_init(size, size_high);

    for (u32 i=0; i<20; i++) {
        printk("[%d] Hello, Snailix!\n", i);
    }
    

}