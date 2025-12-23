/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-22 23:23:50
 * @LastEditors: Lettle 1071445082@qq.com
 * @LastEditTime: 2025-12-23 13:25:46
 * @Copyright: MIT License
 * @Description: Assertion related macros are defined.
 */
#ifndef SNAILIX_ASSERT_H
#define SNAILIX_ASSERT_H

void assertion_failure(char *exp, char *file, char *base, int line);

#define assert(exp) \
    if (exp)        \
        ;           \
    else            \
        assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)

void panic(const char *fmt, ...);

#endif // SNAILIX_ASSERT_H
