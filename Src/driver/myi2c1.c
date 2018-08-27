/********************************************************************************
 * �ļ���  ��i2c1.c
 * Ӳ�����ӣ�-----------------
 *          |                 |
 *          |  PB6-I2C1_SCL	  |
 *          |  PB7-I2C1_SDA   |
 *          |                 |
 *           -----------------
 * ��汾  ��ST3.5.0
 * ����    ������ 
**********************************************************************************/
#include "myi2c1.h"
#include "delay.h"



void my_i2c1_init()
{
    GPIO_InitTypeDef  GPIO_InitStructure; 

/* ʹ���� I2C1 �йص�ʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);  
    
/* PB6-I2C1_SCL��PB7-I2C1_SDA*/
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	       // ��©���
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/*
 * ��������my_iic1_start
 * ����  ��start
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */
void my_iic1_start(void)
{
    SDA_OUT();     //sda�����
    IIC_SDA_H;
    delay_us(100);	  	  
    IIC_SCL_H;
    delay_us(100);
    IIC_SDA_L;    //START:when CLK is high,DATA change form high to low 
    delay_us(100);
    IIC_SCL_L;
    delay_us(50);
}	  
/*
 * ��������my_iic1_stop
 * ����  ��
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */
void my_iic1_stop(void)
{
    SDA_OUT();
    IIC_SDA_L;    //STOP:when CLK is high DATA change form low to high
    delay_us(100);
    IIC_SCL_H; 
    delay_us(100);
    IIC_SDA_H;
    delay_us(100);	
}
/*
 * ��������my_iic1_stop
 * ����  ��
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */
uint8_t my_iic1_wait_ack(void)
{
    uint8_t ucErrTime = 0; 
	
    IIC_SCL_L;
    delay_us(100);	
    IIC_SDA_H;
    SDA_IN();
    delay_us(100);	   
    IIC_SCL_H;
    delay_us(100);	 
    while (READ_SDA){
        ucErrTime++;
        if (ucErrTime > 250){
        my_iic1_stop();
        //printf("IIC ACK ERROR\r\n");
        return 1;
				}
		}
    IIC_SCL_L;
    delay_us(100); 	   
    return 0;  
}
/*
 * ��������my_iic1_stop
 * ����  ��
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */
void my_iic1_ack(void)
{
    SDA_OUT();
    IIC_SDA_L;
    IIC_SCL_H;
    delay_us(100);
    IIC_SCL_L;
    delay_us(100);
}
/*
 * ��������my_iic1_stop
 * ����  ��
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */    
void my_iic1_nack(void)
{
	
    SDA_OUT();
    IIC_SDA_H;
    IIC_SCL_H;
    delay_us(100);
    IIC_SCL_L;
	  delay_us(100);
}					 				     
/*
 * ��������my_iic1_stop
 * ����  ��
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */		  
void my_iic1_send_byte(uint8_t txd)
{                        
    uint8_t t;   
	  SDA_OUT(); 	    
    IIC_SCL_L;//����ʱ�ӿ�ʼ���ݴ���
    for (t = 0; t < 8; t++){
        if ((txd & 0x80) >> 7){
            IIC_SDA_H;
				}
				else{
            IIC_SDA_L;
				}
        txd <<= 1; 	  
        delay_us(100);   
        IIC_SCL_H;
        delay_us(100); 
        IIC_SCL_L;	
        delay_us(100);
    }	 
} 	    
/*
 * ��������my_iic1_stop
 * ����  ��
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */  
uint8_t my_iic1_read_byte(unsigned char ack)
{
    unsigned char i,receive = 0;
    SDA_IN();//SDA����Ϊ����
    for (i = 0; i < 8; i++){
        IIC_SCL_L; 
        delay_us(100);
		    IIC_SCL_H;
        receive <<= 1;
        if (READ_SDA){
            receive++;
				}
		    delay_us(100);
    }			
    IIC_SCL_L;		
    if (!ack){
        my_iic1_nack();//����nACK
		}
    else{
        my_iic1_ack(); //����ACK  
		}			
    return receive;
}
