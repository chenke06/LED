#ifndef __USART1_H
#define	__USART1_H

#include "stm32f10x.h"
#include <stdio.h>

void usart1_init(void);
void usart1_nvic_configuration(void);
void usart1_send_data(u8 *send_data, u8 lens);
#endif /* __USART1_H */
