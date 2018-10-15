/*******************************************************************************
 * �ļ���  ��opt3001.c
 * ����    ��opt3001Ӧ�ú�����        
 * ��汾  ��ST3.5.0
**********************************************************************************/
#include "opt3001.h"
#include "myi2c1.h"
#include "delay.h"

static __IO uint16_t s_opt3001_data = 0;
/*
 * ��������init_opt3001
 * ����  : 
 * ����  : 
 * ���  ����
 * ����  ���ڲ�����
 */
void init_opt3001()
{
    my_i2c1_init();
    opt3001_config();
}
/*
 * ��������write_opt3001_register
 * ����  : 
 * ����  : 
 * ���  ����
 * ����  ���ڲ�����
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
 * ��������read_opt3001_register
 * ����  : 
 * ����  : 
 * ���  ����
 * ����  ���ڲ�����
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
 * ��������get_opt3001_id
 * ����  : 
 * ����  : 
 * ���  ��0X5449
 * ����  ���ڲ�����
 */
uint16_t get_opt3001_id()
{
    uint16_t id_num = 0;
    id_num = read_opt3001_register(0x7E);
    return id_num;
}

/*
 * ��������opt3001_config
 * ����  : 12 - 15  RN[0,3]:0X1100 , full-scale mode
 * ����  : 9 - 10 : M[0,1]:0X10, ����ת��ģʽ
 * ���  ��4 bit : Latch = 1 ����λ��ʼ��Ϊ00 ����
 * ����  ���ڲ�����
 */
void opt3001_config()
{
    write_opt3001_register(0x01, 0xC410);
}

void read_opt3001_thread()
{
   s_opt3001_data = read_opt3001_register(0x00);
//   s_opt3001_data = (1 << ((s_opt3001_data & 0xf000) >> 12)) * (s_opt3001_data & 0x0FFF); //�õ���ʵֵ��100
}

/*
 * ��������updata_opt3001_data
 * ����  ��
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */
void updata_opt3001_data(uint16_t *value)
{
    *value = s_opt3001_data;
}