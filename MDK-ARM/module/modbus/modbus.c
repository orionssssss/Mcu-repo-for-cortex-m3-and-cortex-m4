
#include "modbus.h"
#include "CRCDataCale.h"
#include "cmsis_os2.h"
#include "string.h"
#include "stdlib.h"
#include "bsp_log.h"

#define MODBUSREADHOLD    0x03
#define MODBUSWRITESINGLE 0x06
#define MODBUSWRITEMULTI  0x10

uint8_t ModbusInstanceCount                                 = 0;
ModbusInstance_t *ModbusInstanceList[MODBUS_MAX_INSTANCE] = {0};

static MODBUS_STATE modbus_get_slave_response(ModbusInstance_t *_instance, uint8_t funtion, uint16_t reg_addr, uint16_t reg_cnt, uint16_t *rxdata, uint8_t data_cnt)
{

    uint16_t crc = 0, crc_temp = 0;
    /*接收数据超时检测*/
    while (!_instance->communicate_flag.rx_finish_flag) {
        _instance->offline_cnt++;
        if (_instance->offline_cnt >= 50) // 5s超时
        {
            _instance->offline_cnt                   = 0;
            _instance->communicate_flag.offline_flag = 1;
            // LOG("Modbus offline");
            LOGWARNING("%s offline", _instance->name);
            return MODBUS_TIMEOUT;
        }
        osDelay(10);
    }
    memset(_instance->modbus_decodebuf, 0, sizeof(_instance->modbus_decodebuf));
    Buffer_Read(_instance->rx_buff, _instance->modbus_decodebuf, _instance->respond_length); // 从循环buff读取数据
    _instance->communicate_flag.rx_finish_flag = 0;                                          // 清除接收完成标志
    /*校验帧错误*/
    if (_instance->slave_addr != _instance->modbus_decodebuf[0])
        return MODBUS_ADDR_ERROR; // MODBUS地址错误
    if (funtion != _instance->modbus_decodebuf[1])
        return MODBUS_FUNTION_ERROR; // MODBUS功能码错误
    crc_temp = (uint16_t)(_instance->modbus_decodebuf[_instance->respond_length - 2] << 8) |
               (_instance->modbus_decodebuf[_instance->respond_length - 1]);
    crc = crc16(_instance->modbus_decodebuf, _instance->respond_length - 2);
    if (crc != crc_temp)
        return MODBUS_CRC_ERROR; // crc错误
    /*数据解析*/
    switch (funtion) {
        case MODBUSREADHOLD:
            if (_instance->modbus_decodebuf[2] != data_cnt)
                return MODBUS_FRAME_ERROR; // 数据长度错误
            for (int i = 0; i < data_cnt; i += 2) {
                rxdata[i / 2] = ((uint16_t)_instance->modbus_decodebuf[3 + i] << 8) | (uint16_t)_instance->modbus_decodebuf[4 + i]; // 大端转小端
            }
            break;
        case MODBUSWRITESINGLE:
            break;
    }
    return MODBUS_SUCCESS;
}

void modbus_transmit(ModbusInstance_t *_instance, uint8_t funtion, uint16_t reg_addr, uint16_t reg_cnt, uint16_t *data, uint8_t data_cnt)
{
    uint16_t crc                                = 0;
    uint16_t EncodeBufIndex                     = 0; // 编码buff
    uint16_t receive_length                     = 0;
    _instance->modbus_sendbuf[EncodeBufIndex++] = _instance->slave_addr;
    _instance->modbus_sendbuf[EncodeBufIndex++] = funtion;
    _instance->modbus_sendbuf[EncodeBufIndex++] = (reg_addr >> 8);
    _instance->modbus_sendbuf[EncodeBufIndex++] = (reg_addr & 0xFF);
    receive_length += 2;
    /*根据功能码不同，组帧（小端转大端）*/
    switch (funtion) {
        case MODBUSREADHOLD:
            _instance->modbus_sendbuf[EncodeBufIndex++] = (uint8_t)(reg_cnt >> 8);
            _instance->modbus_sendbuf[EncodeBufIndex++] = (uint8_t)(reg_cnt & 0xFF);
            receive_length += 1 + 2 * reg_cnt;
            break;
        case MODBUSWRITESINGLE:
            if (data == NULL)
                return;
            _instance->modbus_sendbuf[EncodeBufIndex++] = (*data >> 8);
            _instance->modbus_sendbuf[EncodeBufIndex++] = (*data & 0xFF);
            receive_length += 2 + 2;
            break;
    }
    /*添加16位CRC校验码*/
    crc                                         = crc16(_instance->modbus_sendbuf, EncodeBufIndex);
    if(_instance->crc_type == MODBUS_CRC_MSBFITST){
        _instance->modbus_sendbuf[EncodeBufIndex++] = (uint8_t)(crc >> 8);
        _instance->modbus_sendbuf[EncodeBufIndex++] = (uint8_t)(crc & 0xFF);
    }
    else if(_instance->crc_type == MODBUS_CRC_LSBFITST){
        _instance->modbus_sendbuf[EncodeBufIndex++] = (uint8_t)(crc & 0xFF);
        _instance->modbus_sendbuf[EncodeBufIndex++] = (uint8_t)(crc >> 8);
    }
    receive_length += 2;
    _instance->respond_length      = receive_length;
    _instance->need_respond_length = receive_length;
   
    _instance->uart_instance->Transmit(_instance->uart_instance, _instance->modbus_sendbuf, EncodeBufIndex, 1000);
     if (_instance->communicate_flag.begin_flag == 0) {
        _instance->uart_instance->Receive(_instance->uart_instance, _instance->uart_instance->rx_buff, 1, 1000);
        _instance->communicate_flag.begin_flag = 1;
    }
    return;
}

