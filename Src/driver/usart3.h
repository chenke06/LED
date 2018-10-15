#ifndef __USART3_H
#define	__USART3_H

#include "stm32f10x.h"
#include <stdio.h>

void usart3_init(void);
void usart3_nvic_configuration(void);
void usart3_send_data(u8 *send_data, u8 lens);

#endif /* __USART2_H */
