/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-22 23:55:09
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-03 19:40:11
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
static u8 *memory_map = (u8 *) 0x6000;
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
        // Skip page directory and page tables (0x1000, 0x2000, 0x3000) for now
        // They will be explicitly marked as used in mapping_init
        memory_map[i] = 1;
    }

    kernel_info("Total pages = %d, free pages = %d, start_page = %d\n", total_pages, free_pages, start_page);
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
    entry->user = user;
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

/**
 * @brief Allocate kernel pages
 * @param count Number of pages to allocate
 * @return Virtual address of the allocated pages
 */
u32 alloc_kpage(u32 count)
{
    if (count == 0 || free_pages < count) {
        return 0;
    }

    u32 pages_found = 0;
    u32 start_index = start_page;

    // Find contiguous free pages
    for (size_t i = start_page; i < total_pages && pages_found < count; i++) {
        if (memory_map[i] == 0) {
            if (pages_found == 0) {
                start_index = i;
            }
            pages_found++;
        } else {
            pages_found = 0;
        }
    }

    if (pages_found < count) {
        return 0;
    }

    // Mark pages as used
    for (size_t i = 0; i < count; i++) {
        memory_map[start_index + i] = 1;
    }

    free_pages -= count;
    return PAGE(start_index);
}

/**
 * @brief Free kernel pages
 * @param vaddr Virtual address of the pages to free
 * @param count Number of pages to free
 */
void free_kpage(u32 vaddr, u32 count)
{
    if (vaddr == 0 || count == 0 || IDX(vaddr) < start_page) {
        return;
    }

    u32 start_index = IDX(vaddr);

    // Mark pages as free
    for (size_t i = 0; i < count && (start_index + i) < total_pages; i++) {
        if (memory_map[start_index + i] == 1) {
            memory_map[start_index + i] = 0;
            free_pages++;
        }
    }
}

/**
 * @brief Initialize memory mapping and enable paging
 */
void mapping_init()
{
    // Use safer memory addresses that are less likely to conflict
    // Place page directory and tables above 1MB (where kernel memory starts)
    #define PAGE_DIR_ADDR (MEMORY_BASE + 0x1000)    // 0x101000
    #define PAGE_TABLE1_ADDR (MEMORY_BASE + 0x2000) // 0x102000
    #define PAGE_TABLE2_ADDR (MEMORY_BASE + 0x3000) // 0x103000

    // Get page indices for these addresses - IDX returns physical page number (physical address / PAGE_SIZE)
    u32 pdir_pfn = PAGE_DIR_ADDR / PAGE_SIZE;  // Physical Frame Number
    u32 pt1_pfn = PAGE_TABLE1_ADDR / PAGE_SIZE;
    u32 pt2_pfn = PAGE_TABLE2_ADDR / PAGE_SIZE;

    // Get memory map indices for these addresses (virtual memory tracking)
    u32 pdir_idx = IDX(PAGE_DIR_ADDR);
    u32 pt1_idx = IDX(PAGE_TABLE1_ADDR);
    u32 pt2_idx = IDX(PAGE_TABLE2_ADDR);

    // Mark these pages as used in memory map first
    memory_map[pdir_idx] = 1;
    memory_map[pt1_idx] = 1;
    memory_map[pt2_idx] = 1;
    free_pages -= 3;

    // Initialize page directory at safe address
    // IMPORTANT: At this point, we are still in real mode - all addresses are physical
    u32 *page_dir = (u32 *)PAGE_DIR_ADDR;
    ASSERT_PAGE((u32)page_dir);
    printk("[Mem Paging] Page directory located at 0x%x\n", (u32)page_dir);
    
    // Clear page directory
    memset(page_dir, 0, PAGE_SIZE);

    // Initialize first page table at safe address
    u32 *page_table1 = (u32 *)PAGE_TABLE1_ADDR;
    ASSERT_PAGE((u32)page_table1);
    printk("[Mem Paging] First page table located at 0x%x\n", (u32)page_table1);
    
    // Clear first page table
    memset(page_table1, 0, PAGE_SIZE);

    // Initialize second page table at safe address
    u32 *page_table2 = (u32 *)PAGE_TABLE2_ADDR;
    ASSERT_PAGE((u32)page_table2);
    printk("[Mem Paging] Second page table located at 0x%x\n", (u32)page_table2);
    
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
        entry_init(pte, i, 1, 1, 0);  // Present, R/W, Supervisor
    }

    // Map the second 4MB of physical memory to virtual memory (1:1 mapping)
    for (size_t i = 0; i < 1024; i++) {
        page_entry_t *pte = (page_entry_t *)&page_table2[i];
        // Physical frame number is i + 1024 (next 4MB)
        entry_init(pte, i + 1024, 1, 1, 0);  // Present, R/W, Supervisor
    }


    // Set CR3 register to point to page directory physical address
    // CR3 must contain the physical address of the page directory, not virtual
    set_cr3(PAGE_DIR_ADDR);

    // Enable paging
    // After this instruction, all memory accesses will use virtual addresses
    enable_page();
}


