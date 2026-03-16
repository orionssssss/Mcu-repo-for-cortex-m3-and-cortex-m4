#include "wireless.h"
#include "stdlib.h"
#include "string.h"

uint8_t WirelessInstanceCount;
WirelessInstance_t *wireless_instance_list[WIRELESS_MAX_INSTANCE] = {0};
static void TxCallback(UARTInstance_s * _instance)
{
	WirelessInstance_t *wireless_instance = (WirelessInstance_t *)_instance->superclass;
		if(!wireless_instance->communicate_flag.tx_finish_flag)
			wireless_instance->communicate_flag.tx_finish_flag = 1;
}

static void RxCallback(UARTInstance_s * _instance)
{
		WirelessInstance_t *wireless_instance = (WirelessInstance_t *)_instance->superclass;
		Buffer_Write(wireless_instance->rx_buff, _instance->rx_buff, _instance->rx_size);//½«Êý¾Ý´æÈëÑ­»·buff
		if(wireless_instance != NULL && wireless_instance->CallbackFunction != NULL)
		{
			wireless_instance->CallbackFunction(wireless_instance);
		}
		wireless_instance->offline_cnt = 0;
		wireless_instance->communicate_flag.offline_flag = 0;
		wireless_instance->communicate_flag.rx_finish_flag = 1;
}

static void Transmit(WirelessInstance_t *_instance, uint8_t *data, uint16_t length)
{
	_instance->uart_instance->Transmit(_instance->uart_instance, data, length, 1000);
}

static void Receive(WirelessInstance_t *_instance, uint16_t length)
{
	_instance->uart_instance->Receive(_instance->uart_instance, _instance->uart_instance->rx_buff, length, 1000);
}

WirelessInstance_t *WirelessRegister(WirelessConfig_t * config)
{
    WirelessInstance_t *_instance = (WirelessInstance_t*)malloc(sizeof(WirelessInstance_t));
	if(_instance == NULL)//ÄÚ´æ·ÖÅäÊ§°Ü
		return NULL;
	memset(_instance, 0, sizeof(WirelessInstance_t));
	// _instance->superclass = config->superclass;
	//×¢²áuart	
	UARTInitConfig_t _uart_config ={
		.uart_id = config->uart_config->uart_id,
		.superclass = (uint32_t*)_instance,
		.TxCallbackFunction = TxCallback,
		.RxCallbackFunction = RxCallback
	};
	_instance->uart_instance = UARTRegister(&_uart_config);
	if(_instance->uart_instance == NULL)//´®¿Ú×¢²áÊ§°Ü
	{
		free(_instance);
		return NULL;
	}
	//×¢²áÑ­»·buffer
	BufferConfig_t _RxBuffer = {
		.type = CircularBuffer,
		.size = RECBUFLEN,
		.superclass = (uint32_t)_instance
	};
	Buffer_t *_rxbuff = Buffer_Init(&_RxBuffer);
	if(_rxbuff == NULL)//Buffer×¢²áÊ§°Ü
	{
		free(_instance->uart_instance);
		free(_instance);
		return NULL;
	}
	//³õÊ¼»¯wireless¶ÔÏó
	_instance->CallbackFunction = config->CallbackFunction;
	_instance->uart_instance->setmode(_instance->uart_instance, UART_TRANSMIT_IT, UART_RECEIVE_IT);
	_instance->rx_buff = _rxbuff;
	_instance->transmit = Transmit;
	_instance->receive = Receive;
	wireless_instance_list[WirelessInstanceCount++] = _instance;
	return _instance;

}