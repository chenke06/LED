/*******************************************************************************
 * �ļ���  ��led.c
 * Ӳ�����ӣ�-----------------
 *          |   PC7 - LED1       |
 *          |   PC8 - LED2       |
 *          |   PC9 - LED3     
 *          |   PA15 - L_control |
 *           ----------------- 
 * ��汾  ��ST3.5.0
**********************************************************************************/
#include "io.h"

/*
 * ��������led_gpio_config
 * ����  ������LED�õ���I/O��
 * ����  ����
 * ���  ����
 */
void led_gpio_config(void)
{		
/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
    GPIO_InitTypeDef GPIO_InitStructure;
/*����GPIOC������ʱ��*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE); 
/*ѡ��Ҫ���Ƶ�GPIOC����*/															   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	
/*��������ģʽΪͨ���������*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
/*������������Ϊ50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
/*���ÿ⺯������ʼ��GPIOC*/
    GPIO_Init(GPIOC, &GPIO_InitStructure);		  
/* �ر�����led��	*/
	  LED2(1);
}

/*
 * ��������led_gpio_config
 * ����  ������LED�õ���I/O��
 * ����  ����
 * ���  ����
 */
void power_onoff_gpio_config(void)
{		
/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
    GPIO_InitTypeDef GPIO_InitStructure;
/*����GPIOC������ʱ��*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE); 
/*ѡ��Ҫ���Ƶ�GPIOC����*/															   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 ;	
/*��������ģʽΪͨ���������*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;   
/*������������Ϊ50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
/*���ÿ⺯������ʼ��GPIOC*/
    GPIO_Init(GPIOC, &GPIO_InitStructure);		  
/* ����	*/
    LED0(0);
	  LED1(0);
}

/*
 * ��������led_gpio_config
 * ����  ������LED�õ���I/O��
 * ����  ����
 * ���  ����
 */
void temp_gpio_config(void)
{		
/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
    GPIO_InitTypeDef GPIO_InitStructure;
/*����GPIOC������ʱ��*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 	
/*ѡ��Ҫ���Ƶ�GPIOC����*/															   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	
/*��������ģʽΪͨ���������*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;   
/*������������Ϊ50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
/*���ÿ⺯������ʼ��GPIOC*/
    GPIO_Init(GPIOB, &GPIO_InitStructure);		  
/* �ر�����led��	*/
    TEMP(0);
}

/*
 * ��������led_gpio_config
 * ����  ������LED�õ���I/O��
 * ����  ����
 * ���  ����
 */
void light_gpio_config(void)
{		
/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
    GPIO_InitTypeDef GPIO_InitStructure;
/*����GPIOC������ʱ��*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); 
/*ѡ��Ҫ���Ƶ�GPIOC����*/															   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	
/*��������ģʽΪͨ���������*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
/*������������Ϊ50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
/*���ÿ⺯������ʼ��GPIOC*/
    GPIO_Init(GPIOA, &GPIO_InitStructure);		  
/* �ر�����led��	*/
   LIGHT(0);
}
/*
 * ��������led_gpio_config
 * ����  ������LED�õ���I/O��
 * ����  ����
 * ���  ����
 */
void relay_gpio_config(void)
{		
/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
    GPIO_InitTypeDef GPIO_InitStructure;
/*����GPIOC������ʱ��*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
/*ѡ��Ҫ���Ƶ�GPIOC����*/															   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	
/*��������ģʽΪͨ���������*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
/*������������Ϊ50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
/*���ÿ⺯������ʼ��GPIOC*/
    GPIO_Init(GPIOA, &GPIO_InitStructure);		  
/* �ر�����led��	*/
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
