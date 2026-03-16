#include "power_module.h"
#include "ParamService.h"
#include "AGVRunStateServe.h"
#include "ServiceDriver.h"
#include "bsp_log.h"
#include "user.h"
#include "cmsis_os2.h"
#include "snr9816vr.h"


void UserInit(void)
{
    osDelay(1000); // Wait for system to stabilize
    BSPLogInit();
    LOG_CLEAR();
    LOG("system start");
    // Initialize user-related resources here
    // ParamServiceInit();
    ServiceDriverInit();
    // AGVRunStateMachineInit();
    // PowerModuleInit();
	
}