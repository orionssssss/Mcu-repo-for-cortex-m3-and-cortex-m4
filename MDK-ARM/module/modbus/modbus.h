/*******************************************************************************
  * FILENAME    : modbus.h
  * Date        : 2025/10/15 09:11:47
  * Author      : [Ma Weiqiang]
  * Version     : [Version Number]
  * Decription  : 此modbus依赖于uart.h和buffer.h，仅实现MODBUS-RTU uart无校验，停止位为1 
  * 				为非标准modbus协议 如需兼容标准modbus协议 请自行修改
 *******************************************************************************/
#ifndef _MODBUS_H__
#define _MODBUS_H__
#include "stdint.h"
#include "bsp_uart.h"
#include "bsp_buffer.h"

#define MODBUS_MAX_INSTANCE 5 //最大MODBUS实例数

typedef enum{
	MODBUS_SUCCESS       = 0,
	MODBUS_ADDR_ERROR    = 1,
	MODBUS_FUNTION_ERROR = 2,
	MODBUS_TIMEOUT       = 3,
	MODBUS_CRC_ERROR     = 4,
	MODBUS_FRAME_ERROR     = 5
}MODBUS_STATE;

typedef enum{
	MODBUS_CRC_MSBFITST = 0,
	MODBUS_CRC_LSBFITST = 1
}MODBUS_CRC_TYPE;


typedef struct ModbusInstance ModbusInstance_t;
typedef struct ModbusInstance{
	UARTInstance_s* uart_instance;
	char name[20];
	MODBUS_STATE state;
	uint8_t slave_addr;
	uint32_t superclass;
	Buffer_t *rx_buff;
	uint8_t modbus_sendbuf[50];		 	// 发送buff
	uint8_t modbus_decodebuf[50]; 		// 解码buff
	struct modbus_communicate_flag
	{
		uint8_t tx_finish_flag : 1;
		uint8_t rx_finish_flag : 1;
		uint8_t offline_flag : 1;
		uint8_t begin_flag : 1;
		uint8_t process_flag : 4;
	} communicate_flag;
	uint8_t respond_length;				//用于记录需要接收的长度
	uint8_t need_respond_length;		//用于记录剩余需要接收的长度
	uint16_t offline_cnt;
	MODBUS_CRC_TYPE crc_type;					//0为crc16-msbfitst  1为crc16-lsbfitst
	MODBUS_STATE (*write_one_reg)(ModbusInstance_t *_instance, uint16_t reg_addr, uint16_t *data);
	MODBUS_STATE (*read_multi_reg)(ModbusInstance_t *_instance, uint16_t reg_addr, uint16_t reg_cnt, uint16_t *data);
	void (*CallbackFunction)(ModbusInstance_t * _instance);

}ModbusInstance_t;

typedef struct{
	UARTInitConfig_t uart_config;
	char name[20];
	uint8_t slave_addr;
	MODBUS_CRC_TYPE crc_type;					//0为crc16-msbfitst  1为crc16-lsbfitst
	uint32_t superclass;
	void (*CallbackFunction)(ModbusInstance_t * _instance);
}ModbusConfig_t;

/**
 * @brief 注册一个MODBUS实例
 * 
 * @param config ：MODBUS实例配置
 * @return ModbusInstance_t* 
 */
ModbusInstance_t *ModbusRegister(ModbusConfig_t * config);
/**
 * @brief  释放一个MODBUS实例
 * @param  instance: MODBUS实例
 * @retval NONE
 */
void ModbusFree(ModbusInstance_t * instance);
/**
 * @brief  写一个寄存器
 * @param  _instance: MODBUS实例
 * @param  reg_addr: 写寄存器地址
 * @param  data: 要写入的数据区指针
 * @retval NONE
 */
MODBUS_STATE ModbusWriteOneReg(ModbusInstance_t *_instance, uint16_t reg_addr, uint16_t *data);
/**
 * @brief  读一个寄存器
 * @param  _instance: MODBUS实例
 * @param  reg_addr: 读寄存器地址
 * @param  reg_cnt: 读寄存器数
 * @param  data: 存放数据区指针
 * @retval NONE
 */
MODBUS_STATE ModbusReadMultiReg(ModbusInstance_t *_instance, uint16_t reg_addr, uint16_t reg_cnt, uint16_t *data);

#endif
