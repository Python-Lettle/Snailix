/*
 * @Author: Lettle 1071445082@qq.com
 * @Date: 2025-10-28 00:12:03
 * @LastEditors: Python-Lettle 1071445082@qq.com
 * @LastEditTime: 2025-11-13 13:33:11
 * @Copyright: MIT License
 * @Description: Kernel main function
 */
#include <snailix/types.h>
#include <snailix/string.h>
#include <snailix/printk.h>
#include <snailix/assert.h>
#include <snailix/interrupt.h>
#include <snailix/debug.h>

extern void screen_init();
extern void memory_init(u32 memsize_low, u32 memsize_high);
extern void memory_map_init();
extern void mapping_init();
extern void gdt_init();
extern void interrupt_init();
extern void task_init();
extern void clock_init();
extern void time_init();
extern void keyboard_init();
extern void syscall_init();

extern void screen_print(char *buf, u32 count);
extern u32 alloc_kpage(u32 count);

void kernel_main(u32 size, u32 size_high)
{
    screen_init();
    gdt_init();
    memory_init(size, size_high);
    memory_map_init();
    mapping_init();
    interrupt_init();
    clock_init();
    time_init();

    keyboard_init();

    syscall_init();

    task_init();

    set_interrupt_state(true);
}