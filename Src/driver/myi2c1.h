#ifndef __MYI2C1_H
#define __MYI2C1_H

#include "stm32f10x.h"

//IO方向设置
#define SDA_IN()  {GPIOB->CRH&=~(3<<(7*2));GPIOB->CRH|=0<<7*2;}	//PB11输入模式
#define SDA_OUT() {GPIOB->CRH&=~(3<<(7*2));GPIOB->CRH|=1<<7*2;} //PB11输出模式
//IO操作函数	 
#define SCL_Pin GPIO_Pin_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_Pin_7
#define SDA_GPIO_Port GPIOB
					
#define IIC_SCL_H    GPIO_SetBits(SCL_GPIO_Port, SCL_Pin) //SCL
#define IIC_SDA_H    GPIO_SetBits(SDA_GPIO_Port, SDA_Pin) //SDA

#define IIC_SCL_L    GPIO_ResetBits(SCL_GPIO_Port, SCL_Pin) //SCL
#define IIC_SDA_L    GPIO_ResetBits(SDA_GPIO_Port, SDA_Pin) //SDA

#define READ_SDA   GPIO_ReadInputDataBit(SDA_GPIO_Port, SDA_Pin)  //输入SDA 

//IIC所有操作函数	 
void my_i2c1_init(void);
void my_iic1_start(void);				//发送IIC开始信号
void my_iic1_stop(void);	  			//发送IIC停止信号
void my_iic1_send_byte(unsigned char txd);			//IIC发送一个字节
unsigned char my_iic1_read_byte(unsigned char ack);//IIC读取一个字节
unsigned char my_iic1_wait_ack(void); 				//IIC等待ACK信号
void my_iic1_ack(void);					//IIC发送ACK信号
void my_iic1_nack(void);				//IIC不发送ACK信号
  

#endif

