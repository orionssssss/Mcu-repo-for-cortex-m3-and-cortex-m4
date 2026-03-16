#include "eeprom.h"
#include "bsp_log.h"
#include "ParamService.h"
#include "string.h"
#include "cmsis_os2.h"

ParamData_t g_ParameterData;
typedef enum {
    PARAM_BYTE_TYPE = 0,
    PARAM_HWORD_TYPE,
    PARAM_WORD_TYPE,
}ParamType_e;
/*taskparam*/
osThreadId_t ParamServiceMainHandle;
const osThreadAttr_t ParamServiceMain_attributes = {
  .name = "ParamServiceMain",
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
void ParamServiceInit(void)
{
    ParamServiceMainHandle = osThreadNew(ParamServiceMain, NULL, &ParamServiceMain_attributes);
}

void ParamServiceMain(void *argument)
{
    static uint8_t return_state = 0;
    EEPROM_Init();
    return_state = EEPROM_Read(100, 16, g_ParameterData.byte, PARAM_DATA_LENGTH/2);
    if(return_state == HAL_OK)
        LOG("ParamServiceInit SUCESS");
    for (;;)
    {
        ParameterLog();
        osDelay(1000);
    }
}

void ParameterLimitCheck(void)
{
    RFID_READ_FLAG = 1;
    RFID_READ_ID = 2;
    
    
}
void ParameterLog(void)
{
    if(DEBUG_FLAG == 1)
     LOGINFO("DEBUG_START");
    if(PUSHBEAM1_EXTEND_DEBUG_FLAG == 1)
        LOGINFO("PUSH1_EXTEND");
    if(PUSHBEAM1_BACK_DEBUG_FLAG == 1)
        LOGINFO("PUSH1_BACK");
    if(PUSHBEAM2_EXTEND_DEBUG_FLAG == 1)
        LOGINFO("PUSH2_EXTEND");
    if(PUSHBEAM2_BACK_DEBUG_FLAG == 1)
        LOGINFO("PUSH2_BACK");
    LOGINFO("PUSH1_SPEED:%d", PUSHBEAM1_MOVE_SPEED);
    LOGINFO("PUSH2_SPEED:%d", PUSHBEAM2_MOVE_SPEED);
    LOGINFO("MOTOR1_SPEED:%d", MOTOR1_MOVE_SPEED);
    LOGINFO("MOROT2_SPEED:%d", MOTOR2_MOVE_SPEED);
    LOGINFO("MACHINE_CODE_H:%d", MACHINE_CODE_H);
    LOGINFO("MACHINE_CODE_H:%d", MACHINE_CODE_L);
    
}

ParamData_t * GetParameterGroup(void)
{
    return &g_ParameterData;
}

void SaveParameter(uint8_t *data, uint8_t data_type, uint16_t index, uint16_t length)
{
    
    switch ((ParamType_e)data_type)
    {
    case PARAM_BYTE_TYPE:
        memcpy(&g_ParameterData.byte[index], data, length);
        break;
    case PARAM_HWORD_TYPE:
        memcpy(&g_ParameterData.hword[index], data, length*2);
        break;
    case PARAM_WORD_TYPE:
        memcpy(&g_ParameterData.word[index], data, length*4);
        break;
    default:
        break;
    }
    
}

void ParameterInit(void)
{
    
}