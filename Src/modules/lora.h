#ifndef __LORA_H
#define	__LORA_H

#include "stm32f10x.h"
#include "rtc.h"


#define W_N_REG_LEN 20
#define CRC_BUF_SIZE 80
#define SEND_LORA_SIZE 30
typedef struct
{
    uint8_t start_b;
    uint8_t address_b;
    uint8_t function_b;
    uint16_t reg_w;
    uint16_t check_w;
    uint8_t stop_b;
    uint8_t count_10ms;
    
    uint16_t r_word_len_w;
    uint16_t w_byte_len_w;
    uint8_t w_word_len_b;
    uint16_t w_sigle_reg_data_w;
    uint16_t w_n_reg_data_w[W_N_REG_LEN];
    uint16_t w_n_reg_i_w;
    
	  uint8_t w_reg_updata;
    uint16_t reg_array_addr;

}Lora_Protocol_TypeDef;

#define R_REG_DATA_LEN 42
typedef struct
{
    uint8_t address_b;
    uint8_t function_b;
    uint16_t reg_w;
	  uint8_t data_len_b;
    uint16_t *reg_data_w;
    uint16_t check_w;

}Lora_Send_Protocol_TypeDef;


typedef struct
{
    uint8_t local_add;
    uint8_t error_code;
    uint16_t abnormal_code;
    uint16_t check_code;
}Lora_Abnormal_Response_TypeDef;


typedef struct
{
	  uint8_t led_stastus_flag;
	  uint8_t current_work_mode;
	
    uint16_t light_power;
	
    uint8_t patrol_time_flag;
    uint16_t patrol_time_value;
	
    uint16_t work_time_tonight;            // min
    uint32_t work_time_total;              // min
	
    uint16_t use_energy_tonight;          // kw/h
    uint32_t use_energy_total;            // kw/h
	 
	  float use_energy_tonight_temp;
	  float use_energy_total_temp;
	
    uint8_t write_read_eerom_flag;
}Lora_Load_Mode_TypeDef;


typedef struct
{
    uint16_t turn_on_data_time;
    uint16_t turn_off_data_time;
	
    uint16_t load_time1;
    uint16_t load_power1;
    
    uint16_t load_time2;
    uint16_t load_power2;	
    
    uint16_t load_time3;
    uint16_t load_power3;	

    uint16_t load_time4;
    uint16_t load_power4;

    uint16_t load_time5;
    uint16_t load_power5;

    uint16_t load_time6;
    uint16_t load_power6;

    uint16_t load_time7;
    uint16_t load_power7;
		
		uint16_t load_time8;
    uint16_t load_power8;
		
		uint16_t load_time9;
    uint16_t load_power9;
		
		uint16_t load_time10;
    uint16_t load_power10;

    uint8_t mode_flag; 
}Lora_Timer_Mode_TypeDef;


extern uint16_t g_pd0_r_reg[R_REG_DATA_LEN];        
extern uint16_t g_pd5_r_reg[128];
extern uint16_t g_pd7_r_reg[56];

extern uint16_t g_pd0_w_reg[4];        
extern uint16_t g_pd3_w_reg[14];
extern uint16_t g_pd5_w_reg[128];
extern uint16_t g_pd7_w_reg[56];

extern Lora_Load_Mode_TypeDef  g_load_mode;
extern Lora_Timer_Mode_TypeDef g_timer_mode;

#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))

//#define WORD0(dwTemp)       (*(char *)(&dwTemp))
//#define WORD1(dwTemp)       (*((char *)(&dwTemp) + 2))


#define BROAD_ADDRESS 0X00
#define LOCAL_ADDRESS 0X01

#define UNUSUAL_FUN_CODE 0X01
#define UNUSUAL_PDU_CODE 0X02
#define UNUSUAL_DATA_LONG_CODE 0X03
#define UNUSUAL_WR_ERROR_CODE  0X04
#define UNUSUAL_CRC_ERROR_CODE 0X05

#define PD0_SYS_DATA_TIME         0X4000

#define PD3_CMD_POWER_ONOFF       0XDF00
#define PD3_CMD_RESET             0XDF01
#define PD3_CMD_RESTORE_FACTORY   0XDF02
#define PD3_CMD_LOAD        0XDF09
#define PD3_CMD_TEST_POWER  0XDF0A
#define PD3_CMD_PATROL_TIME 0XDF0C



void start_bit_check_thread(void);
void lora_rec_thread(uint8_t rec);
uint8_t lora_analysis_thread(void);
void send_lora_unusual(uint16_t unusual_b);
uint8_t check_value_region(uint16_t value, uint16_t start, uint16_t end);
void updata_lora_reg_data_thread(void);

void updata_time_write_data(struct rtc_time *set_tm);
void lora_send_timer(void);
extern Lora_Protocol_TypeDef g_lora_protocol_rec;

#endif

