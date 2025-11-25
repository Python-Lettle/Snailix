/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-28 00:39:15
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-25 18:44:06
 * @Copyright: MIT License
 * @Description: Provides a string library that is almost identical to the traditional C standard library.
 */
#ifndef SNAILIX_STRING_H
#define SNAILIX_STRING_H

#include <snailix/types.h>

char * strcpy(char * dest, const char * src);
char * strcat(char * dest, const char * src);
size_t strlen(const char * str);
int strcmp(const char * lhs, const char * rhs);
char * strchr(const char * str, int ch);
char * strrchr(const char * str, int ch);

int memcmp(const void * lhs, const void * rhs, size_t count);
void * memset(void * dest, int ch, size_t count);
void * memcpy(void * dest, const void * src, size_t count);
void * memchr(const void * ptr, int ch, size_t count);


#endif // SNAILIX_STRING_H