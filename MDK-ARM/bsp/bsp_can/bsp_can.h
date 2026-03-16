/*******************************************************************************
  * FILENAME    : bsp_can.h
  * Date        : 2025/09/19 17:35:00
  * Author      : [Ma Weiqiang]
  * Version     : [Version Number]
  * Decription  : 
 *******************************************************************************/
#ifndef BSP_CAN_H__
#define BSP_CAN_H__

#include "can.h"

#define CAN_MAX_INSTANCE 16 //最大支持can对象数量

typedef enum
{
    CAN_TRANSMIT_SUCCESS = 0x00,
    CAN_TRANSMIT_ERROR = 0x01,
    CAN_TRANSMIT_TIMEOUT = 0x02
}CANTransmitState;

typedef struct CANInstance_s
{
    CAN_HandleTypeDef *hcan_id;
    CAN_TxHeaderTypeDef txconfig;
    uint16_t master_id;
    uint8_t tx_buff[8];
    uint32_t CAN_TxMailBox;
    void* superclass;//指向上层对象
    uint16_t receiver_id;//接收ID
    uint8_t rx_buff[8];
    uint8_t rx_length;//接收数据长度
    uint8_t (*Transmit)(struct CANInstance_s * _instance);
    void (*CallbackFunction)(struct CANInstance_s * _instance);

}CANInstance_s;

typedef struct CANInitconfig
{
    CAN_HandleTypeDef *hcan_id;
    uint16_t master_id;
    uint16_t receiver_id;
    void* superclass;
    void (* CallbackFunction)(struct CANInstance_s * _instance);//can回调函数统一接口
}CANInitconfig_t;

/**
* @brief 注册CAN实例
* @param   config CAN初始化配置
* @return  成功返回CAN实例指针，失败返回NULL
*/
CANInstance_s* CANRegitster(CANInitconfig_t * config);

/**
 * @brief can发送数据
 * @param   instance can实例
 * @return  0:成功 1:can对象为空 2:can发送失败 3:can发送超时
 */
uint8_t CanTransmit(CANInstance_s * instance);


#endif
