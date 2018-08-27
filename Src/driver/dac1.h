#ifndef __dac1_H
#define __dac1_H

#include "stm32f10x.h"

#define PWM_MODE

void dac1_init(void);
void dac1_set_vol(u16 vol);
#endif
