/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-01 10:46:03
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-01 11:01:54
 * @Copyright: MIT License
 * @Description: 
 */
#ifndef SNAILIX_INTERRUPT_H
#define SNAILIX_INTERRUPT_H

#include <snailix/types.h>

#define IDT_SIZE 256

#define INTR_DE 0   // Divide Error
#define INTR_DB 1   // Debug
#define INTR_NMI 2  // Non-Maskable Interrupt
#define INTR_BP 3   // Breakpoint
#define INTR_OF 4   // Overflow
#define INTR_BR 5   // Bound Range Exceeded
#define INTR_UD 6   // Invalid Opcode
#define INTR_NM 7   // Device Not Available
#define INTR_DF 8   // Double Fault
#define INTR_OVER 9 // Coprocessor Segment Overrun
#define INTR_TS 10  // Invalid TSS
#define INTR_NP 11  // Segment Not Present
#define INTR_SS 12  // Stack-Segment Fault
#define INTR_GP 13  // General Protection Fault
#define INTR_PF 14  // Page Fault
#define INTR_RE1 15 // Reserved
#define INTR_MF 16  // x87 FPU Floating-Point Error
#define INTR_AC 17  // Alignment Check
#define INTR_MC 18  // Machine Check
#define INTR_XM 19  // SIMD Floating-Point Exception
#define INTR_VE 20  // Virtualization Exception
#define INTR_CP 21  // Control Protection Exception

#define IRQ_CLOCK 0      // Clock
#define IRQ_KEYBOARD 1   // Keyboard
#define IRQ_CASCADE 2    // 8259 Slave Controller
#define IRQ_SERIAL_2 3   // Serial Port 2
#define IRQ_SERIAL_1 4   // Serial Port 1
#define IRQ_PARALLEL_2 5 // Parallel Port 2
#define IRQ_SB16 5       // SB16 Sound Card
#define IRQ_FLOPPY 6     // Floppy Disk Controller
#define IRQ_PARALLEL_1 7 // Parallel Port 1
#define IRQ_RTC 8        // Real-Time Clock
#define IRQ_REDIRECT 9   // Redirected IRQ2
#define IRQ_NIC 11       // Network Interface Card
#define IRQ_MOUSE 12     // Mouse
#define IRQ_MATH 13      // Math Coprocessor x87
#define IRQ_HARDDISK 14  // ATA Hard Disk Primary Channel
#define IRQ_HARDDISK2 15 // ATA Hard Disk Secondary Channel

#define IRQ_MASTER_NR 0x20 // Master PIC Starting Vector Number
#define IRQ_SLAVE_NR 0x28  // Slave PIC Starting Vector Number

typedef struct gate_t
{
    u16 offset0;    // Segment offset bits 0 ~ 15
    u16 selector;   // Code segment selector
    u8 reserved;    // Reserved, always 0
    u8 type : 4;    // Task gate, 16-bit interrupt gate, 32-bit interrupt gate, 32-bit trap gate
    u8 segment : 1; // segment = 0 present a system segment
    u8 DPL : 2;     // Descriptor privilege level
    u8 present : 1; // is present
    u16 offset1;    // Segment offset bits 16 ~ 31
} _packed gate_t;

// Interrupt handler function type
typedef void *handler_t;

void send_eoi(int vector);

// Set interrupt handler function
void set_interrupt_handler(u32 irq, handler_t handler);
void set_interrupt_mask(u32 irq, bool enable);

// Clear IF flag, return previous IF flag
bool interrupt_disable();
// Get IF flag
bool get_interrupt_state();
// Set IF flag
void set_interrupt_state(bool state);


#endif // SNAILIX_INTERRUPT_H