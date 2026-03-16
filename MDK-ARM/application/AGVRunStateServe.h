#ifndef _AGVRUNSTATESERVE_H__
#define _AGVRUNSTATESERVE_H__

#include "stdint.h"
#include "snr9816vr.h"

#define ControlStateNum 9

//运行状态枚举
typedef enum
{
    AGV_STAGE_INIT = 0,      // 初始化状态
    AGV_STAGE_MOTOR_INIT,        // 马达通信初始化
    AGV_STAGE_PLATFORM_ZERO_INIT, // 托盘回原点初始化
    AGV_STAGE_WAIT_FOR_TASK,      // 待机状态
    AGV_STAGE_GET_READY_FOR_MOVE,   // 为移动状态准备
    AGV_STAGE_RUN,              // 运行状态
    AGV_STAGE_SLEEP,            // 休眠状态
    AGV_STAGE_ERROR,            // 故障状态
}ControlState;

typedef enum
{
    AGV_EVENT_INIT = 0,      // 初始化状态
    AGV_EVENT_MOTOR_INIT,        // 马达通信初始化
    AGV_EVENT_PLATFORM_ZERO_INIT, // 托盘回原点初始化
    AGV_EVENT_WAIT_FOR_TASK,      // 待机状态
    AGV_EVENT_GET_READY_FOR_MOVE,   // 为移动状态准备
    AGV_EVENT_RUN,              // 运行状态
    AGV_EVENT_SLEEP,            // 休眠状态
    AGV_EVENT_ERROR,            // 故障状态
}Event_e;

typedef struct
{
    uint8_t move_speed;
    uint8_t move_diretion;
}MoveParam_t;

typedef struct
{
    uint8_t led_mode;
    uint8_t led_color;
}LedParam_t;

typedef struct
{
    VoiceID_e voice_id;
}VoiceParam_t;

typedef struct
{
    VoiceID_e voice_id;
}GunParam_t;

typedef enum
{
    RUN_STATE_NULL = 0,
    RUN_STATE_STANDBY,
    RUN_STATE_WORKING,
    RUN_STATE_ARRIVE,
    RUN_STATE_CHARGE,
    RUN_STATE_ALARM
}
RunState_e;

/*枪状态：
0x00：待复位
0x01：下降中
0x02：等待导向到下降限位
0x03：下降完成
0x04：回收中
0x05：等待导向到回收限位
0x06：回收完成 */
typedef enum
{
    GUN_STATE_WAIT_INIT = 0,
    GUN_STATE_FALLING,
    GUN_STATE_FALLING_WAIT_LIMIT,
    GUN_STATE_FINISH_FALLING,
    GUN_STATE_RISING,
    GUN_STATE_RISING_WAIT_LIMIT,
    GUN_STATE_FINISH_RISING

}
GunState_e;

/*报警信息：
0,      无错误
1,     急停按下报警
2,      CAN总线初始化故障
3,      马达1通信超时
4,      马达2通信超时
5,      马达3通信超时
6,      马达4通信超时
7,      马达5通信超时
8,      马达6通信超时
9,      马达7通信超时
10,    马达8通信超时
11,     马达9通信超时
12,     马达1故障报警
13,     马达2故障报警
14,      马达3故障报警
15,      马达4故障报警
16,      马达5故障报警
17,      马达6故障报警
18,      马达7故障报警
19,      马达8故障报警
20,      马达9故障报警
21,     马达1控制故障
22,     马达2控制故障
23,     马达3控制故障
24,     马达4控制故障
25,     马达5控制故障
26,     马达6控制故障
27,     马达7控制故障
28,     马达8控制故障
29,     马达9控制故障
30,     移动过程中遇到障碍
31,     丢失二维码
32,     充电枪未完全插入*/
typedef enum
{
    ERROR_NULL                =0, 
    ERROR_EMSTOP              =1, 
    ERROR_CAN_INIT            =2, 
    /*马达通信超时*/
    ERROR_MOTOR1_TIMEOUT      =3, 
    ERROR_MOTOR2_TIMEOUT      =4, 
    ERROR_MOTOR3_TIMEOUT      =5, 
    ERROR_MOTOR4_TIMEOUT      =6, 
    ERROR_MOTOR5_TIMEOUT      =7, 
    ERROR_MOTOR6_TIMEOUT      =8, 
    ERROR_MOTOR7_TIMEOUT      =9, 
    ERROR_MOTOR8_TIMEOUT      =10,
    ERROR_MOTOR9_TIMEOUT      =11,
    /*马达报警*/
    ERROR_MOTOR1_ALARM        =12,
    ERROR_MOTOR2_ALARM        =13,
    ERROR_MOTOR3_ALARM        =14,
    ERROR_MOTOR4_ALARM        =15,
    ERROR_MOTOR5_ALARM        =16,
    ERROR_MOTOR6_ALARM        =17,
    ERROR_MOTOR7_ALARM        =18,
    ERROR_MOTOR8_ALARM        =19,
    ERROR_MOTOR9_ALARM        =20,
    /*马达控制故障*/
    ERROR_MOTOR1_CONTROL      =21,
    ERROR_MOTOR2_CONTROL      =22,
    ERROR_MOTOR3_CONTROL      =23,
    ERROR_MOTOR4_CONTROL      =24,
    ERROR_MOTOR5_CONTROL      =25,
    ERROR_MOTOR6_CONTROL      =26,
    ERROR_MOTOR7_CONTROL      =27,
    ERROR_MOTOR8_CONTROL      =28,
    ERROR_MOTOR9_CONTROL      =29,
    /*运行故障*/
    ERROR_BLOCK_ALARM         =30,
    ERROR_LOST_QRCODE         =31,
    /*枪故障*/
    ERROR_INSERT_FAIL         =32,
    ERROR_INSET_NOT_ALLOW     =33

}ErrorState_e;

typedef struct
{
    MoveParam_t move_param;
    LedParam_t led_param;//flash and color
    VoiceParam_t voice_param;//voice id
    GunParam_t gun_param;

    RunState_e run_state;
    GunState_e gun_state;
    ErrorState_e error_state;
}AGVRunControlParam_t;


typedef struct state_node /*压缩状态机节点结构体*/
{
    ControlState (*state_Function)(Event_e* pEvnt); /*事件处理函数指针*/
    ControlState u8StatChk; /*状态校验*/
}state_node_t;

void AGVRunStateMachineInit(void);
/************************get state and param********************************/
ControlState GetNowState(void);
RunState_e GetNowRunState(void);
GunState_e GetNowGunState(void);
ErrorState_e GetNowErrorState(void);
LedParam_t  GetNowRGBPlayType(void);
VoiceID_e GetNowVoicePlayID(void);
/*private function*/
ControlState StageInitialFuntion(Event_e* pEvnt);
ControlState StageMotorInitFuntion(Event_e* pEvnt);
ControlState StagePlatformZeroInitFuntion(Event_e* pEvnt);
ControlState StageWaitForTaskFuntion(Event_e* pEvnt);
ControlState StageRunFuntion(Event_e* pEvnt);
ControlState StageErrorFuntion(Event_e* pEvnt);
ControlState RGBConvertFuntion(Event_e* pEvnt);
void AGVRunControlMain(void *argument);
void GetCurrentControlMode(void);
void AGVAutoRun(void);


#endif