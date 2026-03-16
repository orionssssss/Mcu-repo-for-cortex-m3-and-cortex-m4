#include "eeprom.h"
#include "bsp_i2c.h"
/*E2PROMµÄÊµÀýÖ¸Õë*/
I2CInstance_s *Eeprom;


void EEPROM_Init(void)
{
    I2CInitConfig_t i2c_config = {
    .i2c_id = hi2c2, 
    .slave_addr = 0xA0, 
    .superclass = NULL, 
    .RxCallbackFunction = NULL, 
    .TxCallbackFunction = NULL
  };
  Eeprom = I2CRegister(&i2c_config);
}



void EEPROM_Write(uint16_t write_addr, uint8_t addr_bit_size, uint8_t *data, uint16_t length)
{
  switch(addr_bit_size)
  {
    case 16:
      Eeprom->MemWrite(Eeprom, write_addr, I2C_MEMADD_SIZE_16BIT, data, length, 1000);
      break;
    case 8:
    default:
      Eeprom->MemWrite(Eeprom, write_addr, I2C_MEMADD_SIZE_8BIT, data, length, 1000);
      break;
  }
    
}


uint8_t EEPROM_Read(uint16_t read_addr, uint8_t addr_bit_size, uint8_t *data, uint16_t length)
{
  uint8_t ret;
  switch(addr_bit_size)
  {
    case 16:
      ret = Eeprom->MemRead(Eeprom, read_addr, I2C_MEMADD_SIZE_16BIT, data, length, 1000);
      break;
    case 8:
    default:
      ret = Eeprom->MemRead(Eeprom, read_addr, I2C_MEMADD_SIZE_8BIT, data, length, 1000);
      break;
  }
  return ret;
  
}