MODBUS_STATE ModbusWriteOneReg(ModbusInstance_t *_instance, uint16_t reg_addr, uint16_t *data) // 写一个寄存器
{
    if (_instance->communicate_flag.tx_finish_flag) {
        _instance->communicate_flag.tx_finish_flag = 0;
        modbus_transmit(_instance, MODBUSWRITESINGLE, reg_addr, 0, data, 1); // 发送数据
    }

    _instance->state = modbus_get_slave_response(_instance, MODBUSWRITESINGLE, reg_addr, 0, NULL, 0); // 获取从机应答
    return _instance->state;
}

MODBUS_STATE ModbusReadMultiReg(ModbusInstance_t *_instance, uint16_t reg_addr, uint16_t reg_cnt, uint16_t *data) // 读多个寄存器
{
    if (_instance->communicate_flag.tx_finish_flag) {
        _instance->communicate_flag.tx_finish_flag = 0;
        modbus_transmit(_instance, MODBUSREADHOLD, reg_addr, reg_cnt, NULL, 0); // 发送数据
    }

    _instance->state = modbus_get_slave_response(_instance, MODBUSREADHOLD, reg_addr, 0, data, reg_cnt * 2); // 获取从机应答
    return _instance->state;
}

void TxCallback(UARTInstance_s *_instance)
{
    ModbusInstance_t *modbus_instance = (ModbusInstance_t *)_instance->superclass;
    if (!modbus_instance->communicate_flag.tx_finish_flag)
        modbus_instance->communicate_flag.tx_finish_flag = 1;
}

void RxCallback(UARTInstance_s *_instance)
{
    ModbusInstance_t *modbus_instance = (ModbusInstance_t *)_instance->superclass;

    switch (modbus_instance->communicate_flag.process_flag) {
        case 0:
            if (_instance->rx_buff[0] != modbus_instance->slave_addr) {
                break; // 地址错误直接丢弃
            }
            modbus_instance->communicate_flag.process_flag++;
            // 无错误接收剩余数据
        case 1:
            Buffer_Write(modbus_instance->rx_buff, _instance->rx_buff, _instance->rx_size); // 将数据存入循环buff
            if (!--modbus_instance->need_respond_length) {
                if (modbus_instance != NULL && modbus_instance->CallbackFunction != NULL) {
                    modbus_instance->CallbackFunction(modbus_instance);
                }
                modbus_instance->offline_cnt                     = 0;
                modbus_instance->communicate_flag.offline_flag   = 0;
                modbus_instance->communicate_flag.rx_finish_flag = 1;
                modbus_instance->communicate_flag.process_flag   = 0;
            }
            break;
        default:
            break;
    }
}

ModbusInstance_t *ModbusRegister(ModbusConfig_t *config)
{
    for (int i = 0; i < ModbusInstanceCount; i++) {
        if (strcmp(ModbusInstanceList[i]->name, config->name) == 1) {
            return ModbusInstanceList[i];
        }
    }
    ModbusInstance_t *_instance = (ModbusInstance_t *)malloc(sizeof(ModbusInstance_t));
    if (_instance == NULL || config == NULL || ModbusInstanceCount >= MODBUS_MAX_INSTANCE) // 内存分配失败//参数错误//实例数超限
    {
        free(_instance);
        return NULL;
    }
    memset(_instance, 0, sizeof(ModbusInstance_t));
    _instance->superclass = config->superclass;
    // 注册uart
    UARTInitConfig_t _uart_config = {
        .uart_id            = config->uart_config.uart_id,
        .superclass         = _instance,
        .TxCallbackFunction = TxCallback,
        .RxCallbackFunction = RxCallback};
    _instance->uart_instance = UARTRegister(&_uart_config);
    if (_instance->uart_instance == NULL) // 串口注册失败
    {
        free(_instance);
        return NULL;
    }
    // 注册循环buffer
    BufferConfig_t _RxBuffer = {
        .type       = CircularBuffer,
        .size       = RECBUFLEN,
        .superclass = (uint32_t)_instance};
    Buffer_t *_rxbuff = Buffer_Init(&_RxBuffer);
    if (_rxbuff == NULL) // Buffer注册失败
    {
        free(_instance->uart_instance);
        free(_instance);
        return NULL;
    }
    // 初始化modbus对象
    strlcpy(_instance->name, config->name, sizeof(_instance->name));
    _instance->slave_addr      = config->slave_addr;
    _instance->CallbackFunction = config->CallbackFunction;
    _instance->uart_instance->setmode(_instance->uart_instance, UART_TRANSMIT_IT, UART_RECEIVE_IT);
    _instance->rx_buff                          = _rxbuff;
    _instance->write_one_reg                      = ModbusWriteOneReg;
    _instance->read_multi_reg                     = ModbusReadMultiReg;
    _instance->communicate_flag.tx_finish_flag  = 1;
    ModbusInstanceList[ModbusInstanceCount++] = _instance;
    return _instance;
}
