/*
 * @Author: Lettle 1071445082@qq.com
 * @Date: 2025-10-28 00:12:03
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-10-28 00:41:20
 * @Copyright: MIT License
 * @Description: Kernel main function
 */
#include <snailix/types.h>
#include <snailix/string.h>

void kernel_main(u32 size, u32 size_high)
{
    u8 * video_memory = (u8 *)0xb8000;

    video_memory[160 + 0] = 'S';
    video_memory[160 + 1] = 0x0f;
    video_memory[160 + 2] = 'n';
    video_memory[160 + 3] = 0x0f;
    video_memory[160 + 4] = 'a';
    video_memory[160 + 5] = 0x0f;
    video_memory[160 + 6] = 'i';
    video_memory[160 + 7] = 0x0f;
    video_memory[160 + 8] = 'l';
    video_memory[160 + 9] = 0x0f;
    video_memory[160 + 10] = 'i';
    video_memory[160 + 11] = 0x0f;
    video_memory[160 + 12] = 'x';
    video_memory[160 + 13] = 0x0f;

    while(1) ;
}