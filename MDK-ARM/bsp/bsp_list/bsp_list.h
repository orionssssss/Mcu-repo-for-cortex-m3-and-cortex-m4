/*******************************************************************************
  * FILENAME    : bsp_list.h
  * Date        : 2025/10/29 11:47:15
  * Author      : [Ma Weiqiang]
  * Version     : [Version Number]
  * Decription  : circle double diretion list
 *******************************************************************************/
#ifndef BSP_LIST_H
#define BSP_LIST_H

#include "stdint.h"

#define LIST_MAX_INSTANCE 20
#define LIST_NODE_MAX_INSTANCE 100

typedef struct list_node ListNode_t;
typedef struct ListInstance ListInstance_t;
typedef struct list_node
{
    ListNode_t *next;
    ListNode_t *prev;
    uint16_t index;
    void *data;
    void *superclass;
    ListInstance_t *container;
} ListNode_t;

typedef struct
{
    void *data;
    void *superclass;
    ListInstance_t *container;

} ListNodeConfig_t;

typedef struct ListInstance
{
    ListNode_t *head;
    ListNode_t *tail;
    char name[20];
    uint16_t size;
} ListInstance_t;

typedef struct ListConfig
{
    char name[20];
} ListConfig_t;

ListNode_t* ListNodeRegister(ListNodeConfig_t *config, uint16_t index);
uint8_t ListNodeUnRegister(ListInstance_t *instance, uint8_t from_front, uint16_t index);
ListInstance_t* ListRegister(ListConfig_t *config);

#endif // BSP_LIST_H