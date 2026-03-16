#include "sitaiwei.h"
#include "stdlib.h"
#include "string.h"

static uint8_t SitaiweiInstanceCount                                   = 0;
static SitaiweiInstance_s *SitaiweiInstanceList[SITAIWEI_MAX_INSTANCE] = {NULL};

static void get_position(SitaiweiInstance_s *_instance, uint8_t *buff)
{
    _instance->position       = (uint16_t)(buff[1]) | (uint16_t)(buff[2] << 8);
    _instance->total_position = (int32_t)(buff[3]) | (int32_t)(buff[4] << 8) | (int32_t)(buff[5] << 16) | (int32_t)(buff[6] << 24);
    _instance->angle          = (float)(_instance->position) / (float)(_instance->reduction_ratio) / (float)(SITAIWEI_COUNT_PER_REVOLUTION) * 360.0f;
    _instance->total_angle    = (float)(_instance->total_position) / (float)(_instance->reduction_ratio) / (float)(SITAIWEI_COUNT_PER_REVOLUTION) * 360.0f;
}

static void get_speed(SitaiweiInstance_s *_instance, uint8_t *buff)
{
    _instance->speed            = (int32_t)(buff[1]) | (int32_t)(buff[2] << 8) | (int32_t)(buff[3] << 16) | (int32_t)(buff[4] << 24);
    _instance->angular_velocity = (float)(_instance->speed) / (float)(_instance->reduction_ratio) / 60.0f * 360.0f;
}

static void get_version(SitaiweiInstance_s *_instance, uint8_t *buff)
{
    _instance->version.boot        = (uint16_t)(buff[1]) | (uint16_t)(buff[2] << 8);
    _instance->version.software    = (uint16_t)(buff[3]) | (uint16_t)(buff[4] << 8);
    _instance->version.hardware    = (uint16_t)(buff[5]) | (uint16_t)(buff[6] << 8);
    _instance->version.CAN_version = (buff[7]);
}

static void get_state(SitaiweiInstance_s *_instance, uint8_t *buff)
{
    _instance->voltage        = (uint16_t)(buff[1]) | (uint16_t)(buff[2] << 8);
    _instance->current        = (uint16_t)(buff[3]) | (uint16_t)(buff[4] << 8);
    _instance->temperature    = buff[5];
    _instance->control_mode   = buff[6];
    _instance->error_code.all = buff[7];
}

static void CallbackFunction(struct CANInstance_s *_instance)
{
    SitaiweiInstance_s *superclass = (SitaiweiInstance_s *)_instance->superclass;
    FuntionCode_e funtion          = (FuntionCode_e)_instance->rx_buff[0];
    switch (funtion) {
        case stw_read_realtime_position:
        case stw_absulote_position_control:
        case stw_relative_position_control:
        case stw_optimal_solution_position_back_origin:
            get_position(superclass, _instance->rx_buff);
            break;
        case stw_read_realtime_speed:
        case stw_speed_control:
            get_speed(superclass, _instance->rx_buff);
            break;
        case stw_read_realtime_Qcurrent:
        case stw_Qcurrent_control:
            superclass->torque_current = (int32_t)(_instance->rx_buff[1] << 24) | (int32_t)(_instance->rx_buff[2] << 16) |
                                         (int32_t)(_instance->rx_buff[3] << 8) | (int32_t)(_instance->rx_buff[4]);
            break;
        case stw_read_version:
            get_version(superclass, _instance->rx_buff);
            break;
        case stw_read_realtime_state:
        case stw_turnoff_output:
            get_state(superclass, _instance->rx_buff);
            break;
        case stw_clear_error:
            break;
        case stw_output_switch_control:
            break;
        default:
            break;
    }
}

static bool AbsulutePositionControl(SitaiweiInstance_s *_instance, int32_t _position)
{
    _instance->can_instance->txconfig.DLC = 0x05;
    memset(_instance->can_instance->tx_buff, 0, 8);
    _instance->can_instance->tx_buff[0] = stw_absulote_position_control;
    _instance->can_instance->tx_buff[1] = (uint8_t)(_position & 0xFF);
    _instance->can_instance->tx_buff[2] = (uint8_t)(_position >> 8 & 0xFF);
    _instance->can_instance->tx_buff[3] = (uint8_t)(_position >> 16 & 0xFF);
    _instance->can_instance->tx_buff[4] = (uint8_t)(_position >> 24 & 0xFF);
    if (_instance->can_instance->Transmit(_instance->can_instance) != CAN_TRANSMIT_SUCCESS)
        return false;
    return true;
}

static bool SpeedControl(SitaiweiInstance_s *_instance, int32_t _speed)
{
    _instance->can_instance->txconfig.DLC = 0x05;
    memset(_instance->can_instance->tx_buff, 0, 8);
    _instance->can_instance->tx_buff[0] = stw_speed_control;
    _instance->can_instance->tx_buff[1] = (uint8_t)(_speed & 0xFF);
    _instance->can_instance->tx_buff[2] = (uint8_t)(_speed >> 8 & 0xFF);
    _instance->can_instance->tx_buff[3] = (uint8_t)(_speed >> 16 & 0xFF);
    _instance->can_instance->tx_buff[4] = (uint8_t)(_speed >> 24 & 0xFF);
    if (_instance->can_instance->Transmit(_instance->can_instance) != CAN_TRANSMIT_SUCCESS)
        return false;
    return true;
}

