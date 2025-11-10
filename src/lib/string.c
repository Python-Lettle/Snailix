/*
 * @Author: Lettle 1071445082@qq.com
 * @Date: 2025-10-28 00:40:24
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-10 13:54:11
 * @Copyright: MIT License
 * @Description: Functions for string or memory manipulation.
 */
#include <snailix/string.h>

char * strcpy(char * dest, const char * src)
{
    char *ptr = dest;
    while(true)
    {
        *ptr++ = *src++;
        if(*src == EOS) {
            *ptr = EOS;
            return dest;
        }
    }        
}

char * strcat(char * dest, const char * src)
{
    char *ptr = dest;
    while(*ptr != EOS)
        ptr++;
    while(true)
    {
        *ptr++ = *src;
        if (*src++ == EOS)
            return dest;
    }
}

size_t strlen(const char * str)
{
    char *ptr = (char *)str;
    while(*ptr != EOS)
        ptr++;
    return ptr - str;
}

int strcmp(const char * lhs, const char * rhs)
{
   while (*lhs == *rhs && *lhs != EOS && *rhs != EOS)
   {
       lhs++;
       rhs++;
   }
   return *lhs < *rhs ? -1 : *lhs > *rhs;
}

char * strchr(const char * str, int ch)
{
    char *ptr = (char *)str;
    while(true)
    {
        if (*ptr == ch)
        {
            return ptr;
        }
        if (*ptr++ == EOS)
            return NULL;
    }
}


char * strrchr(const char * str, int ch)
{
    char *last = NULL;
    char *ptr = (char *)str;
    while(true)
    {
        if (*ptr == ch)
            last = ptr;
        if (*ptr++ == EOS)
            return last;
    }
}

int memcmp(const void * lhs, const void * rhs, size_t count)
{
    char *lptr = (char *)lhs;
    char *rptr = (char *)rhs;
    while (*lptr == *rptr && count-- > 0)
    {
        lptr++;
        rptr++;
    }
    return *lptr < *rptr ? -1 : *lptr > *rptr;
}
void * memset(void * dest, int ch, size_t count)
{
    char *ptr = (char *)dest;
    while(count--)
        *ptr++ = ch;
    return dest;
}

void * memcpy(void * dest, const void * src, size_t count)
{
    char *ptr = dest;
    while (count--)
    {
        *ptr++ = *((char*)(src++));
    }
    return dest;
}

void * memchr(const void * ptr, int ch, size_t count)
{
    char *p = (char *)ptr;
    while (count--)
    {
        if (*p == ch) {
            return (void *)p;
        }
        p++;
    }
    return NULL;
}
