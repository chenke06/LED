#ifndef TIMER2_H
#define TIMER2_H

#include "stm32f10x.h"
 
#define START_TIME2  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);TIM_Cmd(TIM2, ENABLE)
#define STOP_TIME2  TIM_Cmd(TIM2, DISABLE);RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE)

void tim2_nvic_configuration(void);
void tim2_count_init(void);

#endif	/* TIME_TEST_H */
