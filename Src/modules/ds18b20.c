/******************************************************************
 * 文件名  ：ds18b20.c
 * 描述    ：ds18b20应用函数库        
 * 硬件连接：----------------------------------------------------
 *          |                                                    |
 *          | PB9 - 信号口
 *          |  
            |                                                    |
 *           ----------------------------------------------------
 * 库版本  ：ST3.5.0
**********************************************************************************/
#include "ds18b20.h"
#include "delay.h"
static __IO uint8_t s_temp_integer = 0;   //温度整数部分
static __IO uint8_t s_temp_decimal = 0;   //温度小数部分
static __IO float s_temp_value = 0;
/*
 * 函数名：ds18b20_gpio_config
 * 描述  ：
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void ds18b20_gpio_config(void)
{                
    GPIO_InitTypeDef GPIO_InitStructure;        
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);                                                                                                                                  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);        
}

/*
 * 函数名：ds18b20_reset
 * 描述  ：
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void ds18b20_reset(void)
{
    DS18B20_H;
    delay_us(20);
    DS18B20_L;
    delay_us(550);
    DS18B20_H;
    while(GPIO_ReadInputDataBit(GPIO_DS18B20_PORT,GPIO_DS18B20_Pin));
    delay_us(500);
    DS18B20_H;
}
/*
 * 函数名：ds18b20_init
 * 描述  ：
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void ds18b20_init(void)
{
    ds18b20_gpio_config();
    ds18b20_reset();
    ds18b20_write_byte(0xCC);
    ds18b20_write_byte(0x4E);
    ds18b20_write_byte(0x64);        //高温报警值
    ds18b20_write_byte(0x8A);        //低温报警值
    ds18b20_write_byte(0x7F);        //精度 9 位 0.5度 （31 63 95 127(0.0625)）
}
/*
 * 函数名：ds18b20_write_bit0
 * 描述  ：
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void ds18b20_write_bit0(void)
{
    DS18B20_H;
    delay_us(1);             
    DS18B20_L;
    delay_us(55);
    DS18B20_H;
    delay_us(1);
}
/*
 * 函数名：ds18b20_write_bit1
 * 描述  ：
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void ds18b20_write_bit1(void)
{
    DS18B20_H;
    delay_us(1); 
    DS18B20_L;
    delay_us(5);
    DS18B20_H;
    delay_us(5); 
    delay_us(50);
}
/*
 * 函数名：ds18b20_read_bit
 * 描述  ：
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
uint8_t ds18b20_read_bit(void)
{
    uint8_t bdata;
    DS18B20_H;
    delay_us(1);
    DS18B20_L;
    delay_us(4);
    DS18B20_H;
    delay_us(8);
    bdata=GPIO_ReadInputDataBit(GPIO_DS18B20_PORT,GPIO_DS18B20_Pin);
    delay_us(60);
    delay_us(2);
    return bdata;
}
/*
 * 函数名：ds18b20_write_byte
 * 描述  ：
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void ds18b20_write_byte(uint8_t udata)
{
    uint8_t i;
    for (i = 0; i < 8; i++){
        if (udata & 0x01){
            ds18b20_write_bit1();
				}
        else{
            ds18b20_write_bit0();
				}
        udata = udata >> 1;
    }
    delay_us(10);
}
/*
 * 函数名：ds18b20_read_byte
 * 描述  ：
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
uint8_t ds18b20_read_byte(void)
{
    uint8_t i, udata, j;
    udata = 0;
    for (i = 0; i < 8; i++){
        udata = udata >> 1;
        j = ds18b20_read_bit();
        if (j == 0x01){
            udata |= 0x80;
        }
        else{
            udata |= 0x00;
        }
        delay_us(2);
    }
    return udata;
}
/*
 * 函数名：ds18b20_thread
 * 描述  ：0-7 小数位 8-14 整数位 15 符号位   进程 750ms
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void ds18b20_thread(void)
{
    static __IO uint8_t step_i = 0;
    uint8_t temp_integer = 0;   //温度整数部分
    uint8_t temp_decimal = 0;   //温度小数部分
    switch (step_i){
		    case 0:
  	        ds18b20_reset(); 
            ds18b20_write_byte(0xCC);
            ds18b20_write_byte(0x44);
            step_i = 1;
            break;
        case 1:
            ds18b20_reset();
            ds18b20_write_byte(0xCC);
            ds18b20_write_byte(0xBE);
            temp_decimal = ds18b20_read_byte();
            temp_integer = ds18b20_read_byte();
            ds18b20_read_byte();
            ds18b20_read_byte();
            ds18b20_read_byte();
            ds18b20_read_byte();
            ds18b20_read_byte();
            ds18b20_read_byte();
            ds18b20_read_byte();
            s_temp_value = transform_ds18b20_data(temp_integer ,temp_decimal);
			      step_i = 0;
            break;
				default:
					  step_i = 0;
            break;
    }				

       
}

/*
 * 函数名：updata_ds18b20_data
 * 描述  ：
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
float transform_ds18b20_data(uint8_t integer_value ,uint8_t decimal_value)
{
    uint16_t temp = 0;
    float temp_f = 0.0f;
    temp = integer_value * 256 + decimal_value;
    if (temp > 63488){
        temp = ~temp + 1 ;
        temp_f = -1 * temp * DS18B20_12;		
		}
		else{
        temp_f = temp * DS18B20_12;
    }
    return temp_f;
}
/*
 * 函数名：updata_ds18b20_data
 * 描述  ：
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void updata_ds18b20_data(float *temp)
{
    *temp = s_temp_value;
}