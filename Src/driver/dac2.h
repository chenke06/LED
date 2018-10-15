#ifndef __dac2_H
#define __dac2_H

#include "stm32f10x.h"

//#define PWM_MODE

void dac2_init(void);
void dac2_set_vol(u16 vol);
#endif
