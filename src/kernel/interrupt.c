/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-01 13:18:32
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-01 13:46:10
 * @Copyright: MIT License
 * @Description: 
 */
#include <snailix/interrupt.h>
#include <snailix/global.h>
#include <snailix/printk.h>
#include <snailix/asmfuncs.h>

#define ENTRY_SIZE 0x20

#define PIC_M_CTRL 0x20 // Master PIC Control Port
#define PIC_M_DATA 0x21 // Master PIC Data Port
#define PIC_S_CTRL 0xa0 // Slave PIC Control Port
#define PIC_S_DATA 0xa1 // Slave PIC Data Port
#define PIC_EOI 0x20    // End Of Interrupt signal to PIC

gate_t idt[IDT_SIZE];
pointer_t idt_ptr;

handler_t handler_table[IDT_SIZE];
extern handler_t handler_entry_table[ENTRY_SIZE];

static char *messages[] = {
    "#DE Divide Error\0",
    "#DB RESERVED\0",
    "--  NMI Interrupt\0",
    "#BP Breakpoint\0",
    "#OF Overflow\0",
    "#BR BOUND Range Exceeded\0",
    "#UD Invalid Opcode (Undefined Opcode)\0",
    "#NM Device Not Available (No Math Coprocessor)\0",
    "#DF Double Fault\0",
    "    Coprocessor Segment Overrun (reserved)\0",
    "#TS Invalid TSS\0",
    "#NP Segment Not Present\0",
    "#SS Stack-Segment Fault\0",
    "#GP General Protection\0",
    "#PF Page Fault\0",
    "--  (Intel reserved. Do not use.)\0",
    "#MF x87 FPU Floating-Point Error (Math Fault)\0",
    "#AC Alignment Check\0",
    "#MC Machine Check\0",
    "#XF SIMD Floating-Point Exception\0",
    "#VE Virtualization Exception\0",
    "#CP Control Protection Exception\0",
};

// Notify the interrupt controller that the interrupt is finished.
void send_eoi(int vector)
{
    if (vector >= 0x20 && vector < 0x28)
    {
        outb(PIC_M_CTRL, PIC_EOI);
    }
    if (vector >= 0x28 && vector < 0x30)
    {
        outb(PIC_M_CTRL, PIC_EOI);
        outb(PIC_S_CTRL, PIC_EOI);
    }
}

u32 counter = 0;

void default_handler(int vector)
{
    send_eoi(vector);
    kernel_info("[%d] default interrupt called %d...\n", vector, counter++);
}

void exception_handler(int vector)
{
    char *message = NULL;
    if (vector < 22)
    {
        message = messages[vector];
    }
    else
    {
        message = messages[15];
    }

    kernel_info(" Exception : [0x%02X] %s \n", vector, messages[vector]);
    // 阻塞
    halt();
}


// Initialize interrupt controller
void pic_init()
{
    outb(PIC_M_CTRL, 0b00010001); // ICW1: Edge trigger, cascade 8259, need ICW4
    outb(PIC_M_DATA, 0x20);       // ICW2: Starting vector number 0x20
    outb(PIC_M_DATA, 0b00000100); // ICW3: Slave connected at IR2
    outb(PIC_M_DATA, 0b00000001); // ICW4: 8086 mode, normal EOI

    outb(PIC_S_CTRL, 0b00010001); // ICW1: Edge trigger, cascade 8259, need ICW4
    outb(PIC_S_DATA, 0x28);       // ICW2: Starting vector number 0x28
    outb(PIC_S_DATA, 2);          // ICW3: Set slave connected to master's IR2 pin
    outb(PIC_S_DATA, 0b00000001); // ICW4: 8086 mode, normal EOI

    outb(PIC_M_DATA, 0b11111110); // Mask all interrupts
    outb(PIC_S_DATA, 0b11111111); // Mask all interrupts
}

void idt_init()
{
    for (size_t i = 0; i < ENTRY_SIZE; i++)
    {
        gate_t *gate = &idt[i];

        handler_t handler = handler_entry_table[i];

        gate->offset0 = (u32)handler & 0xffff;
        gate->offset1 = ((u32)handler >> 16) & 0xffff;
        gate->selector = 1 << 3; // Code segment
        gate->reserved = 0;      // Reserved
        gate->type = 0b1110;     // Interrupt gate
        gate->segment = 0;       // System segment
        gate->DPL = 0;           // Kernel mode
        gate->present = 1;       // Valid
    }

    for (size_t i=0; i<0x20; i++)
    {
        handler_table[i] = exception_handler;
    }

    idt_ptr.base = (u32)idt;
    idt_ptr.limit = sizeof(idt) - 1;
    asm volatile("lidt idt_ptr\n");

}

void interrupt_init()
{
    pic_init();
    idt_init();
}