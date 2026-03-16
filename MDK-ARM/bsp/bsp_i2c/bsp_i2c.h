#ifndef _BSP_I2C_H__
#define _BSP_I2C_H__ 

#include "i2c.h"

#define I2C_MAX_INSTANCE  12 //最大支持i2c对象数量
#define I2C_RECBUF_LEN 1024

typedef enum    
{
    I2C_RECEIVE_BLOCKING = 0x00,
    I2C_RECEIVE_IT,
    I2C_RECEIVE_DMA,
}I2CReceiveMode;

typedef enum    
{
    I2C_TRANSMIT_BLOCKING = 0x00,
    I2C_TRANSMIT_IT,
    I2C_TRANSMIT_DMA,
}I2CTransmitMode;

typedef struct I2CInstance I2CInstance_s;
struct I2CInstance
{
    /* data */
    I2C_HandleTypeDef i2c_id;
    uint16_t slave_addr;
    uint32_t* superclass;
    I2CReceiveMode receivemode;
    I2CTransmitMode transmitmode;
    uint8_t rx_buff[I2C_RECBUF_LEN];
    uint16_t rx_size;
    uint8_t (*Transmit)(I2CInstance_s * instance, uint8_t *data, uint16_t size, uint32_t timeout);
    uint8_t (*Receive)(I2CInstance_s * instance, uint8_t *data, uint16_t size, uint32_t timeout);
    uint8_t (*MemRead)(I2CInstance_s * instance, uint16_t mem_addr, uint16_t memSize, \
        uint8_t *data, uint16_t size, uint32_t timeout);
    uint8_t (*MemWrite)(I2CInstance_s * instance, uint16_t mem_addr, uint16_t memSize, \
        uint8_t *data, uint16_t size, uint32_t timeout);
    uint8_t (*setmode)(I2CInstance_s * instance, I2CTransmitMode tx_mode, I2CReceiveMode rx_mode);
    void (*TxCallbackFunction)(I2CInstance_s * _instance); //uart发送回调函数统一接口
    void (*RxCallbackFunction)(I2CInstance_s * _instance); //uart接收回调函数统一接口
} ;

typedef struct I2CInitConfig
{
    /* data */
    I2C_HandleTypeDef i2c_id;
    uint16_t slave_addr;
    uint32_t* superclass;
    void (*TxCallbackFunction)(I2CInstance_s * _instance); //uart发送回调函数统一接口
    void (*RxCallbackFunction)(I2CInstance_s * _instance); //uart接收回调函数统一接口
} I2CInitConfig_t;


/**
 * @brief 注册I2C对象
 * 
 * @param config 
 * @return I2CInstance_s* 
 */
I2CInstance_s* I2CRegister(I2CInitConfig_t * config);
#endif