/*******************************************************************************
 * 文件名  ：led.c
 * 硬件连接：-----------------
 *          |   PC7 - LED1       |
 *          |   PC8 - LED2       |
 *          |   PC9 - LED3     
 *          |   PA15 - L_control |
 *           ----------------- 
 * 库版本  ：ST3.5.0
**********************************************************************************/
#include "io.h"

/*
 * 函数名：led_gpio_config
 * 描述  ：配置LED用到的I/O口
 * 输入  ：无
 * 输出  ：无
 */
void led_gpio_config(void)
{		
/*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;
/*开启GPIOC的外设时钟*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE); 
/*选择要控制的GPIOC引脚*/															   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	
/*设置引脚模式为通用推挽输出*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
/*设置引脚速率为50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
/*调用库函数，初始化GPIOC*/
    GPIO_Init(GPIOC, &GPIO_InitStructure);		  
/* 关闭所有led灯	*/
	  LED2(1);
}

/*
 * 函数名：led_gpio_config
 * 描述  ：配置LED用到的I/O口
 * 输入  ：无
 * 输出  ：无
 */
void power_onoff_gpio_config(void)
{		
/*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;
/*开启GPIOC的外设时钟*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE); 
/*选择要控制的GPIOC引脚*/															   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 ;	
/*设置引脚模式为通用推挽输出*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;   
/*设置引脚速率为50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
/*调用库函数，初始化GPIOC*/
    GPIO_Init(GPIOC, &GPIO_InitStructure);		  
/* 开电	*/
    LED0(0);
	  LED1(0);
}

/*
 * 函数名：led_gpio_config
 * 描述  ：配置LED用到的I/O口
 * 输入  ：无
 * 输出  ：无
 */
void temp_gpio_config(void)
{		
/*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;
/*开启GPIOC的外设时钟*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 	
/*选择要控制的GPIOC引脚*/															   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	
/*设置引脚模式为通用推挽输出*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;   
/*设置引脚速率为50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
/*调用库函数，初始化GPIOC*/
    GPIO_Init(GPIOB, &GPIO_InitStructure);		  
/* 关闭所有led灯	*/
    TEMP(0);
}

/*
 * 函数名：led_gpio_config
 * 描述  ：配置LED用到的I/O口
 * 输入  ：无
 * 输出  ：无
 */
void light_gpio_config(void)
{		
/*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;
/*开启GPIOC的外设时钟*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); 
/*选择要控制的GPIOC引脚*/															   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	
/*设置引脚模式为通用推挽输出*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
/*设置引脚速率为50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
/*调用库函数，初始化GPIOC*/
    GPIO_Init(GPIOA, &GPIO_InitStructure);		  
/* 关闭所有led灯	*/
   LIGHT(0);
}
/*
 * 函数名：led_gpio_config
 * 描述  ：配置LED用到的I/O口
 * 输入  ：无
 * 输出  ：无
 */
void relay_gpio_config(void)
{		
/*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;
/*开启GPIOC的外设时钟*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
/*选择要控制的GPIOC引脚*/															   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	
/*设置引脚模式为通用推挽输出*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
/*设置引脚速率为50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
/*调用库函数，初始化GPIOC*/
    GPIO_Init(GPIOA, &GPIO_InitStructure);		  
/* 关闭所有led灯	*/
    RELAY(1);

}

void io_init(void)
{
    led_gpio_config();
    relay_gpio_config();
	  temp_gpio_config();
    light_gpio_config();
	  power_onoff_gpio_config();
}
