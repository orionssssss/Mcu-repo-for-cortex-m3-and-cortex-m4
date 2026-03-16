#include "bsp_uart.h"
#include "string.h"
#include "VOFA.h"

static unsigned char tail[4] = {0x00,0x00,0x80,0x7f};
static uint8_t DataSend4[20] = {0};
static uint8_t DataSend8[36] = {0};
static uint8_t DataSend16[68] = {0};

UARTInstance_s * uart5_instance = NULL;
uint8_t buff[256];
void VOFA_Init(void)
{
	UARTInitConfig_t uartinit = {0};
	uartinit.uart_id = &huart1;
	uartinit.superclass = NULL;
	uartinit.TxCallbackFunction = NULL;
	uart5_instance = UARTRegister(&uartinit);
	uart5_instance->setmode(uart5_instance, UART_TRANSMIT_DMA, UART_RECEIVE_IT);
}

void VOFA_SEND_4(float data1, float data2, float data3, float data4)
{
	float data_temp[4];
	data_temp[0] = data1;
	data_temp[1] = data2;
	data_temp[2] = data3;
	data_temp[3] = data4;
	memcpy(DataSend4, data_temp, sizeof(float)*4);
	memcpy(&DataSend4[16], tail, 4);
	uart5_instance->Transmit(uart5_instance, DataSend4, 20, NULL);

}

void VOFA_SEND_8(float data1, float data2, float data3, float data4, float data5, float data6, float data7, float data8)
{
	float data_temp[8];
	data_temp[0] = data1;
	data_temp[1] = data2;
	data_temp[2] = data3;
	data_temp[3] = data4;
	data_temp[4] = data5;
	data_temp[5] = data6;
	data_temp[6] = data7;
	data_temp[7] = data8;
	memcpy(DataSend8, data_temp, sizeof(float)*8);
	memcpy(&DataSend8[32], tail, 4);
	uart5_instance->Transmit(uart5_instance, DataSend8, 36, NULL);
	// HAL_UART_Transmit_DMA(&huart1, DataSend8, 36);
	// HAL_UART_Transmit(&huart1, DataSend8, 36, 100);
}

void VOFA_SEND_16(float data1, float data2, float data3, float data4, float data5, float data6, float data7, float data8,
				  float data9, float data10, float data11, float data12, float data13, float data14, float data15, float data16)
{
	float data_temp[16];
	data_temp[0] = data1;
	data_temp[1] = data2;
	data_temp[2] = data3;
	data_temp[3] = data4;
	data_temp[4] = data5;
	data_temp[5] = data6;
	data_temp[6] = data7;
	data_temp[7] = data8;
	data_temp[8] = data9;
	data_temp[9] = data10;
	data_temp[10] = data11;
	data_temp[11] = data12;
	data_temp[12] = data13;
	data_temp[13] = data14;
	data_temp[14] = data15;
	data_temp[15] = data16;
	memcpy(DataSend16, data_temp, sizeof(float)*16);
	memcpy(&DataSend16[64], tail, 4);
	uart5_instance->Transmit(uart5_instance, DataSend16, 68, NULL);
}
