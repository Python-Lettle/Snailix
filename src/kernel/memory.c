/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-22 23:55:09
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-15 14:35:20
 * @Copyright: MIT License
 * @Description: Initialize the memory management of Snailix with paging support.
 */
#include <snailix/memory.h>
#include <snailix/printk.h>
#include <snailix/types.h>
#include <snailix/debug.h>
#include <snailix/assert.h>
#include <snailix/stdlib.h>
#include <snailix/string.h>
#include <snailix/bitmap.h>

// Get addr page index
#define IDX(addr) ((u32)addr >> 12)
#define DIDX(addr) (((u32)addr >> 22) & 0x3ff)
#define TIDX(addr) (((u32)addr >> 12) & 0x3ff)
// Gets the start position of the page corresponding to the page index idx
#define PAGE(idx) ((u32)idx << 12)
// Check if the address is a page boundary
#define ASSERT_PAGE(addr) assert((addr & 0xfff) == 0)

static u32 KERNEL_PAGE_TABLE[] = {
    0x2000,
    0x3000,
};

#define KERNEL_MAP_BITS 0x4000

#define KERNEL_MEMORY_SIZE (0x100000 * sizeof(KERNEL_PAGE_TABLE))

bitmap_t kernel_map;

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
    memory_size = memory_size & 0xfffff000;
    total_pages = IDX(memory_size);
    free_pages = total_pages;

    u32 KB = memory_size >> 10;
    u32 MB = KB >> 10;
    kernel_info("Memory base = 0x%x\n", memory_base);
    kernel_info("Memory size = 0x%x B", memory_size);
    printk(" ( %u KB, %u MB )\n", KB, MB);
}

void memory_map_init()
{
    memory_map = (u8 *)memory_base;
    kernel_info("Memory map base = 0x%x\n", memory_map);

    // Calculate the number of pages used by the memory map
    memory_map_pages = div_round_up(total_pages, PAGE_SIZE);
    kernel_info("Memory map page count = %d\n", memory_map_pages);

    free_pages -= memory_map_pages;

    // Clean the memory map
    memset((void *)memory_map, 0, memory_map_pages * PAGE_SIZE);

    // Set the first 1M memory as used (except page directory and page tables)
    // And set the memory (used by the memory map) as used
    start_page = IDX(MEMORY_BASE) + memory_map_pages;
    for (size_t i=0; i<start_page; i++)
    {
        memory_map[i] = 1;
    }

    kernel_info("Total pages = %d, free pages = %d, start_page = %d\n", total_pages, free_pages, start_page);

    u32 length = (IDX(KERNEL_MEMORY_SIZE) - IDX(MEMORY_BASE)) / 8;
    bitmap_init(&kernel_map, (u8 *)KERNEL_MAP_BITS, length, IDX(MEMORY_BASE));
    bitmap_scan(&kernel_map, memory_map_pages);
}

// Alloc a page of physical memory
static u32 get_page()
{
    for (size_t i = start_page; i < total_pages; i++)
    {
        // If the page is not used
        if (!memory_map[i])
        {
            memory_map[i] = 1;
            free_pages--;
            assert(free_pages >= 0);
            u32 page = ((u32)i) << 12;
            kernel_info("GET page 0x%p\n", page);
            return page;
        }
    }
    panic("Out of Memory!!!");
}

// Free a page of physical memory
static void put_page(u32 addr)
{
    ASSERT_PAGE(addr);

    u32 idx = IDX(addr);

    // idx must be in the range of [start_page, total_pages)
    assert(idx >= start_page && idx < total_pages);

    // The page must be used
    assert(memory_map[idx] >= 1);

    // Physical reference minus one
    memory_map[idx]--;

    // If the reference count is 0, then the page is free
    if (!memory_map[idx])
    {
        free_pages++;
    }

    assert(free_pages > 0 && free_pages < total_pages);
    kernel_info("PUT page 0x%p\n", addr);
}

/**
 * @brief Get the value of CR3 register
 * @return The value of CR3 register
 */
u32 get_cr3()
{
    u32 cr3;
    __asm__ __volatile__("movl %%cr3, %0" : "=a"(cr3));
    return cr3;
}

/**
 * @brief Set the value of CR3 register
 * @param pde The page directory entry to set
 */
void set_cr3(u32 pde)
{
    __asm__ __volatile__("movl %0, %%cr3" : : "a"(pde));
}

/**
 * @brief Initialize a page entry
 * @param entry Pointer to the page entry
 * @param page_number Physical page number (not index)
 * @param present Present flag
 * @param write Write permission flag
 * @param user User access flag
 */
