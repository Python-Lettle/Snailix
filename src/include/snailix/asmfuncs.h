/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-28 00:20:19
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-01 13:43:22
 * @Description: Assembler instructions to C language functions
 */
#ifndef SNAILIX_IO_H
#define SNAILIX_IO_H

#include <snailix/types.h>

extern u8 inb(u16 port);
extern u16 inw(u16 port);

extern void outb(u16 port, u8 data);
extern void outw(u16 port, u16 data);

extern void halt();

#endif // SNAILIX_IO_H