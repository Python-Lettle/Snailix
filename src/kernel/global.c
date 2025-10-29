/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-29 10:43:24
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-10-29 10:49:16
 * @Copyright: MIT License
 * @Description: 
 */
#include <snailix/global.h>
#include <snailix/string.h>
#include <snailix/printk.h>

descriptor_t gdt[GDT_SIZE]; // Kernel Global Descriptor Table
pointer_t gdt_ptr;          // Kernel GDT pointer

void descriptor_init(descriptor_t *desc, u32 base, u32 limit)
{
    desc->base_low = base & 0xffffff;
    desc->base_high = (base >> 24) & 0xff;
    desc->limit_low = limit & 0xffff;
    desc->limit_high = (limit >> 16) & 0xf;
}

// Init the GDT
void gdt_init()
{
    memset(gdt, 0, sizeof(gdt));

    descriptor_t *desc;
    // Code Segment
    desc = gdt + KERNEL_CODE_IDX;
    descriptor_init(desc, 0, 0xFFFFF);
    desc->segment = 1;     // Code segment
    desc->granularity = 1; // 4K
    desc->big = 1;         // 32 bit mode
    desc->long_mode = 0;   // Not 64 bit
    desc->present = 1;     // In memory
    desc->DPL = 0;         // Kernel privilege level
    desc->type = 0b1010;   // Code / Not conforming / Readable / Not accessed

    // Data Segment
    desc = gdt + KERNEL_DATA_IDX;
    descriptor_init(desc, 0, 0xFFFFF);
    desc->segment = 1;     // Data segment
    desc->granularity = 1; // 4K
    desc->big = 1;         // 32 bit mode
    desc->long_mode = 0;   // Not 64 bit
    desc->present = 1;     // In memory
    desc->DPL = 0;         // Kernel privilege level
    desc->type = 0b0010;   // Data / Upward growing / Writable / Not accessed

    // User Code Segment
    desc = gdt + USER_CODE_IDX;
    descriptor_init(desc, 0, 0xFFFFF);
    desc->segment = 1;     // Code segment
    desc->granularity = 1; // 4K
    desc->big = 1;         // 32 bit mode
    desc->long_mode = 0;   // Not 64 bit
    desc->present = 1;     // In memory
    desc->DPL = 3;         // User privilege level
    desc->type = 0b1010;   // Code / Not conforming / Readable / Not accessed

    // User Data Segment
    desc = gdt + USER_DATA_IDX;
    descriptor_init(desc, 0, 0xFFFFF);
    desc->segment = 1;     // Data segment
    desc->granularity = 1; // 4K
    desc->big = 1;         // 32 bit mode
    desc->long_mode = 0;   // Not 64 bit
    desc->present = 1;     // In memory
    desc->DPL = 3;         // User privilege level
    desc->type = 0b0010;   // Data / Upward growing / Writable / Not accessed

    gdt_ptr.base = (u32)&gdt;
    gdt_ptr.limit = sizeof(gdt) - 1;

    kernel_info("GDT init done!!!\n");
}