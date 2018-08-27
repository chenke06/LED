#ifndef __task_schedul_H
#define	__task_schedul_H


#include "stm32f10x.h"
void task_nvic_init(void);
void task_init(void);
void task_schedul(void);

void lora_send_exe_cmd_analy(void);
void lora_cmd_task_exe (void);
void light_on_off_task (uint8_t key);
void light_cs_thread(void);
void test_task(void);
void adjust_light_func(uint16_t light_value);



extern int16_t g_task_last_min;
extern int16_t g_task_current_min;

extern int16_t g_task_min_count1;
extern int16_t g_lora_trans_count1;
extern int16_t g_task_min_count1_flag;


extern int16_t g_task_last_hour;
extern int16_t g_task_current_hour;

extern int16_t g_task_last_day;
extern int16_t g_task_current_day;


#endif
