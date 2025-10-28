/*
 * @Author: Lettle 1071445082@qq.com
 * @Date: 2025-10-28 00:12:03
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-10-28 22:49:54
 * @Copyright: MIT License
 * @Description: Kernel main function
 */
#include <snailix/types.h>
#include <snailix/string.h>
#include <snailix/printk.h>
#include <snailix/assert.h>

extern void screen_init();
extern void screen_print(char *buf, u32 count);

void kernel_main(u32 size, u32 size_high)
{
    screen_init();

    assert(size == 0xFFF);
    printk("Hello, Snailix! %d\n", 666);

    while(1) ;
}