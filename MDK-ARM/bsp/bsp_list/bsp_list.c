#include "bsp_list.h"
#include "bsp_log.h"
#include "stdlib.h"
#include "string.h"

uint8_t ListInstanceCount = 0;
ListInstance_t *ListInstanceList[LIST_MAX_INSTANCE] = {0};
uint8_t ListNodeInstanceCount = 0;
ListInstance_t *ListNodeInstanceList[LIST_MAX_INSTANCE] = {0};
//循环双向链表节点注册
ListNode_t* ListNodeRegister(ListNodeConfig_t *config, uint16_t index)
{
    ListNode_t *instance = (ListNode_t*)malloc(sizeof(ListNode_t));
    if(instance == NULL)
    {
        LOGWARNING("ListNodeRegister malloc error");
        return NULL;
    }
    memset(instance, 0, sizeof(ListNode_t));
    instance->data = config->data;
    instance->container = config->container;
    instance->superclass = config->superclass;
    if (instance->container->head == NULL || instance->container->tail == NULL || instance->container->size == 0){
        instance->container->head = instance;
        instance->container->tail = instance;
    }
    else if(index >= instance->container->size + 1){//添加到末尾
        instance->next = instance->container->head;//新节点下一个为开头节点
        instance->prev = instance->container->tail;//新节点前一个为结尾节点
        instance->container->tail->next = instance;//结尾的下一个为最新节点
        instance->container->head->prev = instance;//开头的前一个为最新节点
        instance->container->tail = instance;      //结尾节点设为最新节点
    }
    else if(index == 0){//添加到头部
        instance->prev = instance->container->tail;//新节点的前一个为结尾节点
        instance->next = instance->container->head;//新节点的下一个为开头节点
        instance->container->head->prev = instance;//开头节点的前一个为最新节点
        instance->container->tail->next = instance;//结尾节点的下一个为最新节点
        instance->container->head = instance;      //开头节点设置为最新节点
    }
    else{//添加到中间
        ListNode_t *node;
        node = instance->container->head;
        for(int i = 2; i <= index; i++){//从第二个开始遍历
            node = node->next;
        }
        instance->prev = node;
        instance->next = node->next;
        node->next = instance;
        node->next->prev = instance;
    }
    instance->container->size++;
    return instance;
}

uint8_t ListNodeUnRegister(ListInstance_t *instance, uint8_t from_front, uint16_t index)
{
    ListNode_t *node;
    if(index < instance->size && index > 1){
        node = instance->head;
        for(int i = 2; i <= index; i++){//从第二个开始遍历
            node = node->next;
        }
    }
    else if(index == 1){
        node = instance->head;
        instance->head = node->next;
    }
    else if(index == instance->size){
        node = instance->tail;
        instance->tail = node->prev;
    }
    node->prev->next = node->next;
    node->next->prev = node->prev;
    free(node);
    return 1;
}

ListInstance_t* ListRegister(ListConfig_t *config)
{
    ListInstance_t *list = (ListInstance_t*)malloc(sizeof(ListInstance_t));
    if(list == NULL)
    {
        LOGWARNING("ListRegister malloc error");
        return NULL;
    }
    memset(list, 0, sizeof(ListInstance_t));
    memcpy(list->name, config->name, strlen(config->name));
    ListInstanceList[ListInstanceCount++] = list;
    return list;
}