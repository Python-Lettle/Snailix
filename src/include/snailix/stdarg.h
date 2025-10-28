/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-22 23:21:23
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-10-28 22:39:30
 * @Copyright: MIT License
 * @Description: C header file used to implement variadic functions.
 */
#ifndef SNAILIX_STDARG_H
#define SNAILIX_STDARG_H

typedef char *va_list;

#define stack_size(t) (sizeof(t) <= sizeof(char *) ? sizeof(char *) : sizeof(t))
#define va_start(ap, v) (ap = (va_list)&v + sizeof(char *))
#define va_arg(ap, t) (*(t *)((ap += stack_size(t)) - stack_size(t)))
#define va_end(ap) (ap = (va_list)0)

#endif // SNAILIX_STDARG_H