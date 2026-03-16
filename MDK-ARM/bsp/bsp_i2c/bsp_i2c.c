#include "bsp_i2c.h"
#include "stdlib.h"
#include "string.h"

 uint8_t I2C_Register_Count = 0;//uart对象数量
 I2CInstance_s * I2CInstanceList[I2C_MAX_INSTANCE] = {0};//uart对象列表

static uint8_t SetCommunicateMode(I2CInstance_s * instance, I2CTransmitMode tx_mode, I2CReceiveMode rx_mode)
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

static uint8_t MemWrite(I2CInstance_s * instance, uint16_t mem_addr,  uint16_t memSize,uint8_t *data, uint16_t size, uint32_t timeout)
{
    uint8_t result = 0;
    if (instance == NULL)
    {
        result = 1; // Error: Invalid instance
    }
    // Transmit data using the I2C peripheral
    instance->i2c_id.State = HAL_I2C_STATE_READY;
    switch (instance->transmitmode)
    {
    case I2C_TRANSMIT_BLOCKING:
        result = HAL_I2C_Mem_Write(&instance->i2c_id, instance->slave_addr, mem_addr, memSize, data, size, timeout);
        break;
    case I2C_TRANSMIT_IT:
        result = HAL_I2C_Mem_Write_IT(&instance->i2c_id, instance->slave_addr, mem_addr, memSize, (uint8_t *)data, size);
        break;
    case I2C_TRANSMIT_DMA:
        result = HAL_I2C_Mem_Write_DMA(&instance->i2c_id, instance->slave_addr, mem_addr, memSize, (uint8_t *)data, size);
        break;
    default:
        break;
    }
    return result;
}

static uint8_t MemRead(I2CInstance_s * instance, uint16_t mem_addr, uint16_t memSize, uint8_t *data, uint16_t size, uint32_t timeout)
{
    uint8_t result = 0;
    if (instance == NULL)
    {
        result = 1; // Error: Invalid instance
    }
    // Receive data using the I2C peripheral
    switch (instance->receivemode)
    {
    case I2C_RECEIVE_BLOCKING:
        result = HAL_I2C_Mem_Read(&instance->i2c_id, instance->slave_addr, mem_addr, memSize, data, size, timeout);
        break;
    case I2C_RECEIVE_IT:
        result = HAL_I2C_Mem_Read_IT(&instance->i2c_id, instance->slave_addr, mem_addr, memSize, (uint8_t *)data, size);
        break;
    case I2C_RECEIVE_DMA:
        result = HAL_I2C_Mem_Read_DMA(&instance->i2c_id, instance->slave_addr, mem_addr, memSize, (uint8_t *)data, size);
        break;
    default:
        break;
    }
    return result; // Success
}

static uint8_t Transmit(I2CInstance_s * instance, uint8_t *data, uint16_t size, uint32_t timeout)
{
    uint8_t result = 0;
    if (instance == NULL)
    {
        result = 1; // Error: Invalid instance
    }
    // Transmit data using the I2C peripheral
    instance->i2c_id.State = HAL_I2C_STATE_READY;
    switch (instance->transmitmode)
    {
    case I2C_TRANSMIT_BLOCKING:
        result = HAL_I2C_Master_Transmit(&instance->i2c_id, instance->slave_addr, data, size, timeout);
        break;
    case I2C_TRANSMIT_IT:
        result = HAL_I2C_Master_Transmit_IT(&instance->i2c_id, instance->slave_addr, (uint8_t *)data, size);
        break;
    case I2C_TRANSMIT_DMA:
        result = HAL_I2C_Master_Transmit_DMA(&instance->i2c_id, instance->slave_addr, (uint8_t *)data, size);
        break;
    default:
        break;
    }
    return result;
}

static uint8_t Receive(I2CInstance_s * instance, uint8_t *data, uint16_t size, uint32_t timeout)
{
    uint8_t result = 0;
    if (instance == NULL)
    {
        result = 1; // Error: Invalid instance
    }
    // Receive data using the I2C peripheral
    switch (instance->receivemode)
    {
    case I2C_RECEIVE_BLOCKING:
        result = HAL_I2C_Master_Receive(&instance->i2c_id, instance->slave_addr, data, size, timeout);
        break;
    case I2C_RECEIVE_IT:
        result = HAL_I2C_Master_Receive_IT(&instance->i2c_id, instance->slave_addr, data, size);
        break;
    case I2C_RECEIVE_DMA:
        result = HAL_I2C_Master_Receive_DMA(&instance->i2c_id, instance->slave_addr, data, size);
        break;
    default:
        break;
    }
    return result; // Success
}

void TxCallbackFuntion(I2C_HandleTypeDef *hi2c)
{
    for (uint8_t i = 0; i < I2C_Register_Count; i++) {
        if (I2CInstanceList[i] != NULL && I2CInstanceList[i]->i2c_id.Instance == hi2c->Instance) {
            I2CInstanceList[i]->rx_size = I2CInstanceList[i]->i2c_id.XferSize;
            if (I2CInstanceList[i]->TxCallbackFunction != NULL) {
                I2CInstanceList[i]->TxCallbackFunction(I2CInstanceList[i]);
            }
            break;
        }
    }
}
void RxCallbackFuntion(I2C_HandleTypeDef *hi2c)
{
    for (uint8_t i = 0; i < I2C_Register_Count; i++) {
        if (I2CInstanceList[i] != NULL && I2CInstanceList[i]->i2c_id.Instance == hi2c->Instance &&
            I2CInstanceList[i]->i2c_id.Devaddress == I2CInstanceList[i]->slave_addr) {
            I2CInstanceList[i]->rx_size = I2CInstanceList[i]->i2c_id.XferSize;
            if (I2CInstanceList[i]->RxCallbackFunction != NULL) {
                I2CInstanceList[i]->RxCallbackFunction(I2CInstanceList[i]);
            }
            break;
        }
    }
}
I2CInstance_s* I2CRegister(I2CInitConfig_t * config)
{
    for(uint8_t i = 0; i < I2C_Register_Count; i++)
    {
        if (I2CInstanceList[i]->i2c_id.Instance == config->i2c_id.Instance)
        {
            return I2CInstanceList[i]; // Error: Instance already registered
        }
    }
    I2CInstance_s *instance = (I2CInstance_s *)malloc(sizeof(I2CInstance_s));
    if (instance == NULL || config == NULL || I2C_Register_Count >= I2C_MAX_INSTANCE)
    {
        free(instance);
        return NULL; // Error: Memory allocation failed or invalid config or max instances reached
    }
    memset(instance, 0, sizeof(I2CInstance_s));
    
    instance->i2c_id = config->i2c_id;
    instance->slave_addr = config->slave_addr;
    instance->TxCallbackFunction = config->TxCallbackFunction;
    instance->RxCallbackFunction = config->RxCallbackFunction;
    instance->superclass = config->superclass;
    instance->receivemode = I2C_RECEIVE_BLOCKING; //默认阻塞接收
    instance->transmitmode = I2C_TRANSMIT_BLOCKING; //默认阻塞发送
    instance->Transmit = Transmit;
    instance->Receive = Receive;
    instance->MemWrite = MemWrite;
    instance->MemRead = MemRead;
    instance->setmode = SetCommunicateMode;
    I2CInstanceList[I2C_Register_Count++] = instance;
    return instance;
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    TxCallbackFuntion(hi2c);
}
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    RxCallbackFuntion(hi2c);
}
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    TxCallbackFuntion(hi2c);
}
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    RxCallbackFuntion(hi2c);
}