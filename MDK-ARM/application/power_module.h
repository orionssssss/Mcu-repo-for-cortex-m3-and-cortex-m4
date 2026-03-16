#ifndef POWER_MODULE_H_
#define POWER_MODULE_H_

#include "sitaiwei.h"
#include "lingkong.h"
#include "pushbeam.h"

typedef struct PowerModuleInstance {
    uint8_t module_id;
} PowerModuleInstance_t;
/*power module function*/
void PowerModuleInit(void);
/*private function*/
void MotorControlMain(void *argument);
void PushbeamControlMain(void *argument);
#endif /* _POWER_MODULE_H_ */