#include "pushbeam.h"
#include "modbus.h"
#include "bsp_log.h"
#include "string.h"
#include "cmsis_os2.h"


uint8_t PushbeamInstanceCount;
KBSPushbeamInstance_t *pushbeam_instance_list[PUSHBEAM_MAX_INSTANCE] = {0};

static uint8_t PushbeamcClearError(KBSPushbeamInstance_t * _instance)
{
	static uint16_t pushbeam_initdata[2] = {0, 2};
    _instance->modbus_instance->write_one_reg(_instance->modbus_instance, 11, &pushbeam_initdata[0]);
	_instance->modbus_instance->write_one_reg(_instance->modbus_instance, 4, &pushbeam_initdata[1]);
	return _instance->pushbeam_t.data.init_run;
}

static uint8_t PushbeamEnable(KBSPushbeamInstance_t * _instance)
{
	static uint16_t pushbeam_initdata[2] = {1, 2};
	uint8_t init_cnt = 0;
	//等待初始化完成
	while((_instance->pushbeam_t.data.init == 0 || _instance->pushbeam_t.data.init_run == 0) && init_cnt < 10)
	{
		init_cnt++;
		_instance->getdata(_instance);
		_instance->modbus_instance->write_one_reg(_instance->modbus_instance, 10, &pushbeam_initdata[0]);
		_instance->modbus_instance->write_one_reg(_instance->modbus_instance, 4, &pushbeam_initdata[1]);
		osDelay(10);
	}
	return _instance->pushbeam_t.data.init;
}
static void PushbeamSetposition(KBSPushbeamInstance_t *_instance, uint16_t position, uint16_t pushspeed)
{
    _instance->modbus_instance->write_one_reg(_instance->modbus_instance, 3, &pushspeed);
	_instance->modbus_instance->write_one_reg(_instance->modbus_instance, 2, &position);
}

static void PushbeamSetLimitPosition(KBSPushbeamInstance_t *_instance, uint16_t limit_position)
{
    _instance->modbus_instance->write_one_reg(_instance->modbus_instance, 9, &limit_position);
}

static uint8_t PushbeamGetData(KBSPushbeamInstance_t *_instance)
{
	MODBUS_STATE communicate_state;
    communicate_state = _instance->modbus_instance->read_multi_reg(_instance->modbus_instance, 1, 11, _instance->pushbeam_t.hword);
	if(communicate_state == MODBUS_SUCCESS && (_instance->pushbeam_t.data.state == 2||_instance->pushbeam_t.data.state == 1))
	{
		PushbeamcClearError(_instance);
	}
	return !communicate_state;
}

KBSPushbeamInstance_t *KBSPushbeamRegister(KBSPushbeamConfig_t * config)
{
	for (int i = 0; i < PushbeamInstanceCount; i++) {
        if (strcmp(pushbeam_instance_list[i]->name, config->name) == 1) {
            return pushbeam_instance_list[i];
        }
    }
    KBSPushbeamInstance_t *_instance = (KBSPushbeamInstance_t*)malloc(sizeof(KBSPushbeamInstance_t));
	if(_instance == NULL || config == NULL || PushbeamInstanceCount >= PUSHBEAM_MAX_INSTANCE)//内存分配失败//参数错误//实例数超限
	{
		free(_instance);
		return NULL;
	}
	memset(_instance, 0, sizeof(KBSPushbeamInstance_t));
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
	_instance->setlimitposition = PushbeamSetLimitPosition;
	_instance->setposition = PushbeamSetposition;
	_instance->getdata = PushbeamGetData;
	_instance->enable = PushbeamEnable;
	pushbeam_instance_list[PushbeamInstanceCount++] = _instance;
	return _instance;

}