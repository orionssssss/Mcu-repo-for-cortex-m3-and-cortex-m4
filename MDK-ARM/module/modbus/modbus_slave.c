#include "modbus_slave.h"
#include "ParamService.h"
#include "CRCDataCale.h"
#include "bsp_log.h"
#include "cmsis_os.h"
#include "bsp_uart.h"
//#define ADDRDEC
//字地址 0 - 255 (只取低8位)
//位地址 0 - 255 (只取低8位)
 

unsigned char localAddr = 1;	//单片机控制板的地址
						  //modbus 通信位2048个
 unsigned short	sendCount;	
 unsigned short	receCount;
 unsigned short	sendPosi;
 unsigned char	sendBuf[256],receBuf[256];
 unsigned char	checkoutError;	// ==2 偶校验错 
 unsigned char	receTimeOut;
 unsigned int dwTickCount;
// 置位写EEPROM内的数据标识

static unsigned char ComReachTime=0;      //距离上次接收数据时长
UARTInstance_s * uart_instance;
void ModbusRecieveOneData(char data)
{
	    receTimeOut = 3;    //通讯超时值
			receBuf[receCount] = data;
	    receCount++;          //接收地址偏移寄存器加1
			receCount &= 0xff;    //最多一次只能接收16个字节	
}
void Uart3ReadOneDataDeal1PC1(unsigned char data)
{
	ModbusRecieveOneData(data);
}
void Modbus_Process(UARTInstance_s *instance)
{	
	for(int i=0;i<instance->rx_size;i++)
	{
		Uart3ReadOneDataDeal1PC1(instance->rx_buff[i]);//数据处理缓存区
	}
}
void InitTouchScreen(void)
{
		UARTInitConfig_t uart3 = {
			.uart_id = &huart3,
			.RxCallbackFunction = Modbus_Process,
			.TxCallbackFunction = 0
		};
		uart_instance = UARTRegister(&uart3);
		uart_instance->setmode(uart_instance,UART_TRANSMIT_IT,UART_RECEIVE_IT);
		uart_instance->Receive(uart_instance,uart_instance->rx_buff,1,0);
}

//10ms定时处理，清空接收区
void DataRecTimeProc(void)
{
	if(ComReachTime<100)ComReachTime++;
   if(receTimeOut>0)
    {
        receTimeOut--;
        if(receTimeOut==0 && receCount>0)   //判断通讯接收是否超时
        {
            receCount = 0;      //将接收地址偏移寄存器清零
			checkoutError = 0;
        }
   }
}
// void SaveDataWord(unsigned short data,unsigned short addr)
// {
// 	unsigned short addrSet,addrData,addrEeprom;
// 	if(addr<1024)
// 	{
// 		addrData=addr;
// 		addrSet=addr*2;
// 	}
// }

// void SaveDataBit(unsigned char data,unsigned short addr)
// {	
// 	unsigned short addrSet;
// 	if(addr>1000)
// 		return;

// }

//开始发送
void beginSend(void)
{
	uart_instance->Transmit(uart_instance, sendBuf, sendCount, 0);
	sendCount=0;

}//void beginSend(void)


//读线圈状态
void readCoil(void)
{
	unsigned short addr;
	unsigned short tempAddr;
	unsigned short byteCount;
	unsigned short bitCount;
	unsigned short crcData;
	unsigned char position;
	unsigned char i,k;
	unsigned char tempData=0;
	unsigned char  exit = 0;
	

	addr =(receBuf[2]<<8)+receBuf[3];
	tempAddr = addr;
  #ifdef ADDRDEC
	tempAddr-=1;
	#endif
	bitCount = (receBuf[4]<<8)+receBuf[5];

	byteCount = bitCount / 8;					//字节个数
	if(bitCount%8 != 0)
		byteCount++;
									
	for(k=0;k<byteCount;k++)
	{//字节位置
		position = k + 3;
		sendBuf[position] = 0;
		for(i=0;i<8;i++)
		{
			getCoilVal(tempAddr,&tempData);
			
			sendBuf[position] |= tempData << i;
			tempAddr++;
			if(tempAddr >= addr+bitCount)
			{	//读完
				exit = 1;
				break;
			}	
		}
		if(exit == 1)
			break;
	}
	
	sendBuf[0] = localAddr;
	sendBuf[1] = 0x01;	
	sendBuf[2] = byteCount;
	byteCount += 3;
	crcData = crc16(sendBuf,byteCount);	
	sendBuf[byteCount] = crcData >> 8;
	byteCount++;
	sendBuf[byteCount] = crcData & 0xff;
	sendCount = byteCount + 1;
	
	beginSend();	
}//void readCoil(void) 

