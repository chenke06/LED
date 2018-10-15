/********************************************************************************
 * 文件名  ：i2c2.c
 * 硬件连接：-----------------
 *          |                 |
 *          |  PB10-I2C1_SCL	  |
 *          |  PB11-I2C1_SDA   |
 *          |                 |
 *           -----------------
 * 库版本  ：ST3.5.0
 * 作者    ：保留 
**********************************************************************************/
#include "i2c2.h"
#define I2C2_Speed              400000
#define I2C2_OWN_ADDRESS7      0x0A

/*
 * 函数名：i2c_gpio_config
 * 描述  ：I2C1 I/O配置
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void i2c2_gpio_config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure; 

/* 使能与 I2C1 有关的时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);  
    
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       // 开漏输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*
 * 函数名：I2C_Configuration
 * 描述  ：I2C 工作模式配置
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void i2c2_mode_config(void)
{
    I2C_InitTypeDef  I2C_InitStructure; 

/* I2C 配置 */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 =I2C2_OWN_ADDRESS7; 
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = I2C2_Speed;
  
/* 使能 I2C1 */
    I2C_Cmd(I2C2, ENABLE);

/* I2C1 初始化 */
    I2C_Init(I2C2, &I2C_InitStructure);
}

/*
 * 函数名：i2c2_init
 * 描述  ：
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 */
void i2c2_init(void)
{
    i2c2_gpio_config();
    i2c2_mode_config();	
}