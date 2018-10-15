/********************************************************************************
 * 文件名  ：Timer1.c
 * 描述    ：TIM1 CH1 捕获PWM应用函数库
 * 硬件连接：----------------------
 *          |					            |
 *          |	  TIM1_CH1 -- PE9   |
 *          |					            |
 *           ----------------------
 * 库版本  ：ST3.5.0
**********************************************************************************/
#include "Timer1.h"
/*
 * 函数名：tim1_nvic_configuration
 * 描述  ：TIM1中断优先级配置
 * 输入  ：无
 * 输出  ：无	
 */
void tim1_nvic_configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  													
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/*
 * 函数名：tim1_configuration
 * 描述  : 10khz 频率计数
 *       
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void tim1_configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;  
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
    TIM_ICInitTypeDef TIM_ICInitStructure;
  
    RCC_APB1PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);  
      
  
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;                   //设定计数器自动重装值
    TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;               //预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //设置时钟分割：TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式 
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;
	  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);               

    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICFilter = 0X01;
		
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInit(TIM1, &TIM_ICInitStructure);
		
		TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}
/*
 * 函数名：tim2_pwm_lnit
 * 描述  :      
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void tim1_cap_init(void)
{
    tim1_configuration();
}