/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-29 10:42:08
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-14 13:57:20
 * @Copyright: MIT License
 * @Description: Some constants and necessary data structures are defined.
 */
#ifndef SNAILIX_GLOBAL_H
#define SNAILIX_GLOBAL_H

#include <snailix/types.h>

#define GDT_SIZE 128

#define KERNEL_CODE_IDX 1
#define KERNEL_DATA_IDX 2
#define KERNEL_TSS_IDX 3

#define USER_CODE_IDX 4
#define USER_DATA_IDX 5

#define KERNEL_CODE_SELECTOR (KERNEL_CODE_IDX << 3)
#define KERNEL_DATA_SELECTOR (KERNEL_DATA_IDX << 3)
#define KERNEL_TSS_SELECTOR (KERNEL_TSS_IDX << 3)

#define USER_CODE_SELECTOR (USER_CODE_IDX << 3 | 0b11)
#define USER_DATA_SELECTOR (USER_DATA_IDX << 3 | 0b11)

// GDT (8 Byte)
typedef struct descriptor_t
{
    unsigned short limit_low;      // Segment limit bits 0-15
    unsigned int base_low : 24;    // Base address bits 0-23 (16M)
    unsigned char type : 4;        // Segment type
    unsigned char segment : 1;     // 1 for code or data segment, 0 for system segment
    unsigned char DPL : 2;         // Descriptor Privilege Level 0-3
    unsigned char present : 1;     // Present bit, 1 in memory, 0 on disk
    unsigned char limit_high : 4;  // Segment limit bits 16-19
    unsigned char available : 1;   // Available for use by operating system
    unsigned char long_mode : 1;   // 64-bit extension flag
    unsigned char big : 1;         // 32-bit or 16-bit
    unsigned char granularity : 1; // Granularity 4KB or 1B
    unsigned char base_high;       // Base address bits 24-31
} _packed descriptor_t;

// Segment selector
typedef struct selector_t
{
    u8 RPL : 2; // Request Privilege Level
    u8 TI : 1;  // Table Indicator
    u16 index : 13;
} selector_t;

// Global Descriptor Table pointer
typedef struct pointer_t
{
    u16 limit;
    u32 base;
} _packed pointer_t;

typedef struct tss_t
{
    u32 backlink;      // Previous task link, holds the segment selector of the previous task's state segment
    u32 esp0;          // Ring 0 stack top address
    u32 ss0;           // Ring 0 stack segment selector
    u32 esp1;          // Ring 1 stack top address
    u32 ss1;           // Ring 1 stack segment selector
    u32 esp2;          // Ring 2 stack top address
    u32 ss2;           // Ring 2 stack segment selector
    u32 cr3;
    u32 eip;
    u32 flags;
    u32 eax;
    u32 ecx;
    u32 edx;
    u32 ebx;
    u32 esp;
    u32 ebp;
    u32 esi;
    u32 edi;
    u32 es;
    u32 cs;
    u32 ss;
    u32 ds;
    u32 fs;
    u32 gs;
    u32 ldtr;          // Local descriptor table selector
    u16 trace : 1;     // If set, a debug exception will be raised on task switch
    u16 reversed : 15; // Reserved, unused
    u16 iobase;        // I/O bitmap base address, 16-bit offset from TSS to IO permission bitmap
    u32 ssp;           // Task shadow stack pointer
} _packed tss_t;

#endif //SNAILIX_GLOBAL_H