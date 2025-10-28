/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-23 09:54:33
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-10-28 23:37:20
 * @Copyright: MIT License
 * @Description: A linked list structure is defined.
 */
#ifndef SNAILIX_LIST_H
#define SNAILIX_LIST_H

#include <snailix/types.h>

// Linked list node
typedef struct LinkedNode
{
    struct LinkedNode *prev; // previous node
    struct LinkedNode *next; // next node
    void *data;              // data
} LinkedNode;

// Linked list
typedef struct LinkedList
{
    LinkedNode head; // head node
    LinkedNode tail; // tail node
} LinkedList;

// Initialize a node with a data pointer
void node_init(LinkedNode *node, void *data);

// Initialize a list
void list_init(LinkedList *list);

// Insert node before anchor
void list_insert_before(LinkedNode *anchor, LinkedNode *node);

// Insert node after anchor
void list_insert_after(LinkedNode *anchor, LinkedNode *node);

// Insert node to HEAD
void list_lpush(LinkedList *list, LinkedNode *node);

// The node after the HEAD node will be removed
LinkedNode *list_lpop(LinkedList *list);

// Insert node before TAIL
void list_rpush(LinkedList *list, LinkedNode *node);

// The node before the TAIL node will be removed
LinkedNode *list_rpop(LinkedList *list);

// Search whether the node is in the list
bool list_search(LinkedList *list, LinkedNode *node);

// Get the node at the specified index
LinkedNode* list_get(LinkedList *list, u32 index);

// Remove the node from the list
void list_remove(LinkedNode *node);

// Check if the list is empty
bool list_empty(LinkedList *list);

// Get the size of the list
u32 list_size(LinkedList *list);

#endif // SNAILIX_LIST_H