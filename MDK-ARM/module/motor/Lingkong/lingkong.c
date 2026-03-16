#include "lingkong.h"
#include "bsp_log.h"
#include "stdlib.h"
#include "string.h"


uint8_t LingKongInstanceCount = 0;
LingKongInstance_s *LingKongInstanceList[LINGKONG_MAX_INSTANCE];

static void get_state2(LingKongInstance_s *_instance, uint8_t *buff)
{
    _instance->temperature = (uint16_t)(buff[1]);
    _instance->torque_current = (uint16_t)(buff[2]) | (uint16_t)(buff[3] << 8);
    _instance->speed = (uint16_t)(buff[4]) | (uint16_t)(buff[5] << 8);
    _instance->position = (uint16_t)(buff[6]) | (uint16_t)(buff[7] << 8);
    switch (_instance->encoder_bits)
    {
    case lk_16bit:
        _instance->angle = (_instance->position) * 360 / 65535.0f;
        break;
    case lk_14bit:
        _instance->angle = (_instance->position) * 360 / 16383.0f;
        break;
    case lk_15bit:
        _instance->angle = (_instance->position) * 360 / 32767.0f;
        break;
    case lk_18bit:
        _instance->angle = (_instance->position) * 360 / 65535.0f;
        break;
    default:
        break;
    }
    
}

static void get_state1(LingKongInstance_s *_instance, uint8_t *buff)
{
    _instance->temperature = (uint16_t)(buff[1]);
    _instance->voltage = (uint16_t)(buff[3]) | (uint16_t)(buff[4] << 8);
    _instance->error_code.all = buff[7];
}

static void get_encoder(LingKongInstance_s *_instance, uint8_t *buff)
{
    _instance->position = (uint16_t)(buff[2]) | (uint16_t)(buff[3] << 8);
    _instance->position_zero = (uint16_t)(buff[4]) | (uint16_t)(buff[5] << 8);
    _instance->position_offset = (uint16_t)(buff[6]) | (uint16_t)(buff[7] << 8);
}

static void get_angle(LingKongInstance_s *_instance, uint8_t *buff)
{
    int32_t angle_temp = 0;
    angle_temp = (buff[4]) | (buff[5] << 8) | (buff[6]) << 16 | (buff[7] << 24);
    _instance->angle = angle_temp / 1000.0f;
}

static void get_total_angle(LingKongInstance_s *_instance, uint8_t *buff)
{
    int64_t angle_temp = 0;
    memcpy(&angle_temp, buff + 1, 7);
    _instance->total_angle = angle_temp / 1000.0f;
}
static void CallbackFunction(CANInstance_s *_instance)
{
    LingKongInstance_s *superclass = (LingKongInstance_s *)_instance->superclass;
    LingKongFuntionCode_e funtion  = (LingKongFuntionCode_e)_instance->rx_buff[0];
    switch (funtion) {
        case lk_OpencloseOutput:
        case lk_Qcurrent_control:
        case lk_speed_control:
        case lk_absulote_total_position_control1:
        case lk_absulote_total_position_control2:
        case lk_absulote_position_control1:
        case lk_absulote_position_control2:
        case lk_relative_position_control1:
        case lk_relative_position_control2:
        case lk_read_realtime_state2:
            get_state2(superclass, _instance->rx_buff);break;
        case lk_read_encoder:
            get_encoder(superclass, _instance->rx_buff);break;
        case lk_read_realtime_total_angle:
            get_total_angle(superclass, _instance->rx_buff);break;
        case lk_read_realtime_angle:
            get_angle(superclass, _instance->rx_buff);break;
        case lk_read_realtime_state1:
            get_state1(superclass, _instance->rx_buff);break;
        case lk_close_cmd:
            superclass->control_mode = 0;break;
        case lk_open_cmd:
            superclass->control_mode = 1;break;
        case lk_stop_cmd:
            superclass->control_mode = 2;break;
        default:
            break;
    }
}