static bool QcurrentControl(SitaiweiInstance_s *_instance, int32_t _current)
{
    _instance->can_instance->txconfig.DLC = 0x05;
    memset(_instance->can_instance->tx_buff, 0, 8);
    _instance->can_instance->tx_buff[0] = stw_Qcurrent_control;
    _instance->can_instance->tx_buff[1] = (uint8_t)(_current & 0xFF);
    _instance->can_instance->tx_buff[2] = (uint8_t)(_current >> 8 & 0xFF);
    _instance->can_instance->tx_buff[3] = (uint8_t)(_current >> 16 & 0xFF);
    _instance->can_instance->tx_buff[4] = (uint8_t)(_current >> 24 & 0xFF);
    if (_instance->can_instance->Transmit(_instance->can_instance) != CAN_TRANSMIT_SUCCESS)
        return false;
    return true;
}



bool ReadPosition(SitaiweiInstance_s *_instance)
{
    _instance->can_instance->txconfig.DLC = 0x01;
    memset(_instance->can_instance->tx_buff, 0, 8);
    _instance->can_instance->tx_buff[0] = stw_read_realtime_position;
    if (_instance->can_instance->Transmit(_instance->can_instance) != CAN_TRANSMIT_SUCCESS)
        return false;
    return true;
}
bool ReadState(SitaiweiInstance_s *_instance)
{
    _instance->can_instance->txconfig.DLC = 0x01;
    memset(_instance->can_instance->tx_buff, 0, 8);
    _instance->can_instance->tx_buff[0] = stw_read_realtime_state;
    if (_instance->can_instance->Transmit(_instance->can_instance) != CAN_TRANSMIT_SUCCESS)
        return false;
    return true;
}
bool CloseOutput(SitaiweiInstance_s *_instance)
{
    _instance->can_instance->txconfig.DLC = 0x01;
    memset(_instance->can_instance->tx_buff, 0, 8);
    _instance->can_instance->tx_buff[0] = stw_turnoff_output;
    if (_instance->can_instance->Transmit(_instance->can_instance) != CAN_TRANSMIT_SUCCESS)
        return false;
    return true;
}

bool OutputSwitchControl(SitaiweiInstance_s *_instance, bool _switch)
{
    _instance->can_instance->txconfig.DLC = 0x02;
    memset(_instance->can_instance->tx_buff, 0, 8);
    _instance->can_instance->tx_buff[0] = stw_output_switch_control;
    _instance->can_instance->tx_buff[1] = _switch;
    if (_instance->can_instance->Transmit(_instance->can_instance) != CAN_TRANSMIT_SUCCESS)
        return false;
    return true;
}

bool ReadVersion(SitaiweiInstance_s *_instance)
{

    _instance->can_instance->txconfig.DLC = 0x01;
    memset(_instance->can_instance->tx_buff, 0, 8);
    _instance->can_instance->tx_buff[0] = stw_read_version;
    if (_instance->can_instance->Transmit(_instance->can_instance) != CAN_TRANSMIT_SUCCESS)
        return false;
    return true;
}

SitaiweiInstance_s *SiTaiWeiMotorRegitster(SitaiweiConfig_t *_config)
{
    for (uint8_t i = 0; i < SitaiweiInstanceCount; i++) {
        if (SitaiweiInstanceList[i]->can_instance->hcan_id->Instance == _config->can_config.hcan_id->Instance &&
            SitaiweiInstanceList[i]->can_instance->master_id == _config->can_config.master_id)
            return SitaiweiInstanceList[i];
    }
    SitaiweiInstance_s *instance = (SitaiweiInstance_s *)malloc(sizeof(SitaiweiInstance_s));
    CANInitconfig_t _can_config  = {
         .hcan_id          = _config->can_config.hcan_id,
         .master_id        = _config->can_config.master_id,
         .receiver_id      = _config->can_config.receiver_id,
         .superclass       = instance,
         .CallbackFunction = CallbackFunction};
    CANInstance_s *can_insance = CANRegitster(&_can_config);
    if (instance == NULL || _config == NULL || can_insance == NULL || SitaiweiInstanceCount >= SITAIWEI_MAX_INSTANCE) {
        free(instance);
        free(can_insance);
        return NULL;
    }
    memset(instance, 0, sizeof(SitaiweiInstance_s));
    instance->can_instance                        = can_insance;
    instance->control_mode                        = _config->control_mode;
    instance->SetTorque                           = QcurrentControl;
    instance->SetSpeed                            = SpeedControl;
    instance->SetPosition                         = AbsulutePositionControl;
    SitaiweiInstanceList[SitaiweiInstanceCount++] = instance;
    return instance;
}