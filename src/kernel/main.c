/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-25 09:37:26
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-25 18:56:24
 * @Copyright: MIT License
 * @Description: 
 *      This file is responsible for initializing the kernel and 
 *      starting the execution of the operating system.
 */
#include <snailix/types.h>
#include <snailix/string.h>

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

extern void screen_print(char *str, u32 len);
void print_logo(void)
{
    // Print the logo on the screen.
    for(u32 i=0; i<LOGO_LINE; i++)
    {
        screen_print(logo[i], strlen(logo[i]));
        screen_print("\n", 1);
    }
    screen_print("--------------------------------------------------------------------------------\n", 81);
}

void kernel_main(void)
{
    print_logo();
}