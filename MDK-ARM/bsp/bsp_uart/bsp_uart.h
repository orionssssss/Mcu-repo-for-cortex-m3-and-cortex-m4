#ifndef _BSP_UART_H__
#define _BSP_UART_H__

#include "usart.h"

#define UART_MAX_INSTANCE  5 //最大支持uart对象数量
#define RECBUFLEN 255

typedef enum    
{
    UART_RECEIVE_BLOCKING = 0x00,
    UART_RECEIVE_IT,
    UART_RECEIVE_DMA,
    UART_RECEIVE_DMA_IDLE,
    // UART_RECEIVE_IT_CONTINUOUS,
    // UART_RECEIVE_DMA_CONTINUOUS,
    // UART_RECEIVE_DMA_IDLE_CONTINUOUS

}UARTReceiveMode;

typedef enum    
{
    UART_TRANSMIT_BLOCKING = 0x00,
    UART_TRANSMIT_IT,
    UART_TRANSMIT_DMA

}UARTTransmitMode;

typedef struct UARTInstance UARTInstance_s;
struct UARTInstance
{
    /* data */
    UART_HandleTypeDef *uart_id;
    void* superclass;
    UARTReceiveMode receivemode;
    UARTTransmitMode transmitmode;
    uint8_t rx_buff[RECBUFLEN];
    uint16_t rx_size;
    uint8_t (*Transmit)(UARTInstance_s * _instance, uint8_t *data, uint16_t size, uint32_t timeout);
    uint8_t (*Receive)(UARTInstance_s * _instance, uint8_t *data, uint16_t size, uint32_t timeout);
    uint8_t (*setmode)(UARTInstance_s * instance, UARTTransmitMode tx_mode, UARTReceiveMode rx_mode);
    void (*TxCallbackFunction)(UARTInstance_s * _instance); //uart发送回调函数统一接口
    void (*RxCallbackFunction)(UARTInstance_s * _instance); //uart接收回调函数统一接口
} ;

typedef struct UARTInitConfig
{
    /* data */
    UART_HandleTypeDef *uart_id;
    void* superclass;
    void (*TxCallbackFunction)(UARTInstance_s * _instance); //uart发送回调函数统一接口
    void (*RxCallbackFunction)(UARTInstance_s * _instance); //uart接收回调函数统一接口
} UARTInitConfig_t;

UARTInstance_s* UARTRegister(UARTInitConfig_t * config);
#endif // BSP_UART_H
