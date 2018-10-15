#ifndef __IO_H
#define	__IO_H

#include "stm32f10x.h"

/* the macro definition to trigger the led on or off 
 * 1 - off
 - 0 - on
 */
#define ON  0
#define OFF 1

//带参宏，可以像内联函数一样使用
#define LED0(a)	if (a)	\
					GPIO_SetBits(GPIOC,GPIO_Pin_7);\
					else		\
					GPIO_ResetBits(GPIOC,GPIO_Pin_7)
					
#define LED1(a)	if (a)	\
					GPIO_SetBits(GPIOC,GPIO_Pin_8);\
					else		\
					GPIO_ResetBits(GPIOC,GPIO_Pin_8)
					
#define LED2(a)	if (a)	\
					GPIO_SetBits(GPIOC,GPIO_Pin_9);\
					else		\
					GPIO_ResetBits(GPIOC,GPIO_Pin_9)					

#define RELAY(a)	if (a)	\
					GPIO_SetBits(GPIOA,GPIO_Pin_15);\
					else		\
					GPIO_ResetBits(GPIOA,GPIO_Pin_15)			

#define TEMP(a)	if (a)	\
					GPIO_SetBits(GPIOB,GPIO_Pin_3);\
					else		\
					GPIO_ResetBits(GPIOB,GPIO_Pin_3)

#define LIGHT(a)	if (a)	\
					GPIO_SetBits(GPIOA,GPIO_Pin_7);\
					else		\
					GPIO_ResetBits(GPIOA,GPIO_Pin_7)						
					
void led_gpio_config(void);
void relay_gpio_config(void);
void io_init(void);
#endif /* __LED_H */
