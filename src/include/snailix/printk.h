/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-22 23:20:04
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-10-29 10:48:00
 * @Copyright: MIT License
 * @Description: Kernel print function
 */
#ifndef SNAILIX_PRINTK_H
#define SNAILIX_PRINTK_H

int printk(const char *fmt, ...);

#define KERNEL_INFO_ON true
int kernel_info(const char *fmt, ...);

#endif // SNAILIX_PRINTK_H