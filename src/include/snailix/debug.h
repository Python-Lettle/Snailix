#ifndef SNAILIX_DEBUG_H
#define SNAILIX_DEBUG_H

#define BMB asm volatile("xchgw %bx, %bx") // bochs magic breakpoint

#endif // SNAILIX_DEBUG_H