//读寄存器
void readRegisters(void)
{
	unsigned short addr;
	unsigned short tempAddr;
	unsigned short crcData;
	unsigned short readCount;
	unsigned short byteCount;
	
	unsigned short i;
	unsigned short tempData = 0;	
	
	addr = (receBuf[2]<<8) + receBuf[3];
	tempAddr = addr & 0xffff;	
  #ifdef ADDRDEC
	tempAddr-=1;
	#endif
	readCount = (receBuf[4]<<8) + receBuf[5];	//要读的个数

	byteCount = readCount * 2;
	
	for(i=0;i<byteCount;i+=2,tempAddr++)
	{
		getRegisterVal(tempAddr,&tempData);				
		sendBuf[i+3] = tempData >> 8;				   		
		sendBuf[i+4] = tempData & 0xff;			
	}
	
	sendBuf[0] = localAddr;
	sendBuf[1] = 3;
	sendBuf[2] = byteCount;
	byteCount += 3;
	crcData = crc16(sendBuf,byteCount);
	sendBuf[byteCount] = crcData >> 8;
	byteCount++;
	sendBuf[byteCount] = crcData & 0xff;
	
	sendCount = byteCount + 1;
	beginSend();
}//void readRegisters(void)


//强制单个线圈
void forceSingleCoil(void)
{
	unsigned short addr;
	unsigned short tempAddr;
	unsigned char tempData=0;
	unsigned short  onOff;
	unsigned char i;
	
	addr = (receBuf[2]<<8) + receBuf[3];	
	tempAddr = addr & 0xffff;
	
	#ifdef ADDRDEC
	tempAddr-=1;
	#endif
	
	onOff = (receBuf[4]<<8) + receBuf[5];	
	
	if(onOff == 0xff00)
	{	//设为ON
		tempData = 1;
	} 
	else if(onOff == 0x0000)
	{	//设为OFF
		tempData = 0;
	}

	setCoilVal(tempAddr,tempData);	
	
	for(i=0;i<receCount;i++)
	{
		sendBuf[i] = receBuf[i];
	}
	sendCount = receCount;
	beginSend();	
}//void forceSingleCoil(void)
void forceMultipleCoils(void)
{
	unsigned short addr;
	unsigned short tempAddr;
	unsigned short SetCount;
	unsigned short SetByte;
	unsigned short crcData;
	unsigned char i,j;
	
	addr = (receBuf[2]<<8) + receBuf[3];	
	tempAddr = addr & 0xffff;
	
	#ifdef ADDRDEC
	tempAddr-=1;
	#endif
	
	SetCount= (receBuf[4]<<8) + receBuf[5];
	
	SetByte=receBuf[6];

	for(i=0;i<SetByte;i++)
	{
		for(j=0;j<8&&((i*8+j)<SetCount);j++)
		{
			if((receBuf[7+i]&(1<<j))!=0)
				setCoilVal(tempAddr+i*8+j,1);
			else
				setCoilVal(tempAddr+i*8+j,0);
		}
	}
	for(i=0;i<6;i++)
	{
		sendBuf[i] = receBuf[i];
	}
	crcData = crc16(sendBuf,6);
	sendBuf[6] = crcData >> 8;
	sendBuf[7] = crcData & 0xff;
	sendCount = 8;
	beginSend();
}

//设置多个寄存器
void presetMultipleRegisters(void)
{
	unsigned short addr;
	unsigned short tempAddr;
	unsigned short setCount;
	unsigned short crcData;
	unsigned short tempData;
	unsigned char i;
	
	addr = (receBuf[2]<<8) + receBuf[3];
	tempAddr = addr & 0xffff;

	#ifdef ADDRDEC
	tempAddr-=1;
	#endif
	
	setCount = (receBuf[4]<<8) + receBuf[5];
	
	for(i=0;i<setCount;i++,tempAddr++)
	{
		tempData = (receBuf[i*2+7]<<8) + receBuf[i*2+8];
	
		setRegisterVal(tempAddr,tempData);			
	}
	
	sendBuf[0] = localAddr;
	sendBuf[1] = 16;
	sendBuf[2] = addr >> 8;
	sendBuf[3] = addr & 0xff;
	sendBuf[4] = setCount >> 8;	  
	sendBuf[5] = setCount & 0xff;
	crcData = crc16(sendBuf,6);
	sendBuf[6] = crcData >> 8;
	sendBuf[7] = crcData & 0xff;
	sendCount = 8;
	beginSend();	
}//void presetMultipleRegisters(void)

void presetSingleRegister(void)
{
	unsigned short addr;
	unsigned short tempAddr;
	unsigned short setCount=0;
	unsigned short crcData;
	unsigned short tempData;
	
	addr = (receBuf[2]<<8) + receBuf[3];
	tempAddr = addr & 0xffff;

	#ifdef ADDRDEC
	tempAddr-=1;
	#endif
	
	tempData = (receBuf[4]<<8) + receBuf[5];
	setRegisterVal(tempAddr,tempData);			
	
	sendBuf[0] = localAddr;
	sendBuf[1] = 16;
	sendBuf[2] = addr >> 8;
	sendBuf[3] = addr & 0xff;
	sendBuf[4] = setCount >> 8;	  
	sendBuf[5] = setCount & 0xff;
	crcData = crc16(sendBuf,6);
	sendBuf[6] = crcData >> 8;
	sendBuf[7] = crcData & 0xff;
	sendCount = 8;
	beginSend();
}