static bool AbsulutePositionControl(LingKongInstance_s *_instance, int32_t _position)
{
    int32_t _position1 = _position;
    _instance->can_instance->txconfig.DLC = 0x08;
    memset(_instance->can_instance->tx_buff, 0, 8);
    _instance->can_instance->tx_buff[0] = lk_absulote_total_position_control1;
    _instance->can_instance->tx_buff[4] = *(uint8_t *)(&_position1) ;
    _instance->can_instance->tx_buff[5] = *((uint8_t *)(&_position1)+1);
    _instance->can_instance->tx_buff[6] = *((uint8_t *)(&_position1)+2);
    _instance->can_instance->tx_buff[7] = *((uint8_t *)(&_position1)+3);
    if (_instance->can_instance->Transmit(_instance->can_instance) != CAN_TRANSMIT_SUCCESS)
        return false;
    return true;
}

static bool SinglePositionControl(LingKongInstance_s *_instance, uint8_t direction, int32_t _position)
{

    _instance->can_instance->txconfig.DLC = 0x08;
    memset(_instance->can_instance->tx_buff, 0, 8);
    _instance->can_instance->tx_buff[0] =  lk_absulote_position_control1;
    _instance->can_instance->tx_buff[4] = (uint8_t)(_position & 0xFF);
    _instance->can_instance->tx_buff[5] = (uint8_t)(_position >> 8 & 0xFF);
    _instance->can_instance->tx_buff[6] = (uint8_t)(_position >> 16 & 0xFF);
    _instance->can_instance->tx_buff[7] = (uint8_t)(_position >> 24 & 0xFF);
    if (_instance->can_instance->Transmit(_instance->can_instance) != CAN_TRANSMIT_SUCCESS)
        return false;
    return true;
}

static bool SpeedControl(LingKongInstance_s *_instance, int32_t _speed)
{
    _instance->can_instance->txconfig.DLC = 0x08;
    memset(_instance->can_instance->tx_buff, 0, 8);
    _instance->can_instance->tx_buff[0] = lk_speed_control;
    _instance->can_instance->tx_buff[4] = (uint8_t)(_speed & 0xFF);
    _instance->can_instance->tx_buff[5] = (uint8_t)(_speed >> 8 & 0xFF);
    _instance->can_instance->tx_buff[6] = (uint8_t)(_speed >> 16 & 0xFF);
    _instance->can_instance->tx_buff[7] = (uint8_t)(_speed >> 24 & 0xFF);
    if (_instance->can_instance->Transmit(_instance->can_instance) != CAN_TRANSMIT_SUCCESS)
        return false;
    return true;
}

static bool QcurrentControl(LingKongInstance_s *_instance, int16_t _current)
{
    _instance->can_instance->txconfig.DLC = 0x08;
    memset(_instance->can_instance->tx_buff, 0, 8);
    _instance->can_instance->tx_buff[0] = lk_Qcurrent_control;
    _instance->can_instance->tx_buff[4] = (uint8_t)(_current & 0xFF);
    _instance->can_instance->tx_buff[5] = (uint8_t)(_current >> 8 & 0xFF);
    if (_instance->can_instance->Transmit(_instance->can_instance) != CAN_TRANSMIT_SUCCESS)
        return false;
    return true;
}

static bool CloseOutput(LingKongInstance_s *_instance)
{
    _instance->can_instance->txconfig.DLC = 0x08;
    memset(_instance->can_instance->tx_buff, 0, 8);
    _instance->can_instance->tx_buff[0] = lk_close_cmd;
    if (_instance->can_instance->Transmit(_instance->can_instance) != CAN_TRANSMIT_SUCCESS)
        return false;
    return true;
}

