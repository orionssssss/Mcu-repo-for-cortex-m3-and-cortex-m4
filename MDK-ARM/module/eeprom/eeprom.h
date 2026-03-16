#ifndef _EEPROM_H__
#define _EEPROM_H__

#include "bsp_i2c.h"

typedef union 
{
  uint8_t Data[512];
  uint16_t Hword[256];
  uint32_t Word[128];
}EepromData;
/**
 * @brief  EEPROM_Init
 * @param  None
 * @retval None
 */
void EEPROM_Init(void);
/**
 * @brief  EEPROM_Write
 * @param  write_addr: 写入寄存器值
 * @param  addr_size: 要写入的寄存器长度 可以为：I2C_MEMADD_SIZE_16BIT/I2C_MEMADD_SIZE_8BIT
 * @param  data: 要写入数据区
 * @param  length: 要写入长度
 * @retval None
 */
void EEPROM_Write(uint16_t write_addr, uint8_t addr_bit_size, uint8_t *data, uint16_t length);
/**
 * @brief  EEPROM_Read
 * @param  read_addr: 读取寄存器值
 * @param  addr_size: 要读取的寄存器的位长度 可以为：8 或 16
 * @param  data: 读取数据区
 * @param  length: 读取长度
 * @retval None
 */
uint8_t EEPROM_Read(uint16_t read_addr, uint8_t addr_bit_size, uint8_t *data, uint16_t length);

#endif