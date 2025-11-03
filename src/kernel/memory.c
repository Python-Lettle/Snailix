/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-22 23:55:09
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-03 19:40:11
 * @Copyright: MIT License
 * @Description: Initialize the memory management of Snailix.
 */
#include <snailix/memory.h>
#include <snailix/printk.h>
#include <snailix/types.h>
#include <snailix/stdlib.h>
#include <snailix/string.h>

// Get addr page index
#define IDX(addr) ((u32)addr >> 12) 
// Gets the start position of the page corresponding to the page index idx
#define PAGE(idx) ((u32)idx << 12)
// Check if the address is a page boundary
#define ASSERT_PAGE(addr) assert((addr & 0xfff) == 0)

static u32 memory_base = 0;
static u64 memory_size = 0;
static u32 total_pages = 0;
static u32 free_pages = 0;

// The start address of the available memory area
static u32 start_page = 0;
// Phisical memory array
static u8 *memory_map;
// Pages used by the physical memory array
static u32 memory_map_pages;

#define used_pages (total_pages - free_pages)

void memory_init(u32 memsize_low, u32 memsize_high)
{
    memory_base = MEMORY_BASE;
    memory_size += memsize_low;
    if (memsize_high != 0) memory_size += ((u64)memsize_high << 31);
    total_pages = IDX(memory_size);
    free_pages = total_pages;

    u32 KB = memory_size >> 10;
    u32 MB = KB >> 10;
    kernel_info("Memory base = 0x%x\n", memory_base);
    kernel_info("Memory size = %lu B", memory_size);
    printk(" ( %u KB, %u MB )\n", KB, MB);
}

void memory_map_init()
{
    // Initialize the memory map
    memory_map = (u8 *)memory_base;

    // Calculate the number of pages used by the memory map
    memory_map_pages = div_round_up(total_pages, PAGE_SIZE);
    kernel_info("Memory map page count = %d\n", memory_map_pages);

    free_pages -= memory_map_pages;

    // Clean the memory map
    memset((void *)memory_map, 0, memory_map_pages * PAGE_SIZE);
    
    // Set the first 1M memory as used
    // And set the memory (used by the memory map) as used
    start_page = IDX(MEMORY_BASE) + memory_map_pages;
    for (size_t i=0; i<start_page; i++)
    {
        memory_map[i] = 1;
    }

    kernel_info("Total pages = %d, free pages = %d\n", total_pages, free_pages);
}
