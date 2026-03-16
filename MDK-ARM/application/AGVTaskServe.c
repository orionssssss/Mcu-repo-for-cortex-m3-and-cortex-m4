#include "AGVTaskServe.h"
#include "bsp_list.h"
#include "stdlib.h"
#include "string.h"

ListInstance_t *AGVTaskList;
void AGVTaskServeInit(void)
{
    ListConfig_t list_config = {"AGVTaskList"};
    AGVTaskList = ListRegister(&list_config);
}

ListNode_t *AGVGetFirstTask(void)
{
    if(AGVTaskList == NULL || AGVTaskList->head == NULL){
        return NULL;
    }
    return AGVTaskList->head;
}

ListNode_t *AGVGetLastTask(void)
{
    if(AGVTaskList == NULL || AGVTaskList->tail == NULL){
        return NULL;
    }
    return AGVTaskList->tail;
}

ListNode_t *AGVTaskAdd(AGVTask_t *task_param)
{
    AGVTask_t *task = (AGVTask_t *)malloc(sizeof(AGVTask_t));
    if(task == NULL)
    {
        return NULL;
    }
    memcpy(task, task_param, sizeof(AGVTask_t));
    ListNodeConfig_t node_config = {
        .data = task,
        .container = AGVTaskList
    };
    ListNode_t *task_node = ListNodeRegister(&node_config, 0);
    if(task_node == NULL){
        free(task);
        return NULL;
    }
    return task_node;
}

