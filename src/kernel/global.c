/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-29 10:43:24
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-14 14:11:18
 * @Copyright: MIT License
 * @Description: 
 */
#include <snailix/global.h>
#include <snailix/string.h>
#include <snailix/printk.h>
#include <snailix/debug.h>

descriptor_t gdt[GDT_SIZE]; // Kernel Global Descriptor Table
pointer_t gdt_ptr;          // Kernel GDT pointer
tss_t tss;                  // Task State Segment

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

void tss_init()
{
    memset(&tss, 0, sizeof(tss));

    tss.ss0 = KERNEL_DATA_SELECTOR;
    tss.iobase = sizeof(tss);

    descriptor_t *desc = gdt + KERNEL_TSS_IDX;
    descriptor_init(desc, (u32)&tss, sizeof(tss) - 1);
    desc->segment = 0;     // System segment
    desc->granularity = 0; // Byte granularity
    desc->big = 0;         // Fixed to 0
    desc->long_mode = 0;   // Fixed to 0
    desc->present = 1;     // Present in memory
    desc->DPL = 0;         // For task gate or call gate
    desc->type = 0b1001;   // 32-bit available TSS

    asm volatile(
        "lgdt (%%eax)\n"
        "ltr %%bx\n"
        ::"a"(&gdt_ptr), "b"(KERNEL_TSS_SELECTOR) : "memory");
}