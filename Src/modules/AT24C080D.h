#ifndef __AT24C080D_H
#define	__AT24C080D_H


#include "stm32f10x.h"

#define LIGHE_TIME_ADD 0X00
#define LIGHE_POWER_ADD 0X08
#define EEPROM_Block0_ADDRESS 0xA0   /* E2 = 0 */

void at24c08_init(void);
void i2c_ee_buffer_write(u8* p_buffer, u8 write_addr, u16 num_byte_write);
void i2c_ee_page_write(u8* p_buffer, u8 write_addr, u8 num_byte_write);
void I2C_EE_BufferRead(u8* p_buffer, u8 read_addr, u16 num_byte_read);
void i2c_ee_wait_eeprom_stand_state(void);     

void eepom_test(void);
void IIC_Byte_Write( u8 pBuffer, u8 Word_Address);
void IIC_Byte_Read( u8* pRead, u8 Word_Address);
void write_eepom_data(uint32_t data, uint32_t add);
uint32_t read_eepom_data(uint32_t add);
#endif

