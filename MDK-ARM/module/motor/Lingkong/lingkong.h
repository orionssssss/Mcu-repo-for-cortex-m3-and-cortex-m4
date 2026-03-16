/*******************************************************************************
  * FILENAME    : lingkong.h
  * Date        : 2025/10/28 09:41:19
  * Author      : [Ma Weiqiang]
  * Version     : [Version Number]
  * Decription  : 电机发送需要添加间隔 不能一直发送 否则会丢帧
 *******************************************************************************/
#ifndef _LINGKONG_H_
#define _LINGKONG_H_

#include "bsp_can.h"
#include "stdbool.h"

#define LINGKONG_MAX_INSTANCE 8

typedef enum {
    lk_16bit = 0x00,
    lk_14bit = 0x01,
    lk_15bit = 0x02,
    lk_18bit = 0x03,
} LingKongEncoderBits_e;

typedef enum {
    lk_close_cmd = 0x80,
    lk_open_cmd = 0x88,
    lk_stop_cmd = 0x81,

    lk_OpencloseOutput = 0xA0,//只能在MS电机上实现
    lk_Qcurrent_control = 0xA1,
    lk_speed_control,
    lk_absulote_total_position_control1,
    lk_absulote_total_position_control2,
    lk_absulote_position_control1,
    lk_absulote_position_control2,
    lk_relative_position_control1,
    lk_relative_position_control2,

    lk_read_encoder = 0x90,
    lk_set_encoder_zero = 0x91,
    lk_read_realtime_total_angle = 0x92,
    lk_read_realtime_angle = 0x94,
    
    lk_read_realtime_state1    = 0x9A,
    lk_clear_error            = 0x9B,
    lk_read_realtime_state2    = 0x9C,
    lk_read_realtime_state3    = 0x9D,
    lk_set_position_zero = 0x19,

} LingKongFuntionCode_e;

typedef struct LingKongInstance LingKongInstance_s;
typedef struct LingKongInstance
{
    CANInstance_s * can_instance;
    uint16_t motor_id;
    LingKongEncoderBits_e encoder_bits;
    int8_t temperature;
    uint16_t voltage;
    int16_t torque_current;
    int16_t speed;
    uint16_t position;
    uint16_t position_zero;
    uint16_t position_offset;
    double total_angle;
    float angle;
    struct control
    {
        float set_angle;
        float set_total_angle;
        float set_speed;
        float set_torque;
    }control;
    
    union{
         struct{
        uint8_t low_voltage_protection : 1;
        uint8_t reserve0 : 2;
        uint8_t over_temperature_protection : 1;
        uint8_t reserve1 : 4;
        }single;
        uint8_t all;
    }error_code;
    uint8_t control_mode; //0:关闭模式 1:运行模式 2：停止模式
    
    bool (* SetSpeed)(LingKongInstance_s * _instance, int32_t speed);
    bool (* SetTorque)(LingKongInstance_s * _instance, int16_t torque);
    bool (* SetPosition)(LingKongInstance_s * _instance, int32_t position);
    bool (* SetSinglePosition)(LingKongInstance_s * _instance, uint8_t direction, int32_t position);
    bool (* disable)(LingKongInstance_s * _instance);
    bool (* enable)(LingKongInstance_s * _instance);
    bool (* clearerr)(LingKongInstance_s * _instance);
    bool (* getdata)(LingKongInstance_s * _instance);
}LingKongInstance_s;

typedef struct
{
    CANInitconfig_t can_config;
    LingKongEncoderBits_e encoder_bits;
    uint8_t control_mode;
} LingKongConfig_t;

/**
 * @brief 电机注册
 * 
 * @param _config 注册实例配置
 * @return LingKongInstance_s* 
 */
LingKongInstance_s *LingKongMotorRegitster(LingKongConfig_t *_config);

bool GetAngle(LingKongInstance_s *_instance);
bool GetTotalAngle(LingKongInstance_s *_instance);

#endif