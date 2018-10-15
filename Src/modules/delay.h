#ifndef __delay_H
#define	__delay_H


#include "stm32f10x.h"

extern __IO uint32_t g_timing_delay;

void delay_init(void);
void delay_us(u16 time);
void delay_ms(u16 time);


#endif /* __ADC_H */

