/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-01 15:13:41
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-01 16:42:41
 * @Copyright: MIT License
 * @Description: 
 */
#include <snailix/asmfuncs.h>
#include <snailix/interrupt.h>
#include <snailix/assert.h>
#include <snailix/task.h>
#include <snailix/snailix.h>
#include <snailix/printk.h>


#define PIT_CHAN0_REG 0X40
#define PIT_CHAN2_REG 0X42
#define PIT_CTRL_REG 0X43

#define HZ 100
#define OSCILLATOR 1193182
#define CLOCK_COUNTER (OSCILLATOR / HZ)
#define JIFFY (1000 / HZ)

// Timeslice counter
u32 volatile jiffies = 0;
u32 jiffy = JIFFY;

void clock_handler(int vector)
{
    assert(vector == 0x20);
    send_eoi(vector);

    jiffies++;

    // kernel_info("jiffies: %d\n", jiffies);
}

void pit_init()
{
    // Set up counter 0 clocking
    outb(PIT_CTRL_REG, 0b00110100);
    outb(PIT_CHAN0_REG, CLOCK_COUNTER & 0xff);
    outb(PIT_CHAN0_REG, (CLOCK_COUNTER >> 8) & 0xff);
}

void clock_init()
{
    pit_init();
    set_interrupt_handler(IRQ_CLOCK, clock_handler);
    set_interrupt_mask(IRQ_CLOCK, true);
}