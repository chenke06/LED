/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stdio.h"
#include "RN8209.h"
#include "delay.h"
#include "rtc.h"
#include "lora.h"

static Task_Loop_Count_TypeDef s_timer2_task_count =
{
    .first = 0,
    .second = 0,
    .third = 0,
    .fourth = 0,
    .fifth = 0,
};
Task_Loop_Count_TypeDef g_task_state_flag = 
{
    .first = 0,
    .second = 0,
    .third = 0,
    .fourth = 0,
    .fifth = 0,
    .sixth = 0,
};

#define FIRST_TASK_SPEED  3
#define SECOND_TASK_SPEED 21
#define THIRD_TASK_SPEED  501
#define FOURTH_TASK_SPEED 751
#define FIFTH_TASK_SPEED  1001
#define SIXTH_TASK_SPEED  3001


volatile float g_timer1_fq = 0;
static volatile u8 s_timer1_state = 0;
static volatile u16 s_timer1_count1 = 0;
static volatile u16 s_timer1_count2 = 0;

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    if (g_timing_delay != 0){
        g_timing_delay--;
		}
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
/**
  * @brief  This function handles RTC interrupt request.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
	  if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	  {
	    /* Clear the RTC Second interrupt */
	    RTC_ClearITPendingBit(RTC_IT_SEC);
	
	    /* Enable time update */
	    g_rtc_count_irq = 1;
	
	    /* Wait until last write operation on RTC registers has finished */
	    RTC_WaitForLastTask();


	  }
}
void USART1_IRQHandler(void)
{
    u8 c;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){ 
        USART_ClearITPendingBit(USART1,USART_IT_RXNE); 			
        c = USART1->DR;
 //       printf("hh%d\r\n", c);
        lora_rec_thread(c);
        g_lora_protocol_rec.count_10ms = 0;               //收到数据就清零一次,判断起始位
        
	
    } 
}

void USART3_IRQHandler(void)
{
    u8 temp;
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){ 	
			  USART_ClearITPendingBit(USART3,USART_IT_RXNE); 
        temp = USART3->DR;
        g_rn8209_uart_com_pack.RxBuf[g_rn8209_uart_com_pack.RxLen++] = temp; 
    }
		else{
        g_rn8209_uart_com_pack.RxLen = 0;
		}
}
void TIM1_CC_IRQHandler(void)  
{
    float diff_value = 0;
    if (TIM_GetITStatus(TIM1,TIM_IT_CC1)!= RESET){  
        TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);    //清除中断标志
        //捕获第一个上升沿
        if (s_timer1_state == 0){  
            s_timer1_state = 1;  
            s_timer1_count1 = TIM_GetCapture1(TIM1);         //记录第一次上升沿的CNT值
       }                                                     //捕获第二个上升沿
       else if(s_timer1_state == 1){  
           s_timer1_state = 0;  
           s_timer1_count2 = TIM_GetCapture1(TIM1);          //记录第二次上升沿的CNT值
           if (s_timer1_count1 < s_timer1_count2){  
               diff_value = s_timer1_count2 - s_timer1_count1;     //两次上升沿的差值 
           }  
           else if(s_timer1_count1 > s_timer1_count2){  
               diff_value = (0xffff - s_timer1_count1) + s_timer1_count2;
           }  
           else  
               diff_value = 0;  
  
             g_timer1_fq = 1000000 / diff_value;           
       }  
   }  
}  

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET){	
        
        s_timer2_task_count.first++;   
        if (s_timer2_task_count.first >= FIRST_TASK_SPEED){
            start_bit_check_thread();					
            s_timer2_task_count.first = 0;
            g_task_state_flag.first = 1;          
        }
        s_timer2_task_count.second++;   
        if (s_timer2_task_count.second >= SECOND_TASK_SPEED){        
            s_timer2_task_count.second = 0;
            g_task_state_flag.second = 1;          
        }			
        s_timer2_task_count.third++;   
        if (s_timer2_task_count.third >= THIRD_TASK_SPEED){        
            s_timer2_task_count.third = 0;
            g_task_state_flag.third = 1;          
        }
        s_timer2_task_count.fourth++;   
        if (s_timer2_task_count.fourth >= FOURTH_TASK_SPEED){        
            s_timer2_task_count.fourth = 0;
            g_task_state_flag.fourth = 1;          
        }
        s_timer2_task_count.fifth++;   
        if (s_timer2_task_count.fifth >= FIFTH_TASK_SPEED){        
            s_timer2_task_count.fifth = 0;
            g_task_state_flag.fifth = 1;          
        }
				s_timer2_task_count.sixth++;   
        if (s_timer2_task_count.sixth >= SIXTH_TASK_SPEED){        
            s_timer2_task_count.sixth = 0;
            g_task_state_flag.sixth = 1;          
        }
        TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update); 			
			
    }		 	
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
