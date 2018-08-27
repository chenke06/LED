/**********************************************************************************
 * �ļ���  ��Timer4.c
 * ����    ��         
 * Ӳ�����ӣ�---------------------
 *          |  PB.09: (TIM4_CH4)  |
 *          |                     |
 *      	  |                     | 
 *    		  |                     |
 *           ---------------------    			
 * ��汾  ��ST3.5.0
 * ���  ��
 * ����  ���ڲ����ã�һ��8����ʱ��1��8�Ǹ߼���ʱ����234��103ϵ����û��5��ͨ�ö�ʱ����67�ǻ�����ʱ����ͨ�ö�ʱ��ÿ���ܲ���4·PWM��
**********************************************************************************/
#include "Timer4.h"

/*
 * ��������tim4_gpio_config
 * ����  ������TIM4�������PWMʱ�õ���I/O
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */
static void tim4_gpio_config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;

/* TIM4 clock enable */
//PCLK1����2��Ƶ����ΪTIM3��ʱ��Դ����72MHz
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 

/* GPIOC clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 

/*GPIOC Configuration: TIM4 channel 4 as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // �����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOB, &GPIO_InitStructure);

}

/*
 * ��������tim3_mode_config
 * ����  ������TIM3�����PWM�źŵ�ģʽ�������ڡ����ԡ�ռ�ձ�
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */
static void tim4_mode_config(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

/* PWM�źŵ�ƽ����ֵ */
    u16 CCR4_Val = 500;        
/* -----------------------------------------------------------------------
    TIM3 Configuration: generate 4 PWM signals with 4 different duty cycles:
    TIM3CLK = 72 MHz, Prescaler = 0x0, TIM3 counter clock = 72 MHz
    TIM3 ARR Register = 999 => TIM3 Frequency = TIM3 counter clock/(ARR + 1)
    TIM3 Frequency = 72 KHz.
    TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR)* 100 = 50%
	  5000 15k

  ----------------------------------------------------------------------- */

/* Time base configuration */		 
    TIM_TimeBaseStructure.TIM_Period = 50000;       //����ʱ����0������999����Ϊ1000�Σ�Ϊһ����ʱ����
    TIM_TimeBaseStructure.TIM_Prescaler = 20;	    //����Ԥ��Ƶ����Ԥ��Ƶ����Ϊ72MHz
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//����ʱ�ӷ�Ƶϵ��������Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //���ϼ���ģʽ

    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

/* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //����ΪPWMģʽ1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
    TIM_OCInitStructure.TIM_Pulse = CCR4_Val;	   //��������ֵ�������������������ֵʱ����ƽ��������
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ

    TIM_OC4Init(TIM4, &TIM_OCInitStructure);	 //ʹ��ͨ��1

    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM4, ENABLE);			 // ʹ��TIM3���ؼĴ���ARR

/* TIM3 enable counter */
    TIM_Cmd(TIM4, ENABLE);                   //ʹ�ܶ�ʱ��3	
}

/*
 * ��������tim3_pwm_init
 * ����  ��TIM3 ���PWM�źų�ʼ����ֻҪ�����������
 *         TIM3���ĸ�ͨ���ͻ���PWM�ź����
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void tim4_pwm_init(void)
{
    tim4_gpio_config();
    tim4_mode_config();
    tim4_set_pwm_duty(50000);	
}

void tim4_set_pwm_duty(u16 pwm1)
{
    TIM_SetCompare4(TIM4, pwm1);
}
