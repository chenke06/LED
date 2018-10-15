#ifndef __task_schedul_H
#define	__task_schedul_H


#include "stm32f10x.h"

#define POWER_SUPPLY_ANOMALY_MAX 270
#define POWER_SUPPLY_ANOMALY_MIN 80

extern uint32_t g_fault_detect_flag;

void task_nvic_init(void);
void task_init(void);
void task_schedul(void);

void lora_send_exe_cmd_analy(void);
void lora_cmd_task_exe (void);
void light_on_off_task (uint8_t key);
void adjust_light_func(uint16_t light_value);
void send_lora_time_and_cal_total_time(void);
void light_on_off_task_thread(void);
void fault_detect_thread(void);

void test_timer_power_task(void);
void test_time_task(void);
void light_cs_thread(void);

extern int16_t g_task_last_min;
extern int16_t g_task_current_min;

extern int16_t g_task_min_count1;
extern int16_t g_lora_trans_count1;
extern int16_t g_task_min_count1_flag;



#endif
