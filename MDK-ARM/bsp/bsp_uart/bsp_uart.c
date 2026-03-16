#include "usart.h"
#include "bsp_uart.h"
#include "string.h"
#include "stdlib.h"
#include "bsp_log.h"

 uint8_t UART_Register_Count = 0;//uart对象数量
 UARTInstance_s * UARTInstanceList[UART_MAX_INSTANCE] = {0};//uart对象列表

static uint8_t SetCommunicateMode(UARTInstance_s * instance, UARTTransmitMode tx_mode, UARTReceiveMode rx_mode)
{
    if (instance == NULL)
    {
        return 1; // Error: Invalid instance
    }
    // Set the transmit mode and buffer
    instance->transmitmode = tx_mode;
    instance->receivemode = rx_mode;
    return 0; // Success
}

static uint8_t Transmit(UARTInstance_s * instance, uint8_t *data, uint16_t size, uint32_t timeout)
{
    uint8_t result = 0;
    if (instance == NULL)
    {
        result = 1; // Error: Invalid instance
        return result;
    }
    // Transmit data using the UART peripheral
    switch (instance->transmitmode)
    {
    case UART_TRANSMIT_BLOCKING:
        result = HAL_UART_Transmit(instance->uart_id, data, size, timeout);
        break;
    case UART_TRANSMIT_IT:
        result = HAL_UART_Transmit_IT(instance->uart_id, data, size);
        break;
    case UART_TRANSMIT_DMA:
        result = HAL_UART_Transmit_DMA(instance->uart_id, data, size);
        break;
    default:
        break;
    }
    return result;
}

static uint8_t Receive(UARTInstance_s * instance, uint8_t *data, uint16_t size, uint32_t timeout)
{
    uint8_t result = 0;
    if (instance == NULL)
    {
        result = 1; // Error: Invalid instance
    }
    // Receive data using the UART peripheral
    switch (instance->receivemode)
    {
    case UART_RECEIVE_BLOCKING:
        result = HAL_UART_Receive(instance->uart_id, data, size, timeout);
        break;
    // case UART_RECEIVE_IT_CONTINUOUS:
    case UART_RECEIVE_IT:
        result = HAL_UART_Receive_IT(instance->uart_id, data, size);
        break;
    // case UART_RECEIVE_DMA_CONTINUOUS:
    case UART_RECEIVE_DMA:
        result = HAL_UART_Receive_DMA(instance->uart_id, data, size);
        break;
    // case UART_RECEIVE_DMA_IDLE_CONTINUOUS:
    case UART_RECEIVE_DMA_IDLE:
        result = HAL_UARTEx_ReceiveToIdle_DMA(instance->uart_id, data, RECBUFLEN);
        break;
    default:
        break;
    }
    return result; // Success
}



void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    // LOG("UART Rx Complete size :%d", Size);
    if(__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != 0) // Check for IDLE flag
    {
        __HAL_UART_CLEAR_IDLEFLAG(huart); // Clear IDLE 
        for (uint8_t i = 0; i < UART_Register_Count; i++)
        {
            if (UARTInstanceList[i] != NULL && UARTInstanceList[i]->uart_id->Instance == huart->Instance)
            {
                UARTInstanceList[i]->rx_size = Size;
                if (UARTInstanceList[i]->RxCallbackFunction != NULL)
                {
                    UARTInstanceList[i]->RxCallbackFunction(UARTInstanceList[i]);
                }
                    // LOG("UART Rx Complete size :%d", UARTInstanceList[i]->rx_size);
                HAL_UARTEx_ReceiveToIdle_DMA(UARTInstanceList[i]->uart_id, UARTInstanceList[i]->rx_buff, RECBUFLEN);
                UARTInstanceList[i]->uart_id->hdmarx->Instance->CR &= ~DMA_IT_TC; // Disable Transfer Complete interrupt
                break;
            }
        }
    }

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    for (uint8_t i = 0; i < UART_Register_Count; i++)
    {
        if (UARTInstanceList[i] != NULL && UARTInstanceList[i]->uart_id->Instance == huart->Instance)
        {
            UARTInstanceList[i]->rx_size = UARTInstanceList[i]->uart_id->RxXferSize;
            // LOG("UART Rx Complete size :%d", UARTInstanceList[i]->rx_size);
            if (UARTInstanceList[i]->RxCallbackFunction != NULL)
            {
                UARTInstanceList[i]->RxCallbackFunction(UARTInstanceList[i]);
            }
            switch(UARTInstanceList[i]->receivemode)
            {
                case UART_RECEIVE_IT:
                    HAL_UART_Receive_IT(UARTInstanceList[i]->uart_id, UARTInstanceList[i]->rx_buff, UARTInstanceList[i]->rx_size);
                    break;
                case UART_RECEIVE_DMA:
                    HAL_UART_Receive_DMA(UARTInstanceList[i]->uart_id, UARTInstanceList[i]->rx_buff, UARTInstanceList[i]->rx_size);
                    UARTInstanceList[i]->uart_id->hdmarx->Instance->CR &= ~DMA_IT_TC; // Disable Transfer Complete interrupt
                    break;
                default:
                    break;
            }
            break;
        }
    }
    
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	for (uint8_t i = 0; i < UART_Register_Count; i++)
    {
        if (UARTInstanceList[i] != NULL && UARTInstanceList[i]->uart_id->Instance == huart->Instance)
        {
            if (UARTInstanceList[i]->TxCallbackFunction != NULL)
            {
                UARTInstanceList[i]->TxCallbackFunction(UARTInstanceList[i]);
            }
            break;
        }
    }
}

UARTInstance_s* UARTRegister(UARTInitConfig_t * config)
{
    for(uint8_t i = 0; i < UART_Register_Count; i++)
    {
        if (UARTInstanceList[i]->uart_id->Instance == config->uart_id->Instance)
        {
            return UARTInstanceList[i]; // Error: Instance already registered
        }
    }
    UARTInstance_s *instance = (UARTInstance_s *)malloc(sizeof(UARTInstance_s));
    if (instance == NULL || config == NULL || UART_Register_Count >= UART_MAX_INSTANCE)
    {
        free(instance);
        return NULL; // Error: Memory allocation failed or invalid config or max instances reached
    }
    memset(instance, 0, sizeof(UARTInstance_s));
    
    instance->uart_id = config->uart_id;
    instance->TxCallbackFunction = config->TxCallbackFunction;
    instance->RxCallbackFunction = config->RxCallbackFunction;
    instance->superclass = config->superclass;
    instance->receivemode = UART_RECEIVE_BLOCKING; //默认阻塞接收
    instance->transmitmode = UART_TRANSMIT_BLOCKING; //默认阻塞发送
    instance->Transmit = Transmit;
    instance->Receive = Receive;
    instance->setmode = SetCommunicateMode;
    UARTInstanceList[UART_Register_Count++] = instance;
    return instance;
}

void UART_REINIT(UARTInstance_s * instance)
{
    if (instance != NULL)
    {
        HAL_UART_DeInit(instance->uart_id);
        HAL_UART_Init(instance->uart_id);
    }
}

