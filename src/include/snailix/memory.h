/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-03 13:42:21
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-03 13:42:50
 * @Copyright: MIT License
 * @Description: 
 */
#ifndef SNAILIX_MEMORY_H
#define SNAILIX_MEMORY_H

#include <snailix/types.h>

#define PAGE_SIZE 0x1000        // 1 Page = 4KB
#define MEMORY_BASE 0x100000    // 1M, start of available memory

// Kernel page directory index
#define KERNEL_PAGE_DIR 0x1000



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

u32 get_cr3();
void set_cr3(u32 pde);

u32 alloc_kpage(u32 count);
void free_kpage(u32 vaddr, u32 count);

// Forward declarations
void enable_page();
void entry_init(page_entry_t *entry, u32 index, u8 present, u8 write, u8 user);
void mapping_init();

#endif // SNAILIX_MEMORY_H