void entry_init(page_entry_t *entry, u32 page_number, u8 present, u8 write, u8 user)
{
    // Zero out the entire entry to clear any existing flags
    memset(entry, 0, sizeof(page_entry_t));
    
    entry->present = present;
    entry->write = write;
    entry->user = 1;
    entry->pwt = 0;      // Page write-through disabled
    entry->pcd = 0;      // Cache enabled
    entry->accessed = 0;  // Not accessed yet
    entry->dirty = 0;    // Not dirty yet
    entry->pat = 0;      // Default page attribute
    entry->global = 1;   // Global pages for kernel - won't be flushed from TLB
    entry->ignored = 0;  // Reserved bits
    
    // For kernel pages, we set the global bit to 1
    // This means these pages won't be flushed from the TLB on task switches
    // which improves performance
    
    // The index field holds the physical page number (page frame number)
    // This is the physical address divided by PAGE_SIZE (4KB)
    entry->index = page_number;
}

/**
 * @brief Enable paging mode
 * 
 * This function enables paging by setting the PG (Paging) bit in the CR0 register.
 * IMPORTANT: After setting the PG bit, we need to perform a far jump to flush the
 * instruction pipeline. This ensures that all subsequent instructions are fetched
 * using the virtual address space.
 */
void enable_page()
{
    u32 cr0;
    
    // Get current CR0 value
    __asm__ __volatile__("movl %%cr0, %0" : "=a"(cr0));
    
    // Set PG bit to enable paging
    cr0 |= 0x80000000;
    
    // Write back to CR0 - this enables paging
    __asm__ __volatile__(
        "movl %0, %%cr0\n"
        "jmp 1f\n" // Jump to next instruction to flush pipeline
        "1:"
        : : "a"(cr0) : "memory"
    );
}

// /**
//  * @brief Allocate kernel pages
//  * @param count Number of pages to allocate
//  * @return Virtual address of the allocated pages
//  */
// u32 alloc_kpage(u32 count)
// {
//     if (count == 0 || free_pages < count) {
//         return 0;
//     }

//     u32 pages_found = 0;
//     u32 start_index = start_page;

//     // Find contiguous free pages
//     for (size_t i = start_page; i < total_pages && pages_found < count; i++) {
//         if (memory_map[i] == 0) {
//             if (pages_found == 0) {
//                 start_index = i;
//             }
//             pages_found++;
//         } else {
//             pages_found = 0;
//         }
//     }

//     if (pages_found < count) {
//         return 0;
//     }

//     // Mark pages as used
//     for (size_t i = 0; i < count; i++) {
//         memory_map[start_index + i] = 1;
//     }

//     free_pages -= count;
//     return PAGE(start_index);
// }

// /**
//  * @brief Free kernel pages
//  * @param vaddr Virtual address of the pages to free
//  * @param count Number of pages to free
//  */
// void free_kpage(u32 vaddr, u32 count)
// {
//     if (vaddr == 0 || count == 0 || IDX(vaddr) < start_page) {
//         return;
//     }

//     u32 start_index = IDX(vaddr);

//     // Mark pages as free
//     for (size_t i = 0; i < count && (start_index + i) < total_pages; i++) {
//         if (memory_map[start_index + i] == 1) {
//             memory_map[start_index + i] = 0;
//             free_pages++;
//         }
//     }
// }

/**
 * @brief Initialize memory mapping and enable paging
 */
