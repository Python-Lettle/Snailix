/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-22 23:55:09
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-18 20:10:07
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
#include <snailix/task.h>

// Get addr page index
#define IDX(addr) ((u32)addr >> 12)
#define DIDX(addr) (((u32)addr >> 22) & 0x3ff)
#define TIDX(addr) (((u32)addr >> 12) & 0x3ff)
// Gets the start position of the page corresponding to the page index idx
#define PAGE(idx) ((u32)idx << 12)
// Check if the address is a page boundary
#define ASSERT_PAGE(addr) assert((addr & 0xfff) == 0)

#define PDE_MASK 0xFFC00000

static u32 KERNEL_PAGE_TABLE[] = {
    0x2000,
    0x3000,
};

#define KERNEL_MAP_BITS 0x4000

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
            u32 page = PAGE(i);
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
void entry_init(page_entry_t *entry, u32 index)
{
    *(u32 *)entry = 0;
    entry->present = 1;
    entry->write = 1;
    entry->user = 1;
    entry->index = index;
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
 * @brief Initialize memory mapping and enable paging
 */
void mapping_init()
{
    page_entry_t *pde = (page_entry_t *)KERNEL_PAGE_DIR;
    memset(pde, 0, PAGE_SIZE);

    idx_t index = 0;

    for (idx_t didx = 0; didx < (sizeof(KERNEL_PAGE_TABLE) / 4); didx++)
    {
        page_entry_t *pte = (page_entry_t *)KERNEL_PAGE_TABLE[didx];
        memset(pte, 0, PAGE_SIZE);

        page_entry_t *dentry = &pde[didx];
        entry_init(dentry, IDX((u32)pte));

        for (idx_t tidx = 0; tidx < 1024; tidx++, index++)
        {
            // Page 0 is not mapped to trigger a page fault on null-pointer access for easier debugging
            if (index == 0)
                continue;

            page_entry_t *tentry = &pte[tidx];
            entry_init(tentry, index);
            memory_map[index] = 1; // Mark this page as used in the physical memory map
        }
    }

    // Map the last page table entry to the page directory itself for convenient modification
    page_entry_t *entry = &pde[1023];
    entry_init(entry, IDX(KERNEL_PAGE_DIR));

    // Set the CR3 register
    set_cr3((u32)pde);

    enable_page();
}

static page_entry_t *get_pde()
{
    return (page_entry_t *)(0xfffff000);
}

static page_entry_t *get_pte(u32 vaddr, bool create)
{
    page_entry_t *pde = get_pde();
    u32 idx = DIDX(vaddr);
    page_entry_t *entry = &pde[idx];

    assert(create || (!create && entry->present));

    page_entry_t *table = (page_entry_t *)(PDE_MASK | (idx << 12));

    if (!entry->present)
    {
        kernel_info("Get and create page table entry for 0x%p\n", vaddr);
        u32 page = get_page();
        entry_init(entry, IDX(page));
        memset(table, 0, PAGE_SIZE);
    }

    return table;
}

// Flush the TLB entry for virtual address vaddr
static void flush_tlb(u32 vaddr)
{
    asm volatile("invlpg (%0)" ::"r"(vaddr)
                 : "memory");
}

// Scan count consecutive pages in the bitmap and return the first free page address
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

// Reset count consecutive pages in the bitmap starting from addr to 0
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

// Allocate count consecutive kernel pages
u32 alloc_kpage(u32 count)
{
    assert(count > 0);
    u32 vaddr = scan_page(&kernel_map, count);
    kernel_info("ALLOC kernel pages 0x%p count %d\n", vaddr, count);
    return vaddr;
}

// Free count consecutive kernel pages
void free_kpage(u32 vaddr, u32 count)
{
    ASSERT_PAGE(vaddr);
    assert(count > 0);
    reset_page(&kernel_map, vaddr, count);
    kernel_info("FREE  kernel pages 0x%p count %d\n", vaddr, count);
}

void link_page(u32 vaddr)
{
    ASSERT_PAGE(vaddr);

    page_entry_t *pte = get_pte(vaddr, true);
    page_entry_t *entry = &pte[TIDX(vaddr)];

    task_t *task = running_task();
    bitmap_t *map = task->vmap;
    u32 index = IDX(vaddr);

    // If the page is already linked, do nothing
    if (entry->present)
    {
        assert(bitmap_test(map, index));
        return;
    }

    assert(!bitmap_test(map, index));
    bitmap_set(map, index, true);

    u32 paddr = get_page();
    entry_init(entry, IDX(paddr));
    flush_tlb(vaddr);

    kernel_info("LINK from 0x%p to 0x%p\n", vaddr, paddr);
}

void unlink_page(u32 vaddr)
{
    ASSERT_PAGE(vaddr);

    page_entry_t *pte = get_pte(vaddr, true);
    page_entry_t *entry = &pte[TIDX(vaddr)];

    task_t *task = running_task();
    bitmap_t *map = task->vmap;
    u32 index = IDX(vaddr);

    if (!entry->present)
    {
        assert(!bitmap_test(map, index));
        return;
    }

    assert(entry->present && bitmap_test(map, index));

    entry->present = false;
    bitmap_set(map, index, false);

    u32 paddr = PAGE(entry->index);

    kernel_info("UNLINK from 0x%p to 0x%p\n", vaddr, paddr);
    if (memory_map[entry->index] == 1)
    {
        put_page(paddr);
    }
    flush_tlb(vaddr);
}

