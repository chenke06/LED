/**********************************************************************************
 * 文件名  ：Timer4.c
 * 描述    ：         
 * 硬件连接：---------------------
 *          |  PB.09: (TIM4_CH4)  |
 *          |                     |
 *      	  |                     | 
 *    		  |                     |
 *           ---------------------    			
 * 库版本  ：ST3.5.0
 * 输出  ：
 * 调用  ：内部调用，一共8个定时器1和8是高级定时器，234（103系列中没有5）通用定时器，67是基本定时器，通用定时器每个能产生4路PWM，
**********************************************************************************/
#include "Timer4.h"

/*
 * 函数名：tim4_gpio_config
 * 描述  ：配置TIM4复用输出PWM时用到的I/O
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void tim4_gpio_config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;

/* TIM4 clock enable */
//PCLK1经过2倍频后作为TIM3的时钟源等于72MHz
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 

/* GPIOC clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 

/*GPIOC Configuration: TIM4 channel 4 as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOB, &GPIO_InitStructure);

}

/*
 * 函数名：tim3_mode_config
 * 描述  ：配置TIM3输出的PWM信号的模式，如周期、极性、占空比
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void tim4_mode_config(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

/* PWM信号电平跳变值 */
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
    TIM_TimeBaseStructure.TIM_Period = 50000;       //当定时器从0计数到999，即为1000次，为一个定时周期
    TIM_TimeBaseStructure.TIM_Prescaler = 20;	    //设置预分频：不预分频，即为72MHz
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//设置时钟分频系数：不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式

    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

/* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
    TIM_OCInitStructure.TIM_Pulse = CCR4_Val;	   //设置跳变值，当计数器计数到这个值时，电平发生跳变
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //当定时器计数值小于CCR1_Val时为高电平

    TIM_OC4Init(TIM4, &TIM_OCInitStructure);	 //使能通道1

    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM4, ENABLE);			 // 使能TIM3重载寄存器ARR

/* TIM3 enable counter */
    TIM_Cmd(TIM4, ENABLE);                   //使能定时器3	
}

/*
 * 函数名：tim3_pwm_init
 * 描述  ：TIM3 输出PWM信号初始化，只要调用这个函数
 *         TIM3的四个通道就会有PWM信号输出
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
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
