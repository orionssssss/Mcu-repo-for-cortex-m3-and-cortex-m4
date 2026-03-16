#ifndef MODBUS_SLAVE_H__
#define MODBUS_SLAVE_H__


void InitTouchScreen(void);
void DataRecTimeProc(void);
void SaveDataWord(unsigned short data,unsigned short addr);
void SaveDataBit(unsigned char data,unsigned short addr);
void SaveDataWords(unsigned short addr,unsigned short counts);
void beginSend(void);
void ModbusDealData(void);
void readCoil(void);
void readRegisters(void);
void forceSingleCoil(void);
void presetSingleRegister(void);
void presetMultipleRegisters(void);
void forceMultipleCoils(void);
unsigned short getRegisterVal(unsigned short addr,unsigned short *tempData);
unsigned short setRegisterVal(unsigned short addr,unsigned short tempData);
unsigned short getCoilVal(unsigned short addr,unsigned char *tempData);
unsigned short setCoilVal(unsigned short addr,unsigned char tempData);



#endif


