#ifndef __ADC_H
#define	__ADC_H


#include "stm32f10x.h"
void adc1_init(void);
void updata_adc1_value(uint16_t *temp, uint16_t *light);
void updata_ntc_value(float *value_r);
void updata_light_value(float *value_r);
#endif /* __ADC_H */

