/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-13 11:26:32
 * @LastEditors: Python-Lettle 1071445082@qq.com
 * @LastEditTime: 2025-11-13 12:45:26
 * @Copyright: MIT License
 * @Description: 
 */
#ifndef SNAILIX_DEBUG_H
#define SNAILIX_DEBUG_H

#define BMB asm volatile("xchgw %bx, %bx") // bochs magic breakpoint

int kernel_info(const char *fmt, ...);

void print_prefix(const char * prefix, const char * fmt, ...);

#endif // SNAILIX_DEBUG_H