static bool OpenOutput(LingKongInstance_s *_instance)
{
    _instance->can_instance->txconfig.DLC = 0x08;
    memset(_instance->can_instance->tx_buff, 0, 8);
    _instance->can_instance->tx_buff[0] = lk_open_cmd;
    if (_instance->can_instance->Transmit(_instance->can_instance) != CAN_TRANSMIT_SUCCESS)
        return false;
    return true;
}
bool StopOutput(LingKongInstance_s *_instance)
{
    _instance->can_instance->txconfig.DLC = 0x08;
    memset(_instance->can_instance->tx_buff, 0, 8);
    _instance->can_instance->tx_buff[0] = lk_stop_cmd;
    if (_instance->can_instance->Transmit(_instance->can_instance) != CAN_TRANSMIT_SUCCESS)
        return false;
    return true;
}
bool ClearError(LingKongInstance_s *_instance)
{
    _instance->can_instance->txconfig.DLC = 0x08;
    memset(_instance->can_instance->tx_buff, 0, 8);
    _instance->can_instance->tx_buff[0] = lk_clear_error;
    if (_instance->can_instance->Transmit(_instance->can_instance) != CAN_TRANSMIT_SUCCESS)
        return false;
    return true;
}

bool GetData(LingKongInstance_s *_instance)
{
    _instance->can_instance->txconfig.DLC = 0x08;
    memset(_instance->can_instance->tx_buff, 0, 8);
    _instance->can_instance->tx_buff[0] = lk_read_realtime_state2;
    if (_instance->can_instance->Transmit(_instance->can_instance) != CAN_TRANSMIT_SUCCESS)
        return false;
    return true;
}

bool GetAngle(LingKongInstance_s *_instance)
{
    _instance->can_instance->txconfig.DLC = 0x08;
    memset(_instance->can_instance->tx_buff, 0, 8);
    _instance->can_instance->tx_buff[0] = lk_read_realtime_angle;
    if (_instance->can_instance->Transmit(_instance->can_instance) != CAN_TRANSMIT_SUCCESS)
        return false;
    return true;
}

bool GetTotalAngle(LingKongInstance_s *_instance)
{
    _instance->can_instance->txconfig.DLC = 0x08;
    memset(_instance->can_instance->tx_buff, 0, 8);
    _instance->can_instance->tx_buff[0] = lk_read_realtime_total_angle;
    if (_instance->can_instance->Transmit(_instance->can_instance) != CAN_TRANSMIT_SUCCESS)
        return false;
    return true;
}

LingKongInstance_s *LingKongMotorRegitster(LingKongConfig_t *_config)
{
    for (uint8_t i = 0; i < LingKongInstanceCount; i++) {
        if (LingKongInstanceList[i]->can_instance->hcan_id->Instance == _config->can_config.hcan_id->Instance &&
            LingKongInstanceList[i]->can_instance->master_id == _config->can_config.master_id + 0x140)
            return LingKongInstanceList[i];
    }
    LingKongInstance_s *instance = (LingKongInstance_s *)malloc(sizeof(LingKongInstance_s));
    CANInitconfig_t _can_config  = {
         .hcan_id          = _config->can_config.hcan_id,
         .master_id        = _config->can_config.master_id + 0x140,
         .receiver_id      = _config->can_config.master_id + 0x140,
         .superclass       = instance,
         .CallbackFunction = CallbackFunction};
    CANInstance_s *can_insance = CANRegitster(&_can_config);
    if (instance == NULL || _config == NULL || can_insance == NULL || LingKongInstanceCount >= LINGKONG_MAX_INSTANCE) {
        free(instance);
        free(can_insance);
        return NULL;
    }
    memset(instance, 0, sizeof(LingKongInstance_s));
    instance->can_instance = can_insance;
    instance->motor_id = _config->can_config.master_id;
    instance->control_mode = _config->control_mode;
    instance->SetTorque = QcurrentControl;
    instance->SetSpeed = SpeedControl;
    instance->SetPosition = AbsulutePositionControl;
    instance->disable = CloseOutput;
    instance->enable = OpenOutput;
    instance->clearerr = ClearError;
    instance->getdata = GetData;
    instance->SetSinglePosition = SinglePositionControl;
    LingKongInstanceList[LingKongInstanceCount++] = instance;
    LOGINFO("LingKong Motor ID %d Registered!", instance->motor_id);
    return instance;
}