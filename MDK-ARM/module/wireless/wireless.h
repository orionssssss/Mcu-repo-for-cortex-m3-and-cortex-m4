#ifndef _WIRELESS_H__
#define _WIRELESS_H__

#include "stdint.h"
#include "bsp_uart.h"
#include "bsp_buffer.h"

#define WIRELESS_MAX_INSTANCE 1

typedef struct WirelessInstance WirelessInstance_t;
typedef struct WirelessInstance{
	UARTInstance_s* uart_instance;
	Buffer_t *rx_buff;
	uint16_t offline_cnt;
    struct wireless_communicate_flag
	{
		uint8_t tx_finish_flag : 1;
		uint8_t rx_finish_flag : 1;
		uint8_t offline_flag : 1;
	} communicate_flag;
	void (*transmit)(WirelessInstance_t *_instance, uint8_t *data, uint16_t length);
	void (*receive)(WirelessInstance_t *_instance, uint16_t length);
	void (*CallbackFunction)(WirelessInstance_t * _instance);

}WirelessInstance_t;

typedef struct{
	UARTInitConfig_t* uart_config;
	void (*CallbackFunction)(WirelessInstance_t * _instance);
}WirelessConfig_t;
/**
 * @brief register a wireless module
 * 
 * @param config regitster config
 * @return WirelessInstance_t* 
 */
WirelessInstance_t *WirelessRegister(WirelessConfig_t * config);

#endif