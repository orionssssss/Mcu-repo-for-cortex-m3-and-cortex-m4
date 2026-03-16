#ifndef HJPUSHBEAM_H_
#define HJPUSHBEAM_H_

#include "modbus.h"


#define PUSHBEAM_MAX_INSTANCE 5


typedef struct HJPushbeamInstance HJPushbeamInstance_t;
typedef struct HJPushbeamInstance {
    ModbusInstance_t *modbus_instance;
    char name[20];
    union {
        struct {
            uint16_t set_position; //0.1mm
            uint16_t set_speed;//0.1mm/s
            uint16_t current_position;//0.1mm
            uint16_t current_speed;//0.1mm/s
            uint16_t run_state;//0停止 1伸出 2收回
            uint16_t protect_state;//0无异常 1过流 2过压
            uint16_t position_state;//0 到达目标位置 1最低点 2未到达目标位置且不在最低点
            uint16_t slow_start;//缓启动时间 0.1ms
            uint16_t slow_stops;//缓停时间 0.1ms
            uint16_t slow_stopm;//缓停开始距离 0.1mm
        } data;
        uint16_t hword[10];
    } pushbeam_t;
    uint16_t offline_cnt;
	uint8_t offline_flag;
    void (*setposition)(HJPushbeamInstance_t *_instance, uint16_t position, uint16_t speed);
    void (*SetRunParam)(HJPushbeamInstance_t *_instance, uint16_t start_time, uint16_t stop_time, uint16_t pre_stop_position);
    uint8_t (*getdata)(HJPushbeamInstance_t *_instance);
	void (*CallbackFunction)(HJPushbeamInstance_t * _instance);

} HJPushbeamInstance_t;

typedef struct {
	ModbusConfig_t modbus_config;
	char name[20];
	void (*CallbackFunction)(HJPushbeamInstance_t * _instance);
}HJPushbeamConfig_t;

HJPushbeamInstance_t *HJPushbeamRegister(HJPushbeamConfig_t * config);

#endif