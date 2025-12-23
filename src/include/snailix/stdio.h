/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-22 23:20:55
 * @LastEditors: Lettle 1071445082@qq.com
 * @LastEditTime: 2025-12-23 13:14:30
 * @Copyright: MIT License
 * @Description: Contains some standard input and output functions.
 */
#ifndef SNAILIX_STDIO_H
#define SNAILIX_STDIO_H

#include <snailix/stdarg.h>

int vsprintf(char *buf, const char *fmt, va_list args);
int sprintf(char *buf, const char *fmt, ...);

#endif //SNAILIX_STDIO_H