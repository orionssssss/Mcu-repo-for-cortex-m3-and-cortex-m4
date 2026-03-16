#ifndef _SITAIWEI_H_
#define _SITAIWEI_H_

#include "bsp_can.h"
#include "stdbool.h"

#define SITAIWEI_MAX_INSTANCE         10
#define SITAIWEI_COUNT_PER_REVOLUTION 16384

typedef enum {
    stw_reboot                 = 0x00,
    stw_read_version           = 0xA0,
    stw_read_realtime_Qcurrent = 0xA1,
    stw_read_realtime_speed    = 0xA2,
    stw_read_realtime_position = 0xA3,
    stw_read_realtime_state    = 0xAE,
    stw_clear_error            = 0xAF,

    stw_read_param         = 0xB0,
    stw_set_origin         = 0xB1,
    stw_set_max_speed      = 0xB2, // 只在速度模式有效,断电不保存
    stw_set_max_Qcurrent   = 0xB3, // 只在速度/位置模式有效，断电不保存
    stw_set_Qcurrent_slope = 0xB4, // 只在电流模式有效，断电不保存
    stw_set_speedmode_acc  = 0xB5, // 只在速度模式有效，断电不保存

    stw_Qcurrent_control = 0xC0,
    stw_speed_control,
    stw_absulote_position_control,
    stw_relative_position_control,
    stw_optimal_solution_position_back_origin, // 以最优角回到原点，转动不超过180°
    stw_output_switch_control = 0xCE,
    stw_turnoff_output        = 0xCF

} FuntionCode_e;

typedef struct sitaiwei SitaiweiInstance_s;
typedef struct sitaiwei {
    CANInstance_s *can_instance;
    struct {
        uint16_t boot;
        uint16_t software;
        uint16_t hardware;
        uint8_t CAN_version;
    } version;
    int16_t torque_current;
    int32_t speed;
    uint16_t position;        // count 16384为一圈
    int32_t total_position;   // count 16384为一圈
    float angle, total_angle; // 角度
    float angular_velocity;   // 角速度
    uint16_t voltage;         // 0.01V
    uint16_t current;         // 0.01A
    uint8_t temperature;
    uint8_t control_mode; // 0:关闭模式 1:电压控制 2:Q轴电流控制 3:速度控制 4:位置控制
    union {
        struct {
            uint8_t voltage_error : 1;
            uint8_t current_error : 1;
            uint8_t temperature_error : 1;
            uint8_t encoder_error : 1;
            uint8_t reserve : 2;
            uint8_t hardware_error : 1;
            uint8_t software_error : 1;
        }single;
        uint8_t all;
    }error_code;

    uint8_t pair_of_poles;   // 极对数
    float torque_constant;   // 转矩常数
    uint8_t reduction_ratio; // 减速比

    int32_t target_torque_current;    // 目标转矩电流0.001A
    int32_t target_speed;             // 0.01rpm
    int32_t target_absolute_position; // count 16384为一圈
    int32_t target_relative_position; // count 16384为一圈

    bool (*SetSpeed)(SitaiweiInstance_s *_instance, int32_t speed);
    bool (*SetTorque)(SitaiweiInstance_s *_instance, int32_t torque);
    bool (*SetPosition)(SitaiweiInstance_s *_instance, int32_t position);
    bool (*GetVersion)(SitaiweiInstance_s *_instance);
} SitaiweiInstance_s;

typedef struct
{
    CANInitconfig_t can_config;
    uint8_t control_mode;
} SitaiweiConfig_t;

/**
 * @brief 注册SiTaiWei电机
 *
 * @param _config 注册实例配置
 * @return SitaiweiInstance_s* 电机实例
 */
SitaiweiInstance_s *SiTaiWeiMotorRegitster(SitaiweiConfig_t *_config);
/**
 * @brief 读取SiTaiWei电机版本信息
 *
 * @param _instance 电机实例
 * @return true 成功
 * @return false 失败
 */
bool ReadVersion(SitaiweiInstance_s *_instance);
#endif