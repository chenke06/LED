/*******************************************************************************
 * 文件名  ：opt3001.c
 * 描述    ：opt3001应用函数库        
 * 库版本  ：ST3.5.0
**********************************************************************************/
#include "opt3001.h"
#include "myi2c1.h"
#include "delay.h"

static __IO uint16_t s_opt3001_data = 0;
/*
 * 函数名：init_opt3001
 * 描述  : 
 * 输入  : 
 * 输出  ：无
 * 调用  ：内部调用
 */
void init_opt3001()
{
    my_i2c1_init();
    opt3001_config();
}
/*
 * 函数名：write_opt3001_register
 * 描述  : 
 * 输入  : 
 * 输出  ：无
 * 调用  ：内部调用
 */
void write_opt3001_register(uint8_t cmd, uint16_t value)
{
    my_iic1_start();
    my_iic1_send_byte(OPT3001_ADDRESS);
	  my_iic1_wait_ack();
    my_iic1_send_byte(cmd);
    my_iic1_wait_ack();
    my_iic1_send_byte((uint8_t)(value >> 8));
	  my_iic1_wait_ack();
    my_iic1_send_byte((uint8_t)(value & 0x00FF));
	  my_iic1_wait_ack();
    my_iic1_stop();
	
}
/*
 * 函数名：read_opt3001_register
 * 描述  : 
 * 输入  : 
 * 输出  ：无
 * 调用  ：内部调用
 */
uint16_t read_opt3001_register(uint8_t cmd)
{
    uint8_t msb = 0;
    uint8_t lsb = 0;
    uint16_t data = 0;

    my_iic1_start();
    my_iic1_send_byte(OPT3001_ADDRESS);
    my_iic1_wait_ack();
	  my_iic1_send_byte(cmd);
    my_iic1_wait_ack();                                                                                         
	  my_iic1_stop();

      
    my_iic1_start();       
    my_iic1_send_byte(OPT3001_ADDRESS + 0x01);   
    my_iic1_wait_ack();

		msb = my_iic1_read_byte(1);
    lsb = my_iic1_read_byte(0);
	  my_iic1_stop();
		
    delay_ms(5);
		data = msb << 8;
		data |= lsb;
		
    return data;		
}

/*
 * 函数名：get_opt3001_id
 * 描述  : 
 * 输入  : 
 * 输出  ：0X5449
 * 调用  ：内部调用
 */
uint16_t get_opt3001_id()
{
    uint16_t id_num = 0;
    id_num = read_opt3001_register(0x7E);
    return id_num;
}

/*
 * 函数名：opt3001_config
 * 描述  : 12 - 15  RN[0,3]:0X1100 , full-scale mode
 * 输入  : 9 - 10 : M[0,1]:0X10, 连续转换模式
 * 输出  ：4 bit : Latch = 1 其他位初始化为00 即可
 * 调用  ：内部调用
 */
void opt3001_config()
{
    write_opt3001_register(0x01, 0xC410);
}

void read_opt3001_thread()
{
   s_opt3001_data = read_opt3001_register(0x00);
//   s_opt3001_data = (1 << ((s_opt3001_data & 0xf000) >> 12)) * (s_opt3001_data & 0x0FFF); //得到真实值的100
}

/*
 * 函数名：updata_opt3001_data
 * 描述  ：
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void updata_opt3001_data(uint16_t *value)
{
    *value = s_opt3001_data;
}