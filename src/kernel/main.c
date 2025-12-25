/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-25 09:37:26
 * @LastEditors: Lettle 1071445082@qq.com
 * @LastEditTime: 2025-12-25 11:00:07
 * @Copyright: MIT License
 * @Description: 
 *      This file is responsible for initializing the kernel and 
 *      starting the execution of the operating system.
 */
#include <snailix/types.h>
#include <snailix/string.h>
#include <snailix/assert.h>
#include <snailix/printk.h>
#include <snailix/list.h>

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

void print_logo(void)
{
    // Print the logo on the screen.
    for(u32 i=0; i<LOGO_LINE; i++)
    {
        printk("%s\n", logo[i]);
    }
    printk("--------------------------------------------------------------------------------\n");
}

typedef struct Object
{
    u32 id;
} Object;

typedef struct TestData 
{
    u8 u8num;
    u16 u16num;
    u32 u32num;
    Object obj;
} TestData;
void list_test()
{
    // Test for LinkedList
    LinkedList list;
    list_init(&list);

    TestData data1 = {1, 2, 3, {1}};
    LinkedNode node1;
    node_init(&node1, &data1);
    list_lpush(&list, &node1);

    TestData data2 = {4, 5, 6, {2}};
    LinkedNode node2;
    node_init(&node2, &data2);
    list_lpush(&list, &node2);

    TestData data3 = {7, 8, 9, {3}};
    LinkedNode node3;
    node_init(&node3, &data3);
    list_lpush(&list, &node3);

    u32 listsize = list_size(&list);
    for (u32 i=0; i<listsize; i++)
    {
        LinkedNode *node = list_get(&list, i);
        TestData *data = node->data;
        printk("[Node %d] u8num: %d, u16num: %d, u32num: %d, obj.id: %d\n", i, data->u8num, data->u16num, data->u32num, data->obj.id);
    }
}

void kernel_main(void)
{
    print_logo();

    list_test();
}