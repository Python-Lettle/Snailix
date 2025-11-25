/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-28 12:52:45
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-25 18:55:52
 * @Copyright: MIT License
 * @Description: Some constants of the screen.
 */
#ifndef SNAILIX_SCREEN_H
#define SNAILIX_SCREEN_H

// Constants of the CRT(6845) registers
#define CRT_ADDR_REG 0x3D4          // CRT(6845) Address Register               (not used)
#define CRT_DATA_REG 0x3D5          // CRT(6845) Data Register                  (not used)
#define CRT_START_ADDR_H 0xC        // Display memory start position - high bit (not used)
#define CRT_START_ADDR_L 0xD        // Display memory start position - low bit  (not used)
#define CRT_CURSOR_H 0xE            // Cursor position - high bit
#define CRT_CURSOR_L 0xF            // Cursor position - low bit

// Constants of the screen
#define SCREEN_MEM_BASE 0xB8000     // The starting position of video memory
#define SCREEN_MEM_SIZE 0x4000      // The size of video memory
#define SCREEN_MEM_END (SCREEN_MEM_BASE + SCREEN_MEM_SIZE) // The end of video memory
#define WIDTH 80                      // Width of the screen
#define HEIGHT 25                     // Height of the screen
#define ROW_SIZE (WIDTH * 2)          // Bytes per row
#define SCR_SIZE (ROW_SIZE * HEIGHT)  // Total bytes of the screen


// Constants of some special ascii code
#define ASCII_NUL 0x00
#define ASCII_ENQ 0x05
#define ASCII_BEL 0x07 // \a
#define ASCII_BS 0x08  // \b
#define ASCII_HT 0x09  // \t
#define ASCII_LF 0x0A  // \n
#define ASCII_VT 0x0B  // \v
#define ASCII_FF 0x0C  // \f
#define ASCII_CR 0x0D  // \r
#define ASCII_DEL 0x7F
#define ERASE_CHAR 0x0720 // \a + ' '


#endif // SNAILIX_SCREEN_H