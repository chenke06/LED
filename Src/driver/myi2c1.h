#ifndef __MYI2C1_H
#define __MYI2C1_H

#include "stm32f10x.h"

//IO��������
#define SDA_IN()  {GPIOB->CRH&=~(3<<(7*2));GPIOB->CRH|=0<<7*2;}	//PB11����ģʽ
#define SDA_OUT() {GPIOB->CRH&=~(3<<(7*2));GPIOB->CRH|=1<<7*2;} //PB11���ģʽ
//IO��������	 
#define SCL_Pin GPIO_Pin_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_Pin_7
#define SDA_GPIO_Port GPIOB
					
#define IIC_SCL_H    GPIO_SetBits(SCL_GPIO_Port, SCL_Pin) //SCL
#define IIC_SDA_H    GPIO_SetBits(SDA_GPIO_Port, SDA_Pin) //SDA

#define IIC_SCL_L    GPIO_ResetBits(SCL_GPIO_Port, SCL_Pin) //SCL
#define IIC_SDA_L    GPIO_ResetBits(SDA_GPIO_Port, SDA_Pin) //SDA

#define READ_SDA   GPIO_ReadInputDataBit(SDA_GPIO_Port, SDA_Pin)  //����SDA 

//IIC���в�������	 
void my_i2c1_init(void);
void my_iic1_start(void);				//����IIC��ʼ�ź�
void my_iic1_stop(void);	  			//����IICֹͣ�ź�
void my_iic1_send_byte(unsigned char txd);			//IIC����һ���ֽ�
unsigned char my_iic1_read_byte(unsigned char ack);//IIC��ȡһ���ֽ�
unsigned char my_iic1_wait_ack(void); 				//IIC�ȴ�ACK�ź�
void my_iic1_ack(void);					//IIC����ACK�ź�
void my_iic1_nack(void);				//IIC������ACK�ź�
  

#endif

