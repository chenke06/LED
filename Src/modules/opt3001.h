#ifndef __opt3001_H
#define	__opt3001_H


#include "stm32f10x.h"

#define OPT3001_ADDRESS 0X88
#define OPT3001_RESULT 0X00
#define OPT3001_CONFIG 0X01
#define OPT3001_LOW_LIMIT 0X02
#define OPT3001_HIGH_LIMIT 0X03
#define OPT3001_MANU_ID 0X7E
#define OPT3001_DEVICE_ID 0X7F

void init_opt3001(void);
void write_opt3001_register(uint8_t cmd, uint16_t value);
uint16_t read_opt3001_register(uint8_t cmd);
uint16_t get_opt3001_id(void);
void opt3001_config(void);
void read_opt3001_thread(void);
void updata_opt3001_data(uint16_t *value);
#endif

