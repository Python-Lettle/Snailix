/*
 * @Author: Lettle 1071445082@qq.com
 * @Date: 2025-10-28 00:12:03
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-10-28 14:30:19
 * @Copyright: MIT License
 * @Description: Kernel main function
 */
#include <snailix/types.h>
#include <snailix/string.h>

extern void screen_init();
extern void screen_print(char *buf, u32 count);

void kernel_main(u32 size, u32 size_high)
{
    screen_init();

    // Test the print function.
    char * teststr = "Screen Init Done!!!";
    screen_print(teststr, strlen(teststr));

    while(1) ;
}