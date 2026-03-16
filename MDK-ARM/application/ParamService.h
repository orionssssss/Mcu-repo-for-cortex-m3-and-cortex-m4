#ifndef PARAMSERVICE_H_
#define PARAMSERVICE_H_

#include "stdint.h"
#define PARAM_DATA_LENGTH 2048

typedef union 
{
    uint8_t byte[2048];
    uint16_t hword[1024];
    uint32_t word[512];
}ParamData_t;

#define PARAM_BYTE_START_ADDR 100
#define PARAM_BYTE_START_INDEX 100
#define PARAM_BYTE_LENGTH 512   //512 bytes
#define PARAM_HWORD_START_ADDR 612
#define PARAM_HWORD_START_INDEX 306
#define PARAM_HWORD_LENGTH 512  //256 half words
#define PARAM_WORD_START_ADDR 1224
#define PARAM_WORD_START_INDEX 306
#define PARAM_WORD_LENGTH 512   //128 words

extern ParamData_t g_ParameterData;
/*********************************************Byte数据*********************************************/
#define DEBUG_FLAG g_ParameterData.byte[PARAM_BYTE_START_INDEX]
#define PUSHBEAM1_EXTEND_DEBUG_FLAG g_ParameterData.byte[PARAM_BYTE_START_INDEX+1]
#define PUSHBEAM1_BACK_DEBUG_FLAG g_ParameterData.byte[PARAM_BYTE_START_INDEX+2]
#define PUSHBEAM2_EXTEND_DEBUG_FLAG g_ParameterData.byte[PARAM_BYTE_START_INDEX+3]
#define PUSHBEAM2_BACK_DEBUG_FLAG g_ParameterData.byte[PARAM_BYTE_START_INDEX+4]
#define MOTOR1_FORWARD_DEBUG_FLAG g_ParameterData.byte[PARAM_BYTE_START_INDEX+5]
#define MOTOR1_REVERSE_DEBUG_FLAG g_ParameterData.byte[PARAM_BYTE_START_INDEX+6]
#define MOTOR2_FORWARD_DEBUG_FLAG g_ParameterData.byte[PARAM_BYTE_START_INDEX+7]
#define MOTOR2_REVERSE_DEBUG_FLAG g_ParameterData.byte[PARAM_BYTE_START_INDEX+8]
#define MOTOR_ENABLE_FLAG g_ParameterData.byte[PARAM_BYTE_START_INDEX+9]

#define RFID_READ_FLAG g_ParameterData.byte[200]
#define WIRELESS_READ_FLAG g_ParameterData.byte[200+1]

/*********************************************HWord数据****************************************************/
#define MOTOR1_MOVE_SPEED g_ParameterData.hword[PARAM_HWORD_START_INDEX]
#define MOTOR1_MOVE_DIR g_ParameterData.hword[PARAM_HWORD_START_INDEX+1]
#define MOTOR2_MOVE_SPEED g_ParameterData.hword[PARAM_HWORD_START_INDEX+2]
#define MOTOR2_MOVE_DIR g_ParameterData.hword[PARAM_HWORD_START_INDEX+3]
#define PUSHBEAM1_MOVE_SPEED g_ParameterData.hword[PARAM_HWORD_START_INDEX+4]
#define PUSHBEAM1_MOVE_DIR g_ParameterData.hword[PARAM_HWORD_START_INDEX+5]
#define PUSHBEAM2_MOVE_SPEED g_ParameterData.hword[PARAM_HWORD_START_INDEX+6]
#define PUSHBEAM2_MOVE_DIR g_ParameterData.hword[PARAM_HWORD_START_INDEX+7]

#define RFID_READ_ID g_ParameterData.hword[PARAM_HWORD_START_INDEX+10]

#define MOTOR1_DEBUG_MOVE_SPEED g_ParameterData.hword[PARAM_HWORD_START_INDEX+50]
#define MOTOR2_DEBUG_MOVE_SPEED g_ParameterData.hword[PARAM_HWORD_START_INDEX+51]
#define PUSHBEAM1_DEBUG_MOVE_SPEED g_ParameterData.hword[PARAM_HWORD_START_INDEX+52]
#define PUSHBEAM2_DEBUG_MOVE_SPEED g_ParameterData.hword[PARAM_HWORD_START_INDEX+53]




/*********************************************Word数据*******************************************************/
#define MACHINE_CODE_H g_ParameterData.word[PARAM_WORD_START_INDEX]
#define MACHINE_CODE_L g_ParameterData.word[PARAM_WORD_START_INDEX+1]

ParamData_t * GetParameterGroup(void);
void SaveParameter(uint8_t *data, uint8_t data_type, uint16_t index, uint16_t length);
void ParamServiceInit(void);
void ParameterLimitCheck(void);
/*private function*/
void ParamServiceMain(void *argument);
void ParameterLog(void);

#endif