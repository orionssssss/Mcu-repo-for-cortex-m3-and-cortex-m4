#ifndef AGVTASK_SERVE_H_
#define AGVTASK_SERVE_H_

#include "stdint.h"

typedef enum{
    task_none = 0,
    task_move,
    task_putaway,
    task_putdown,
}task_type_t;

typedef struct{
    uint8_t position_origin;
    uint8_t position_desinaition;
    uint8_t if_putdown_gun;
    uint8_t if_slowdown_earlier;
    uint8_t if_stop;
    uint8_t position_state;
}task_move_t;

typedef struct{
    uint8_t gun_state;//0:无 1:gun_down 2:gun_up
    uint16_t time_count;//放枪后等待时间
}task_gun_t;

typedef struct
{
    char task_name[20];
    uint8_t task_id;
    task_type_t task_type;
    union{
        task_gun_t gun_param;
        task_move_t move_param;
    }task_param;
    uint8_t task_status;
} AGVTask_t;

#endif