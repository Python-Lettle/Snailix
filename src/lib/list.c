/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-23 13:21:16
 * @LastEditors: Python-Lettle 1071445082@qq.com
 * @LastEditTime: 2025-11-13 14:26:18
 * @Copyright: MIT License
 * @Description: Implementation of linked list operations.
 */
#include <snailix/list.h>
#include <snailix/assert.h>

/* Initialize a node with a data pointer
 */
void node_init(LinkedNode *node, void *data)
{
    assert(node != NULL);
    node->prev = NULL;
    node->next = NULL;
    node->data = data;
}

/* Initialize a list
 * The new linked list's structure is as follows:
 *      NULL <-- Head <--> Tail --> NULL
 */
void list_init(LinkedList *list)
{
    
    list->head.prev = NULL;
    list->head.next = &list->tail;
    list->tail.next = NULL;
    list->tail.prev = &list->head;
}

/* Insert node before anchor
 *      prev <--> anchor <--> next
 *      prev <--> node <--> anchor <--> next
 */
void list_insert_before(LinkedNode *anchor, LinkedNode *node)
{
    node->next = anchor;
    node->prev = anchor->prev;
    anchor->prev->next = node;
    anchor->prev = node;
}

/* Insert node after anchor
 *      prev <--> anchor <--> next
 *      prev <--> anchor <--> node <--> next
 */
void list_insert_after(LinkedNode *anchor, LinkedNode *node)
{
    node->prev = anchor;
    node->next = anchor->next;
    anchor->next->prev = node;
    anchor->next = node;
}

/* Insert node to HEAD
 *      NULL <-- Head <--> Next
 *      NULL <-- Head <--> node <--> Next
 */
void list_lpush(LinkedList *list, LinkedNode *node)
{
    node->next = list->head.next;
    node->prev = &list->head;
    list->head.next = node;
    node->next->prev = node;
}

/* The node after the head node will be removed
 *      NULL <-- Head <--> node <--> Next
 *      NULL <-- Head <--> Next
 */
LinkedNode *list_lpop(LinkedList *list)
{
    assert(!list_empty(list));

    LinkedNode *node = list->head.next;

    list->head.next = node->next;
    node->next->prev = &list->head;
    node->next = NULL;
    node->prev = NULL;

    return node;
}

/* Insert node before TAIL
 *      prev <--> Tail --> NULL
 *      prev <--> node <--> Tail --> NULL
 */
void list_rpush(LinkedList *list, LinkedNode *node)
{
    node->next = &list->tail;
    node->prev = list->tail.prev;
    list->tail.prev->next = node;
    list->tail.prev = node;
}

/* The node before the TAIL node will be removed
 *      prev <--> node <--> Tail --> NULL
 *      prev <--> Tail --> NULL
 */
LinkedNode *list_rpop(LinkedList *list)
{
    assert(!list_empty(list));
    LinkedNode *node = list->tail.prev;

    node->prev->next = &list->tail;
    list->tail.prev = node->prev;
    node->next = NULL;
    node->prev = NULL;

    return node;
}

// Search whether the node is in the list
bool list_search(LinkedList *list, LinkedNode *node)
{
    LinkedNode *p = list->head.next;
    while (p != &list->tail)
    {
        if (p == node)
            return true;
        p = p->next;
    }
    return false;
}

// Get the node at the specified index
LinkedNode* list_get(LinkedList *list, u32 index)
{
    LinkedNode *p = list->head.next;
    while (index--)
    {
        p = p->next;
    }
    return p;
}

// Remove the node from the list
void list_remove(LinkedNode *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next = NULL;
    node->prev = NULL;
}

// Check if the list is empty
bool list_empty(LinkedList *list)
{
    return (list->head.next == &list->tail);
}

// Get the size of the list
u32 list_size(LinkedList *list)
{
    u32 size = 0;
    LinkedNode *p = list->head.next;
    while (p != &list->tail)
    {
        size++;
        p = p->next;
    }
    return size;
}