//检查uart0数据
void ModbusDealData(void)
{
	unsigned short crcData;
	unsigned short tempData;
	//USART_SendData(USART3,sendBuf[0]);
	if(receCount > 4)
	{
		switch(receBuf[1])
		{
			case 1://读取线圈状态(读取点 16位以内)
			case 2://读取保持寄存器(一个或多个)
			case 3://读取保持寄存器(一个或多个)
			case 5://强制单个线圈
			case 6://设置单个寄存器
					if(receCount >= 8)
					{//接收完成一组数据
						//应该关闭接收中断
						
						if(receBuf[0]==localAddr && checkoutError==0)
						{
							crcData = crc16(receBuf,6);
							if(crcData == receBuf[7]+(receBuf[6]<<8))
							{//校验正确
								if(receBuf[1] == 1||receBuf[1]==2)
								{//读取线圈状态(读取点 16位以内)
									readCoil();								
								}
								else if(receBuf[1] == 3)
								{//读取保持寄存器(一个或多个)
									readRegisters();
								}
								else if(receBuf[1] == 5)
								{//强制单个线圈
									forceSingleCoil();								
								}
								else if(receBuf[1] == 6)
								{
									 presetSingleRegister();								
								}

							}
						}						
						receCount = 0;	
						checkoutError = 0;
					}
					break;
			
			case 15://设置多个线圈
					tempData = receBuf[6]; 
					tempData += 9;	//数据个数
					if(receCount >= tempData)
					{
						if(receBuf[0]==localAddr && checkoutError==0)
						{
							crcData = crc16(receBuf,tempData-2);
							if(crcData == (receBuf[tempData-2]<<8)+ receBuf[tempData-1])
							{
								forceMultipleCoils();			
							}
						}	
						receCount = 0;
						checkoutError = 0;
					}
					break;
			
			case 16://设置多个寄存器
					tempData = (receBuf[4]<<8) + receBuf[5];
					tempData = tempData * 2;	//数据个数
					tempData += 9;
					if(receCount >= tempData)
					{
						if(receBuf[0]==localAddr && checkoutError==0)
						{
							crcData = crc16(receBuf,tempData-2);
							if(crcData == (receBuf[tempData-2]<<8)+ receBuf[tempData-1])
							{
							 	receCount=	tempData;
								presetMultipleRegisters();			
							}
						}	
						receCount = 0;
						checkoutError = 0;
					}
					break;
								
			default:
					break;			
		}
	}
}//void checkComm0(void)

//取线圈状态 返回0表示成功
unsigned short getCoilVal(unsigned short addr,unsigned char *tempData)
{
	unsigned short result = 0;
	unsigned short tempAddr;
	ParamData_t *param = GetParameterGroup();
	tempAddr = addr;
	if(tempAddr<2048)
		*tempData = param->byte[tempAddr];
	if(*tempData!=0)*tempData=1;
	return result;
}//unsigned short getCoilVal(unsigned short addr,unsigned short *data)


//设定线圈状态 返回0表示成功
unsigned short setCoilVal(unsigned short addr,unsigned char tempData)
{
	unsigned short result = 0;
	unsigned short tempAddr;
	ParamData_t *param = GetParameterGroup();
	tempAddr = addr;
	if(tempAddr<1024)
	{	
		param->byte[tempAddr]=tempData;
	}	
	else if(tempAddr<2000)
	{
		// DATABIT.Bit[tempAddr]=tempData;
	}
	return result;
}//unsigned short setCoilVal(unsigned short addr,unsigned short data)

//取寄存器值 返回0表示成功
unsigned short getRegisterVal(unsigned short addr,unsigned short *tempData)
{
	unsigned short result = 0;
	unsigned short tempAddr;
	ParamData_t *param = GetParameterGroup();
	tempAddr = addr;
 if(tempAddr<=1024)
	{
		*tempData=param->hword[tempAddr];

	}
	return result;
}//unsigned short getRegisterVal(unsigned short addr,unsigned short &data)

//设置寄存器值 返回0表示成功
unsigned short setRegisterVal(unsigned short addr,unsigned short tempData)
{
	unsigned short result = 0;
	unsigned short tempAddr;
	ParamData_t *param = GetParameterGroup();
	tempAddr = addr;
   	if(tempAddr>=PARAM_HWORD_START_INDEX){
		param->hword[addr] = tempData;
	}
	return result;
}//unsigned char setRegisterVal(unsigned short addr,unsigned short data)



