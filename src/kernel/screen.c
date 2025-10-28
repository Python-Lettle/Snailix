/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-28 00:25:37
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-10-28 14:29:52
 * @Copyright: MIT License
 * @Description:
 *      This file is responsible for writing the output to the screen, 
 * which in this case is the text mode in the CGA (Color Graphics Adapter).
 */
#include <snailix/asmfuncs.h>
#include <snailix/screen.h>

// ==================================================
// Static variables
// ==================================================
#define LOGO_LINE 11
static char *logo[LOGO_LINE] = 
{
    "   &$$$::;;x$&&&X;;&   &;;$& ",
    ".&+;:+;:+:::;.& .:&&;$+. &&& ",
    "&;$;;;;;:X$X;:X;:. .& &&. :$ ",
    "$  ..:::...;:;;X+& &    &&.  ",
    "$$+&&&&&&::;+:;; &:&    $&   ",
    "& .$+  $$&:.X$:+ & +&   &&   ",
    "&;:&$+$ $$$.:::+.& &$   &+    ____              _ _ _",
    ";&.:&&& &X&.++;:.& &X&  &    / ___| _ __   __ _(_) (_)_  __",
    "  &:. :.;X&:;+;:&   &&  &    \\___ \\| '_ \\ / _` | | | \\ \\/ /",
    "   &+;;:&X$.::;:$&&X&.&&&     ___) | | | | (_| | | | |>  < ",
    "&xXX:;;&&$;xXx   ::::. :X&&  |____/|_| |_|\\__,_|_|_|_/_/\\_\\",
};

// The video memory address, the next print position.
static u32 screen;                  // Address of the next print position in the video memory.
static u32 screen_x, screen_y;      // Screen next print position -> (x, y)

static u8 char_attr = 7; // Character attribute

// ==================================================
// Static functions
// ==================================================
// Set the cursor position in the CRT controller.
static void set_cursor()
{
    outb(CRT_ADDR_REG, CRT_CURSOR_H);
    outb(CRT_DATA_REG, ((screen - SCREEN_MEM_BASE) >> 9) & 0xFF);
    outb(CRT_ADDR_REG, CRT_CURSOR_L);
    outb(CRT_DATA_REG, ((screen - SCREEN_MEM_BASE) >> 1) & 0xFF);
}

static void calculate_screen()
{
    screen = screen_y * ROW_SIZE + screen_x * 2;
}

static void get_screen_position()
{
    screen_x = screen % ROW_SIZE;
    screen_y = screen / ROW_SIZE;
}

// Clean the screen by set all the video memory to ' ' character.
static void screen_clear()
{
    u16 *p = (u16 *)SCREEN_MEM_BASE;
    while (p < (u16 *)SCREEN_MEM_END)
    {
        *p++ = ERASE_CHAR;
    }
}

static void scroll_up()
{
    // If the screen is full, scroll up all the video memory area.
    // The first line will be erased.
    if (screen_x >= WIDTH - 1)
    {
        // Move the area from the 2nd row to the end up one row.
        memcpy((void *)SCREEN_MEM_BASE, (void *)(SCREEN_MEM_BASE + ROW_SIZE), SCR_SIZE - ROW_SIZE);
        screen = SCREEN_MEM_BASE + (HEIGHT - 2) * ROW_SIZE;
        screen_y = HEIGHT - 2;

        // Clear the last line.
        u32 *ptr = (u32 *)(screen + ROW_SIZE * (HEIGHT - 1));
        for (size_t i = 0; i < WIDTH; i++)
        {
            *ptr++ = ERASE_CHAR;
        }
    }

    // Set the cursor at the beginning of the last row of the screen.
    screen += ROW_SIZE;
    screen_x = 0;
    screen_y += 1;
    set_cursor();
}

static void command_lf()
{
    // If the cursor is not at the last line.
    if (screen_y < HEIGHT - 1)
    {
        screen_y++;
        screen += ROW_SIZE;
        return;
    }
    scroll_up();
}

// carriage return '\r'
static void command_cr()
{
    screen -= (screen_x << 1);
    screen_x = 0;
}

// backspace '\b'
static void command_bs()
{
    if (screen_x)
    {
        screen_x--;
        screen -= 2;
        *(u16 *)screen = ERASE_CHAR;
    }
}

static void command_del()
{
    *(u16 *)screen = ERASE_CHAR;
}

// ==================================================
// Non static functions
// ==================================================
/**
 * @description: Print a string to the screen.
 * @param {char*} buf - The string to be printed.
 * @param {u32} count - The length of the string to be printed.
 */
void screen_print(char *buf, u32 count)
{
    char ch;
    while (*buf != EOS)
    {
        ch = *buf++;
        switch (ch)
        {
        case ASCII_NUL:
            break;
        case ASCII_BEL:
            break;
        case ASCII_BS:
            command_bs();
            break;
        case ASCII_HT:
            break;
        case ASCII_LF:
            command_lf();
            command_cr();
            break;
        case ASCII_VT:
            break;
        case ASCII_FF:
            command_lf();
            break;
        case ASCII_CR:
            command_cr();
            break;
        case ASCII_DEL:
            command_del();
            break;
        default:
            if (screen_x >= WIDTH)
            {
                screen_x = 0;
                screen -= ROW_SIZE;
                command_lf();
            }

            *((char *)screen) = ch;
            screen++;
            *((char *)screen) = char_attr;
            screen++;

            screen_x++;

            break;
        }
    }

    set_cursor();
}

/**
 * @description: Initialize the screen.
 */
void screen_init()
{
    // 1. Set the video memory address to the beginning of the video memory (0xB8000).
    screen = SCREEN_MEM_BASE;
    screen_x = 0;
    screen_y = 0;
    // 2. Set the cursor position.
    set_cursor();
    // 3. Clear the screen.
    screen_clear();
}
