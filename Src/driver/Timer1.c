/********************************************************************************
 * �ļ���  ��Timer1.c
 * ����    ��TIM1 CH1 ����PWMӦ�ú�����
 * Ӳ�����ӣ�----------------------
 *          |					            |
 *          |	  TIM1_CH1 -- PE9   |
 *          |					            |
 *           ----------------------
 * ��汾  ��ST3.5.0
**********************************************************************************/
#include "Timer1.h"
/*
 * ��������tim1_nvic_configuration
 * ����  ��TIM1�ж����ȼ�����
 * ����  ����
 * ���  ����	
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
 * ��������tim1_configuration
 * ����  : 10khz Ƶ�ʼ���
 *       
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
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
      
  
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;                   //�趨�������Զ���װֵ
    TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;               //Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //����ʱ�ӷָTDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ 
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
 * ��������tim2_pwm_lnit
 * ����  :      
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void tim1_cap_init(void)
{
    tim1_configuration();
}