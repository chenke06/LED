#ifndef __ds18b20_H
#define	__ds18b20_H


#include "stm32f10x.h"

#define RCC_DS18B20              RCC_APB2Periph_GPIOA
#define GPIO_DS18B20_PORT        GPIOA
#define GPIO_DS18B20_Pin         GPIO_Pin_5

#define DS18B20_H    GPIO_SetBits(GPIOA,GPIO_Pin_5)
#define DS18B20_L    GPIO_ResetBits(GPIOA,GPIO_Pin_5)

#define DS18B20_9 0.5f
#define DS18B20_10 0.25f
#define DS18B20_11 0.125f
#define DS18B20_12 0.0625f



void ds18b20_gpio_config(void);
void ds18b20_reset(void);
void ds18b20_init(void);
void ds18b20_write_bit0(void);
void ds18b20_write_bit1(void);
uint8_t ds18b20_read_bit(void);
void ds18b20_write_byte(uint8_t udata);
uint8_t ds18b20_read_byte(void);
void ds18b20_thread(void);
float transform_ds18b20_data(uint8_t integer_value ,uint8_t decimal_value);
void updata_ds18b20_data(float *temp);
#endif 

