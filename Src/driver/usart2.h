#ifndef __USART2_H
#define	__USART2_H

#include "stm32f10x.h"
#include <stdio.h>

void usart2_init(void);
int fputc(int ch, FILE *f);
void usart2_printf(USART_TypeDef* USARTx, uint8_t *Data,...);
void usart2_nvic_configuration(void);
void usart2_send_data(u8 *send_data, u8 lens);
#endif /* __USART2_H */
