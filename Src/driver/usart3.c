/********************************************************************************
 * 文件名  ：usart3.c
 * 硬件连接：---------------------
 *          | 									  |
 *          | PD8  - USART3(Tx)   |
 *          | PD9  - USART3(Rx)   |
 *          |                     |
 *           ---------------------
 * 库版本  ：ST3.5.0
**********************************************************************************/
#include "usart3.h"
#include "misc.h"
/*
 * 函数名：usart3_config
 * 描述  ：USART3 GPIO 配置,工作模式配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void usart3_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
	
/* config USART1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC, ENABLE);
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);	
/* USART1 GPIO config */
/* Configure USART3 Tx (PC.12) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);    
/* Configure USART3 Rx (PC.11) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	
/* USART3 mode config */
    USART_InitStructure.USART_BaudRate = 4800;
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_Even;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
 
    USART_Init(USART3, &USART_InitStructure);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
    USART_Cmd(USART3, ENABLE);
}
/*
 * 函数名：usart3_nvic_configuration
 * 描述  ：
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void usart3_nvic_configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
/* Configure the NVIC Preemption Priority Bits */  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
/* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void usart3_send_data(u8 *send_data, u8 lens)
{
    for (u8 i = 0; i < lens; i++){  
        USART_SendData(USART3, *(send_data + i));
        while (!(USART3->SR & USART_FLAG_TXE));
    }
}

