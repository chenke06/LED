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
#include "i2c1.h"
#define I2C1_Speed             10000
#define I2C1_OWN_ADDRESS7      0x0A

/*
 * ��������i2c_gpio_config
 * ����  ��I2C1 I/O����
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */
static void i2c1_gpio_config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure; 

/* ʹ���� I2C1 �йص�ʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);  
    
/* PB6-I2C1_SCL��PB7-I2C1_SDA*/
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       // ��©���
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*
 * ��������I2C_Configuration
 * ����  ��I2C ����ģʽ����
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */
static void i2c1_mode_config(void)
{
    I2C_InitTypeDef  I2C_InitStructure; 

/* I2C ���� */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 =I2C1_OWN_ADDRESS7; 
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = I2C1_Speed;
  
/* ʹ�� I2C1 */
    I2C_Cmd(I2C1, ENABLE);

/* I2C1 ��ʼ�� */
    I2C_Init(I2C1, &I2C_InitStructure);
}

/*
 * ��������i2c1_init
 * ����  ��
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */
void i2c1_init(void)
{
    i2c1_gpio_config();
    i2c1_mode_config();	
}