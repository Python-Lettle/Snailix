<!--
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-24 00:19:21
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-10-28 23:43:57
 * @Copyright: MIT License
 * @Description: 
-->
# List

## Introduction

This is a simple implementation of a linked list in C. It provides functions to initialize a linked list, add nodes to the front or back, remove nodes from the front or back, get the size of the list, and get a node by index.

In this implementation, I tried to use C to mimic Java-style naming conventions and object-oriented programming as much as possible, and the functions provided by linked lists were modeled after Java specifications.

## Test

If you want to test the list, you can paste the following code into main.c :

```c
extern void screen_init();
extern void memory_init(u32 memsize_low, u32 memsize_high);
extern void screen_print(char *buf, u32 count);

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

void kernel_main(u32 size, u32 size_high)
{
    screen_init();
    memory_init(size, size_high);

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

    while(1) ;
}
```