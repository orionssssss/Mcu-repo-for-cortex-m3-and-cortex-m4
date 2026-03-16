#include "bsp_can.h"
#include "string.h"
#include "stdlib.h"
#include "cmsis_os.h"
/*注册can对象*/
static uint8_t CANInstanceCount = 0;//can对象数量
static CANInstance_s* CANInstanceList[CAN_MAX_INSTANCE] = {0};//can对象列表

void CANFilterInit(CAN_HandleTypeDef * can)
{
    static CAN_FilterTypeDef filterConfig = {0};
    filterConfig.FilterBank = 0;
    filterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    filterConfig.FilterActivation = CAN_FILTER_ENABLE;
    filterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    filterConfig.FilterIdHigh = 0x0000;
    filterConfig.FilterIdLow = 0x0000;
    filterConfig.FilterMaskIdHigh = 0x0000;
    filterConfig.FilterMaskIdLow = 0x0000;
    filterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    HAL_CAN_ConfigFilter(can, &filterConfig);
}
void CANInit(void)
{
    CANFilterInit(&hcan1);
    HAL_CAN_Start(&hcan1);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO1_MSG_PENDING);
    CANFilterInit(&hcan2);
    HAL_CAN_Start(&hcan2);
    HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO1_MSG_PENDING);
}

/*can发送数据*/
uint8_t CanTransmit(CANInstance_s * instance)
{
    uint16_t wait_canfree_time = HAL_GetTick();
    if (instance == NULL)
    {
        return CAN_TRANSMIT_ERROR; //can对象为空
    }
    // while(HAL_CAN_GetTxMailboxesFreeLevel(instance->hcan_id) != 0)
    // {
    //     //等待发送邮箱空闲
    //     if((HAL_GetTick() - wait_canfree_time) > 10) //等待10ms
    //     {
    //         return CAN_TRANSMIT_TIMEOUT; //can发送超时
    //     }
    //     osDelay(1);
    // }
    if (HAL_CAN_AddTxMessage(instance->hcan_id, &instance->txconfig, instance->tx_buff, &instance->CAN_TxMailBox) != HAL_OK)
    {
        return CAN_TRANSMIT_SUCCESS; //can发送失败
    }
    return 0; //can发送成功
}

/*can接收数据*/
static void ReceiveFifoCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rxHeader = {0};
    uint8_t rx_buff[8] = {0};
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxHeader, rx_buff) != HAL_OK)
    {
        return;//接收失败
    }
    else
    {
        for(int i = 0; i < CANInstanceCount; i++)
        {
            if((CANInstanceList[i]->hcan_id->Instance == hcan->Instance) && (CANInstanceList[i]->receiver_id == rxHeader.StdId))
            {
                memcpy(CANInstanceList[i]->rx_buff, rx_buff, 8);
                CANInstanceList[i]->rx_length = rxHeader.DLC;
                if (CANInstanceList[i]->CallbackFunction != NULL)
                {
                    CANInstanceList[i]->CallbackFunction(CANInstanceList[i]);
                }
                break;
            }
        }
    }
}

/*can中断回调函数*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    ReceiveFifoCallback(hcan);
}  
/*can中断回调函数*/
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    ReceiveFifoCallback(hcan);
}  

/*CAN对象注册*/
CANInstance_s* CANRegitster(CANInitconfig_t * config)
{
    if(!CANInstanceCount)
    {
        CANInit();
    }
    for (size_t i = 0; i < CANInstanceCount; i++)
    {
        if (CANInstanceList[i]->hcan_id->Instance == config->hcan_id->Instance && \
            CANInstanceList[i]->master_id == config->master_id)
        {
            return CANInstanceList[i]; //已存在的CAN实例
        }
    }
    CANInstance_s *instance = (CANInstance_s*)malloc(sizeof(CANInstance_s));
    if(config == NULL || instance == NULL || CANInstanceCount >= CAN_MAX_INSTANCE)
    {
        free(instance);
        return NULL; //配置对象为空
    }
    memset(instance, 0, sizeof(CANInstance_s));
    
    instance->hcan_id = config->hcan_id;
    instance->master_id = config->master_id;
    instance->receiver_id = config->receiver_id;
    instance->txconfig.StdId = config->master_id;
    instance->txconfig.IDE = CAN_ID_STD;
    instance->txconfig.RTR = CAN_RTR_DATA;
    instance->txconfig.DLC = 8; //默认发送8字节
    instance->Transmit = CanTransmit;
    instance->superclass = config->superclass;
    instance->CallbackFunction = config->CallbackFunction;
    
    CANInstanceList[CANInstanceCount++] = instance;
    return instance;
}