#include "hjpushbeam.h"
#include "modbus.h"
#include "bsp_log.h"
#include "string.h"
#include "cmsis_os2.h"


static uint8_t PushbeamInstanceCount;
static HJPushbeamInstance_t *pushbeam_instance_list[PUSHBEAM_MAX_INSTANCE] = {0};

void CheckErrorCallback(ModbusInstance_t *_instance)
{
	_instance->rx_buff;
}
static void PushbeamSetPosition(HJPushbeamInstance_t *_instance, uint16_t position, uint16_t pushspeed)
{
	if(pushspeed != 0)
	{
		_instance->modbus_instance->write_one_reg(_instance->modbus_instance, 2, &pushspeed);
	}	
	_instance->modbus_instance->write_one_reg(_instance->modbus_instance, 1, &position);
}

static void PushbeamSetRunParam(HJPushbeamInstance_t *_instance, uint16_t start_time, uint16_t stop_time, uint16_t pre_stop_position)
{

	_instance->modbus_instance->write_one_reg(_instance->modbus_instance, 8, &start_time);
	osDelay(1);
	_instance->modbus_instance->write_one_reg(_instance->modbus_instance, 9, &stop_time);
	osDelay(1);
	_instance->modbus_instance->write_one_reg(_instance->modbus_instance, 10, &pre_stop_position);
	osDelay(1);
}

static uint8_t PushbeamGetData(HJPushbeamInstance_t *_instance)
{
	MODBUS_STATE communicate_state;
    communicate_state = _instance->modbus_instance->read_multi_reg(_instance->modbus_instance, 1, 10, _instance->pushbeam_t.hword);
	return !communicate_state;
}

HJPushbeamInstance_t *HJPushbeamRegister(HJPushbeamConfig_t * config)
{
	for (int i = 0; i < PushbeamInstanceCount; i++) {
        if (strcmp(pushbeam_instance_list[i]->name, config->name) == 1) {
            return pushbeam_instance_list[i];
        }
    }
    HJPushbeamInstance_t *_instance = (HJPushbeamInstance_t*)malloc(sizeof(HJPushbeamInstance_t));
	if(_instance == NULL || config == NULL || PushbeamInstanceCount >= PUSHBEAM_MAX_INSTANCE)//内存分配失败//参数错误//实例数超限
	{
		free(_instance);
		return NULL;
	}
	memset(_instance, 0, sizeof(HJPushbeamInstance_t));
	//注册uart	
	UARTInitConfig_t _uart_config ={
		.uart_id = config->modbus_config.uart_config.uart_id,
		.superclass = (uint32_t*)_instance,
		.TxCallbackFunction = NULL,
		.RxCallbackFunction = NULL
	};
	char name_temp[20] = "modbus_";
	char *name = 0;
	name = strncat(name_temp, config->name, sizeof(name_temp) - strlen(name_temp) - 1);
    ModbusConfig_t _modbus_config = {
        .uart_config = _uart_config,
        .slave_addr = config->modbus_config.slave_addr,
        .superclass = (uint32_t)_instance,
        .CallbackFunction = NULL
    };
	strcpy(_modbus_config.name, name);
	strcpy(_instance->name, name);
    _instance->modbus_instance = ModbusRegister(&_modbus_config);
	LOG("Pushbeam instance registered: %s", _instance->name);
	//初始化pushbeam对象
	_instance->CallbackFunction = config->CallbackFunction;
	_instance->setposition = PushbeamSetPosition;
	_instance->getdata = PushbeamGetData;
	_instance->SetRunParam = PushbeamSetRunParam;
	//记录入注册表
	pushbeam_instance_list[PushbeamInstanceCount++] = _instance;
	return _instance;

}