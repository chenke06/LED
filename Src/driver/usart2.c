/********************************************************************************
 * 文件名  ：usart2.c
 * 硬件连接：---------------------
 *          | 									  |
 *          | PA2  - USART2(Tx)   |
 *          | PA3  - USART2(Rx)   |
 *          |                     |
 *           ---------------------
 * 库版本  ：ST3.5.0
**********************************************************************************/
#include "usart2.h"
#include "misc.h"

/*
 * 函数名：usart2_config
 * 描述  ：USART2 GPIO 配置,工作模式配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void usart2_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

/* config USART2 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

/* USART2 GPIO config */
/* Configure USART2 Tx (PA.02) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	    
/* Configure USART2 Rx (PA.03) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	  
/* USART2 mode config */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);
//    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);
		
}
/*
 * 函数名：usart3_nvic_configuration
 * 描述  ：
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void usart2_nvic_configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
/* Configure the NVIC Preemption Priority Bits */  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
/* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void usart2_send_data(u8 *send_data, u8 lens)
{
    for (u8 i = 0; i < lens; i++){  
        USART_SendData(USART2, *(send_data + i));
        while (!(USART2->SR & USART_FLAG_TXE));
    }
}

/*
 * 函数名：fputc
 * 描述  ：重定向c库函数printf到USART1
 * 输入  ：无
 * 输出  ：无
 * 调用  ：由printf调用
 */
int fputc(int ch, FILE *f)
{
/* 将Printf内容发往串口 */
    USART_SendData(USART2, (unsigned char) ch);
    while (!(USART2->SR & USART_FLAG_TXE));
	
    return (ch);
}