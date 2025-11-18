/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-03 13:42:21
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-18 00:00:00
 * @Copyright: MIT License
 * @Description: 
 */
#ifndef SNAILIX_MEMORY_H
#define SNAILIX_MEMORY_H

#include <snailix/types.h>

#define PAGE_SIZE 0x1000        // 1 Page = 4KB
#define MEMORY_BASE 0x100000    // 1M, start of available memory

// Kernel memory size 8M
#define KERNEL_MEMORY_SIZE 0x800000

// User stack top address 128M
#define USER_STACK_TOP 0x8000000

// User stack max size 2M
#define USER_STACK_SIZE 0x200000

// User stack bottom address 128M - 2M
#define USER_STACK_BOTTOM (USER_STACK_TOP - USER_STACK_SIZE)

// Kernel page directory address
#define KERNEL_PAGE_DIR (0x1000)    // 0x101000

typedef struct page_entry_t
{
    u8 present : 1;  // in memory
    u8 write : 1;    // 0 read-only, 1 read-write
    u8 user : 1;     // 1 everyone, 0 supervisor (DPL < 3)
    u8 pwt : 1;      // page write-through: 1 write-through, 0 write-back
    u8 pcd : 1;      // page cache disable
    u8 accessed : 1; // accessed, for usage statistics
    u8 dirty : 1;    // dirty, buffer has been written
    u8 pat : 1;      // page attribute table, page size 4K/4M
    u8 global : 1;   // global, used by all processes, do not flush
    u8 ignored : 3;  // reserved for OS
    u32 index : 20;  // page index
} _packed page_entry_t;

u32 get_cr2();
u32 get_cr3();
void set_cr3(u32 pde);

u32 alloc_kpage(u32 count);
void free_kpage(u32 vaddr, u32 count);

// Link vaddr to physical memory
void link_page(u32 vaddr);

// Unlink vaddr from physical memory
void unlink_page(u32 vaddr);

page_entry_t *copy_pde();

#endif // SNAILIX_MEMORY_H