void mapping_init()
{
    // Get page indices for these addresses - IDX returns physical page number (physical address / PAGE_SIZE)
    u32 pdir_pfn = KERNEL_PAGE_DIR / PAGE_SIZE;  // Physical Frame Number
    u32 pt1_pfn = KERNEL_PAGE_TABLE[0] / PAGE_SIZE;
    u32 pt2_pfn = KERNEL_PAGE_TABLE[1] / PAGE_SIZE;

    // Get memory map indices for these addresses (virtual memory tracking)
    u32 pdir_idx = IDX(KERNEL_PAGE_DIR);
    u32 pt1_idx = IDX(KERNEL_PAGE_TABLE[0]);
    u32 pt2_idx = IDX(KERNEL_PAGE_TABLE[1]);

    // Mark these pages as used in memory map first
    memory_map[pdir_idx] = 1;
    memory_map[pt1_idx] = 1;
    memory_map[pt2_idx] = 1;
    free_pages -= 3;

    // Initialize page directory at safe address
    // IMPORTANT: At this point, we are still in real mode - all addresses are physical
    u32 *page_dir = (u32 *)KERNEL_PAGE_DIR;
    ASSERT_PAGE((u32)page_dir);
    print_prefix("[Mem Paging]","Page directory located at 0x%x\n", (u32)page_dir);
    
    // Clear page directory
    memset(page_dir, 0, PAGE_SIZE);

    // Initialize first page table at safe address
    u32 *page_table1 = (u32 *)KERNEL_PAGE_TABLE[0];
    ASSERT_PAGE((u32)page_table1);
    print_prefix("[Mem Paging]","First page table located at 0x%x\n", (u32)page_table1);
    
    // Clear first page table
    memset(page_table1, 0, PAGE_SIZE);

    // Initialize second page table at safe address
    u32 *page_table2 = (u32 *)KERNEL_PAGE_TABLE[1];
    ASSERT_PAGE((u32)page_table2);
    print_prefix("[Mem Paging]","Second page table located at 0x%x\n", (u32)page_table2);
    
    // Clear second page table
    memset(page_table2, 0, PAGE_SIZE);

    // Set up page directory entries
    // For page directory entries, we need to store the physical page number of the page table
    page_entry_t *pde0 = (page_entry_t *)&page_dir[0];
    // Use actual physical page frame number for the page table
    entry_init(pde0, pt1_pfn, 1, 1, 0);  // Present, R/W, Supervisor

    // Set up page directory entry for the next 4MB
    page_entry_t *pde1 = (page_entry_t *)&page_dir[1];
    entry_init(pde1, pt2_pfn, 1, 1, 0);  // Present, R/W, Supervisor

    // Map the first 4MB of physical memory to virtual memory (1:1 mapping)
    // This is crucial for the kernel to continue executing after paging is enabled
    for (size_t i = 0; i < 1024; i++) {
        // Map each physical page to the same virtual address
        page_entry_t *pte = (page_entry_t *)&page_table1[i];
        // Here i is the physical frame number (since we're mapping 1:1)
        // For the first few pages (where user_mode_thread might be located), set user access bit
        u8 user_access = (i < 16) ? 1 : 0;  // Allow user mode access to first 16 pages (64KB)
        entry_init(pte, i, 1, 1, user_access);  // Present, R/W, User (for first 16 pages)

    }
    // Map the second 4MB of physical memory to virtual memory (1:1 mapping)
    for (size_t i = 0; i < 1024; i++) {
        page_entry_t *pte = (page_entry_t *)&page_table2[i];
        // Physical frame number is i + 1024 (next 4MB)
        entry_init(pte, i + 1024, 1, 1, 0);  // Present, R/W, Supervisor
    }


    // Set CR3 register to point to page directory physical address
    // CR3 must contain the physical address of the page directory, not virtual
    set_cr3(KERNEL_PAGE_DIR);

    // Enable paging
    // After this instruction, all memory accesses will use virtual addresses
    enable_page();
}

static page_entry_t *get_pde()
{
    return (page_entry_t *)(0xfffff000);
}

static page_entry_t *get_pte(u32 vaddr)
{
    return (page_entry_t *)(0xffc00000 | (DIDX(vaddr) << 12));
}

// 刷新虚拟地址 vaddr 的 块表 TLB
static void flush_tlb(u32 vaddr)
{
    asm volatile("invlpg (%0)" ::"r"(vaddr)
                 : "memory");
}

// 从位图中扫描 count 个连续的页
static u32 scan_page(bitmap_t *map, u32 count)
{
    assert(count > 0);
    int32 index = bitmap_scan(map, count);

    if (index == EOF)
    {
        panic("Scan page fail!!!");
    }

    u32 addr = PAGE(index);
    kernel_info("Scan page 0x%p count %d\n", addr, count);
    return addr;
}

// 与 scan_page 相对，重置相应的页
static void reset_page(bitmap_t *map, u32 addr, u32 count)
{
    ASSERT_PAGE(addr);
    assert(count > 0);
    u32 index = IDX(addr);

    for (size_t i = 0; i < count; i++)
    {
        assert(bitmap_test(map, index + i));
        bitmap_set(map, index + i, 0);
    }
}

// 分配 count 个连续的内核页
u32 alloc_kpage(u32 count)
{
    assert(count > 0);
    u32 vaddr = scan_page(&kernel_map, count);
    kernel_info("ALLOC kernel pages 0x%p count %d\n", vaddr, count);
    return vaddr;
}

// 释放 count 个连续的内核页
void free_kpage(u32 vaddr, u32 count)
{
    ASSERT_PAGE(vaddr);
    assert(count > 0);
    reset_page(&kernel_map, vaddr, count);
    kernel_info("FREE  kernel pages 0x%p count %d\n", vaddr, count);
}

