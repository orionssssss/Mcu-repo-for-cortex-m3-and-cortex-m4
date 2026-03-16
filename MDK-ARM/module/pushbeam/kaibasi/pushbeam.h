#ifndef PUSHBEAM_H_
#define PUSHBEAM_H_

#include "modbus.h"


#define PUSHBEAM_MAX_INSTANCE 5



typedef struct KBSPushbeamInstance KBSPushbeamInstance_t;
typedef struct KBSPushbeamInstance {
    ModbusInstance_t *modbus_instance;
    char name[20];
    union {
        struct {
            uint16_t slave_addr;
            uint16_t set_position;
            uint16_t set_speed;
            uint16_t init_run;
            uint16_t current_position;
            uint16_t current_speed;
            uint16_t reduction_gear_ratio;
            uint16_t daocheng;
            uint16_t limit_position;
            uint16_t init;
            uint16_t state;//0无异常 1堵转 2可能空转
        } data;
        uint16_t hword[11];
    } pushbeam_t;
    uint16_t offline_cnt;
	uint8_t offline_flag;
    uint8_t (*enable)(KBSPushbeamInstance_t * _instance);
    void (*setposition)(KBSPushbeamInstance_t *_instance, uint16_t position, uint16_t speed);
    void (*setlimitposition)(KBSPushbeamInstance_t *_instance, uint16_t limit_position);
    uint8_t (*getdata)(KBSPushbeamInstance_t *_instance);
	void (*CallbackFunction)(KBSPushbeamInstance_t * _instance);

} KBSPushbeamInstance_t;

typedef struct {
	ModbusConfig_t modbus_config;
	char name[20];
	void (*CallbackFunction)(KBSPushbeamInstance_t * _instance);
}KBSPushbeamConfig_t;

KBSPushbeamInstance_t *KBSPushbeamRegister(KBSPushbeamConfig_t * config);

#endif