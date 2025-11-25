/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-28 00:13:03
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-25 18:40:09
 * @Copyright: MIT License
 * @Description: Some basic types
 */
#ifndef SNAILIX_TYPES_H
#define SNAILIX_TYPES_H

#define EOF  -1          // end of file
#define NULL ((void *)0) // null pointer
#define EOS '\0'         // end of string

#ifndef __cplusplus
#define bool _Bool
#define true 1
#define false 0
#endif

// Used to define special structs
#define _packed __attribute__((packed))

// Used to omit the stack frame of a function
#define _ofp __attribute__((optimize("omit-frame-pointer")))

#define _inline __attribute__((always_inline)) inline

typedef unsigned int size_t;

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

#endif // SNAILIX_TYPES_H