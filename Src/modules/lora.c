/*****************************************************************************
 * 文件名  ：lora.c
 * 描述    ：lora应用函数库        
 * 硬件连接：----------------------------------------------------
 *          |                                                    |
 *          | 串口1                                              |
 *           ----------------------------------------------------
 * 库版本  ：ST3.5.0
**********************************************************************************/
#include "lora.h"
#include "CRC16.h"
#include "string.h"
#include "usart1.h"

#include "RN8209.h"
#include "ds18b20.h"
#include "opt3001.h"
#include "delay.h"
#include "adc.h"
#include "task_schedul.h"
Lora_Protocol_TypeDef g_lora_protocol_rec =
{
    .start_b = 0,
    .address_b = 0,
    .function_b = 0,
    .reg_w = 0,
 
    .check_w = 0,
		.stop_b = 0,
    .count_10ms = 0,
	  .r_word_len_w = 0,
    .w_byte_len_w = 0,
    .w_word_len_b = 0,
    .w_n_reg_i_w = 0,
    .w_reg_updata = 0,
    .reg_array_addr = 0

};
Lora_Load_Mode_TypeDef g_load_mode =
{
	  .led_stastus_flag = 0,
	  .current_work_mode = 0,
	  .light_power = 0,
    .patrol_time_flag = 0,
    .patrol_time_value = 0,
    
	  .work_time_tonight = 0,            // min
    .work_time_total = 0,              // min
	 	.use_energy_tonight = 0,          // kw/h
    .use_energy_total = 0,             // kw/h
    .write_read_eerom_flag = 0
};

Lora_Timer_Mode_TypeDef g_timer_mode = 
{
    .turn_on_data_time = 0,
    .turn_off_data_time = 0,
	
    .load_time1 = 0,
    .load_power1 = 0,
    
    .load_time2 = 0,
    .load_power2 = 0,	
    
    .load_time3 = 0,
    .load_power3 = 0,	

    .load_time4 = 0,
    .load_power4 = 0,

    .load_time5 = 0,
    .load_power5 = 0,

    .load_time6 = 0,
    .load_power6 = 0,

    .load_time7 = 0,
    .load_power7 = 0,
		
		.load_time8 = 0,
    .load_power8 = 0,
		
		.load_time9 = 0,
    .load_power9 = 0,
		
		.load_time10 = 0,
    .load_power10 = 0,
    .mode_flag = 0
};

uint8_t s_crc_buf[CRC_BUF_SIZE];
uint16_t s_crc_len;

static uint8_t s_send_lora_buf[SEND_LORA_SIZE];
static uint16_t s_send_lora_len;

uint16_t g_pd0_r_reg[42] = {0};        
uint16_t g_pd5_r_reg[128] = {0};
uint16_t g_pd7_r_reg[56]  = {0};

uint16_t g_pd0_w_reg[4] = {0};        
uint16_t g_pd3_w_reg[14] = {0};
uint16_t g_pd5_w_reg[128] = {0};
uint16_t g_pd7_w_reg[56] = {0};

static struct rtc_time s_updata_time_w;

static Lora_Send_Protocol_TypeDef s_lora_send_timer;
/*
 * 函数名：start_bit_check_thread
 * 描述  ：放在1ms计数器里, g_lora_protocol_rec.start_b 
 *         用于接收标志，错误清零，接收完成清零
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void start_bit_check_thread()
{
    g_lora_protocol_rec.count_10ms++;
	
    if (g_lora_protocol_rec.count_10ms > 10){		  
        g_lora_protocol_rec.start_b = 1;
        g_lora_protocol_rec.count_10ms = 0;
		}		
}

/*
 * 函数名：lora_rec_thread
 * 描述  ：放在串口接收中断里,一帧数据解析
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void lora_rec_thread(uint8_t rec)
{
     static uint8_t s_rec_step = 0;
     switch (s_rec_step){
		     case 0 :                                                 //起始
			       if (g_lora_protocol_rec.start_b == 1){
					       if ( (rec == LOCAL_ADDRESS)){                 // || 地址(rec == BROAD_ADDRESS)
                     s_rec_step = 2; 
							       s_crc_len = 0;
	                   g_lora_protocol_rec.address_b = LOCAL_ADDRESS;
                     s_crc_buf[s_crc_len] = rec;
                     s_crc_len++;							 
                 }
                 else{
                     g_lora_protocol_rec.start_b = 0;               
                     memset(s_crc_buf, 0, sizeof(s_crc_buf));
                     s_crc_len = 0;
							       s_rec_step = 0; 
                 }
					   }
					   else{
                 s_rec_step = 0;    
					   }
						 break;
         case 1:

						 break;
         case 2:                                                   //功能
             if ((rec == 0X03)||(rec == 0X06)||(rec == 0x10)){     
                 g_lora_protocol_rec.function_b = rec;
                 s_crc_buf[s_crc_len] = rec;
                 s_crc_len++;							   
							   s_rec_step = 3;			 
					   }
						 else if ((rec == 0x78)||(rec == 0x79)){
							   g_lora_protocol_rec.function_b = rec;
                 s_crc_buf[s_crc_len] = rec;
                 s_crc_len++;							 
							   s_rec_step = 100;		 
						 }
					   else{
							   g_lora_protocol_rec.start_b = 0; 
                 send_lora_unusual(UNUSUAL_FUN_CODE);         //异常-发送功能异常码
                 memset(s_crc_buf, 0, sizeof(s_crc_buf));
                 s_crc_len = 0;	
							   s_rec_step = 0;	
					   }
						 break;                                        
				 case 3:                                              //地址先发高位，后发低位
             BYTE1(g_lora_protocol_rec.reg_w) = rec;
             s_crc_buf[s_crc_len] = rec;
             s_crc_len++;	
				     s_rec_step = 4;         
				     break;
         case 4:
             BYTE0(g_lora_protocol_rec.reg_w) = rec;
				     if ((check_value_region(g_lora_protocol_rec.reg_w, 0X4000, 0X40FF)) || (check_value_region(g_lora_protocol_rec.reg_w, 0XDF00, 0XDF0D)) 
						     || (check_value_region(g_lora_protocol_rec.reg_w, 0X4E00, 0X4E7F)) || (check_value_region(g_lora_protocol_rec.reg_w, 0XE080, 0XE0B7))){
						     s_crc_buf[s_crc_len] = rec;
                 s_crc_len++;
                 s_rec_step = 5;    
						 }
						 else{
                 send_lora_unusual(UNUSUAL_PDU_CODE);
						     memset(s_crc_buf, 0, sizeof(s_crc_buf));
                 s_crc_len = 0;	
							   s_rec_step = 0;	
						 }

				     break;
         case 5:
					   if (g_lora_protocol_rec.function_b == 0X03){    //读寄存器   跳6 读的字数    
                  BYTE1(g_lora_protocol_rec.r_word_len_w) = rec;
							    s_crc_buf[s_crc_len] = rec;
                  s_crc_len++;
                  s_rec_step = 6; 							  
						 }
						 else if (g_lora_protocol_rec.function_b == 0X06){ //写单个寄存器 跳 7 写入数据
						      BYTE1(g_lora_protocol_rec.w_sigle_reg_data_w) = rec;
							    s_crc_buf[s_crc_len] = rec;
                  s_crc_len++;
                  s_rec_step = 7; 
						 }
						 else if (g_lora_protocol_rec.function_b == 0X10){ //写n个寄存器 跳 8 9 10 11写字节数据 - 字数量 -数据内容 WORD
                  BYTE1(g_lora_protocol_rec.w_byte_len_w) = rec;
							    s_crc_buf[s_crc_len] = rec;
                  s_crc_len++;
                  s_rec_step = 8; 
             }
						 else{
							    g_lora_protocol_rec.start_b =0; 
						      memset(s_crc_buf, 0, sizeof(s_crc_buf));
                  s_crc_len = 0;
                  s_rec_step = 0; 							 
						 }
						 break;			 
         case 6:
				     BYTE0(g_lora_protocol_rec.r_word_len_w) = rec;
						 s_crc_buf[s_crc_len] = rec;
             s_crc_len++;
				     s_rec_step = 100;
             break;	
				 case 7:
             BYTE0(g_lora_protocol_rec.w_sigle_reg_data_w) = rec;
             s_crc_buf[s_crc_len] = rec;
             s_crc_len++;
				     s_rec_step = 100;
             break;	
         case 8:
             BYTE0(g_lora_protocol_rec.w_byte_len_w) = rec;
             s_crc_buf[s_crc_len] = rec;
             s_crc_len++;	
             static uint16_t s_w_n_reg_i;
				     s_w_n_reg_i = 0;
             s_rec_step = 9;
             break;
         case 9:
             BYTE0(g_lora_protocol_rec.w_word_len_b) = rec;
             s_crc_buf[s_crc_len] = rec;
             s_crc_len++;					 
             s_rec_step = 10;
             break;            					 
				 case 10:
             BYTE1(g_lora_protocol_rec.w_n_reg_data_w[s_w_n_reg_i]) = rec;
				     s_crc_buf[s_crc_len] = rec;
             s_crc_len++;
				     s_rec_step = 11;
				     break;
				 case 11:
					   BYTE0(g_lora_protocol_rec.w_n_reg_data_w[s_w_n_reg_i]) = rec;
	           s_crc_buf[s_crc_len] = rec;
             s_crc_len++;               
				     s_w_n_reg_i = s_w_n_reg_i + 1;
				     if ((s_crc_len - 7) >= (g_lora_protocol_rec.w_byte_len_w - 1)){
                 s_rec_step = 100;
						     g_lora_protocol_rec.w_n_reg_i_w = 0;
						 }
						 else{
                 s_rec_step = 10;
						 }
				     break;
				 case 100:
             BYTE0(g_lora_protocol_rec.check_w) = rec;
             s_rec_step = 101;				 
             break;
				 case 101:
				     BYTE1(g_lora_protocol_rec.check_w) = rec;
             g_lora_protocol_rec.stop_b = 1;
             g_lora_protocol_rec.start_b = 0;
				     s_rec_step = 0;	
             break;					 
         default:
					   memset(s_crc_buf, 0, sizeof(s_crc_buf));
             g_lora_protocol_rec.stop_b = 0;
             g_lora_protocol_rec.start_b = 0;
				     s_rec_step = 0;
             break;				 
		}
}
/*
 * 函数名：lora_analysis_thread
 * 描述  ：接收完成后，更新值，返回值或者执行操作
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
uint16_t crc_check = 0;
uint8_t lora_analysis_thread()
{
    
    uint16_t reg_region = 0;
	  uint16_t *reg_region_r_sp;
	 	uint16_t *reg_region_w_sp;

    uint16_t send_lora_crc = 0;
    
	
    memset(s_send_lora_buf, 0, sizeof(s_send_lora_buf));
    s_send_lora_len = 0;
	
    if (g_lora_protocol_rec.stop_b == 1){               
		    crc_check = crc16(s_crc_buf, s_crc_len);
        if (crc_check == g_lora_protocol_rec.check_w){              //一段数据接收完成
            s_send_lora_buf[0] = g_lora_protocol_rec.address_b;
            s_send_lora_buf[1] = g_lora_protocol_rec.function_b;
            s_send_lora_buf[2] = (uint8_t)(g_lora_protocol_rec.reg_w  >> 8);//BYTE1(g_lora_protocol_rec.reg_w);
            s_send_lora_buf[3] = (uint8_t)(g_lora_protocol_rec.reg_w);//BYTE0(g_lora_protocol_rec.reg_w);
 //寄存器解析					
				    reg_region = g_lora_protocol_rec.reg_w & 0XFF00;     
					
					  if (reg_region == 0X4000){                            //PD0                
						    reg_region_r_sp = g_pd0_r_reg;
							  reg_region_w_sp = g_pd0_w_reg;
		            g_lora_protocol_rec.reg_array_addr = g_lora_protocol_rec.reg_w - 0X4000;
						}
					  else if (reg_region == 0XDF00){                      //PD3
	              reg_region_w_sp = g_pd3_w_reg;
                g_lora_protocol_rec.reg_array_addr = g_lora_protocol_rec.reg_w - 0XDF00;
						}
						else if (reg_region == 0x4E00){                      //PD5
						    reg_region_r_sp = g_pd5_r_reg;
							  reg_region_w_sp = g_pd5_w_reg;
                g_lora_protocol_rec.reg_array_addr = g_lora_protocol_rec.reg_w - 0x4E00;
						}
					  else if (reg_region == 0XE000){                     //PD7
						    reg_region_r_sp = g_pd7_r_reg;
                reg_region_w_sp = g_pd7_w_reg;
                g_lora_protocol_rec.reg_array_addr = g_lora_protocol_rec.reg_w - 0XE080;
							  printf("DBUG");
						}
					  else
						{
						    return 0;
						}
//功能码解析						
					  if (g_lora_protocol_rec.function_b == 0X03){                             //读
						    s_send_lora_buf[4] = (uint8_t) (g_lora_protocol_rec.r_word_len_w);   
							  s_send_lora_len = 5;
  							uint8_t j = 0;
							  for (uint8_t i = 0; i < g_lora_protocol_rec.r_word_len_w ; i++){
									  s_send_lora_buf[5 + j] = BYTE1(reg_region_r_sp[g_lora_protocol_rec.reg_array_addr + i]);
                    s_send_lora_buf[6 + j] = BYTE0(reg_region_r_sp[g_lora_protocol_rec.reg_array_addr + i]);
									  s_send_lora_len = s_send_lora_len + 2;
									  j = j + 2;
								}
								
						}
						else if (g_lora_protocol_rec.function_b == 0X06){                      //单次写
						    s_send_lora_buf[4] = (uint8_t) (g_lora_protocol_rec.w_sigle_reg_data_w >> 8);//BYTE1(g_lora_protocol_rec.w_sigle_reg_data_w);
						    s_send_lora_buf[5] = (uint8_t) (g_lora_protocol_rec.w_sigle_reg_data_w);//BYTE0(g_lora_protocol_rec.w_sigle_reg_data_w);
					      s_send_lora_len = 6;
						    reg_region_w_sp[g_lora_protocol_rec.reg_array_addr] = g_lora_protocol_rec.w_sigle_reg_data_w;						  
                g_lora_protocol_rec.w_reg_updata = 1;                              
                
						}
						else if (g_lora_protocol_rec.function_b == 0X10){                     //写多个寄存器
						    s_send_lora_buf[4] = (uint8_t)(g_lora_protocol_rec.w_byte_len_w >> 8);
						    s_send_lora_buf[5] = (uint8_t)(g_lora_protocol_rec.w_byte_len_w);
					      s_send_lora_len = 6; 
							  
							  for (uint8_t i = 0; i < g_lora_protocol_rec.w_word_len_b ; i++){								  
                    reg_region_w_sp[g_lora_protocol_rec.reg_array_addr + i] = g_lora_protocol_rec.w_n_reg_data_w[i];						  
								}
                g_lora_protocol_rec.w_reg_updata = 1;							
						}
						else{
						    return 0;
						}
				    send_lora_crc = crc16(s_crc_buf, s_crc_len);
				    s_send_lora_buf[s_send_lora_len++] = (uint8_t)(send_lora_crc);//BYTE0(send_lora_crc);
		        s_send_lora_buf[s_send_lora_len++] = (uint8_t)(send_lora_crc >> 8);//BYTE1(send_lora_crc);
						delay_ms(1);
						usart1_send_data(s_send_lora_buf, s_send_lora_len);
						delay_us(5);
				}
        else{//校验不正确
				    send_lora_unusual(UNUSUAL_CRC_ERROR_CODE);
				}					
		}
		g_lora_protocol_rec.stop_b = 0;                              //一阵数据解析完成
		return 1;
}

/*
 * 函数名：send_lora_unusual
 * 描述  ：发送异常码
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void send_lora_unusual(uint16_t unusual_b)
{
    uint8_t str_abnormal[6];
    uint16_t unusual_check;
    str_abnormal[0] = LOCAL_ADDRESS;
    str_abnormal[1] = g_lora_protocol_rec.function_b | 0X80;
    str_abnormal[2] = (uint8_t)(unusual_b >> 8);//BYTE1(unusual_b);
    str_abnormal[3] = (uint8_t)(unusual_b);//BYTE0(unusual_b);
    unusual_check = crc16(str_abnormal, 4);
    str_abnormal[4] = (uint8_t)(unusual_check);//BYTE0(unusual_check);
    str_abnormal[5] = (uint8_t)(unusual_check >> 8);//BYTE1(unusual_check);
    usart1_send_data(str_abnormal, 6);
}
/*
 * 函数名：send_lora_unusual
 * 描述  ：发送异常码
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
uint8_t check_value_region(uint16_t value, uint16_t start, uint16_t end)
{
    if((value >= start) && (value <= end)){
		    return 1;
		}
		else{
        return 0;
		}
}
/*
 * 函数名：updata_lora_reg_data_thread
 * 描述  ：更新读写寄存器缓存，1ms
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */

void updata_lora_reg_data_thread()
{
//PD0
    struct rtc_time updata_time_r;
    updata_time_data(&updata_time_r);                           //更新上传时间数据
    
	  uint16_t password_protect_flag = 0;           
		
	  uint16_t current_fault_flag_h = 0;
	  uint16_t current_fault_flag_l = 0;
    
	  current_fault_flag_h = (uint16_t)(g_fault_detect_flag >> 16);//WORD1(light_time_total);
		current_fault_flag_l = (uint16_t)(g_fault_detect_flag);//WORD0(light_time_total);

	
	  RN8209_Front_TypeDef rn8209_data;           
	  updata_rn8209_data(&rn8209_data);                           //更新电量计数据

    float temp;
    int16_t temp_int;
	  updata_ntc_value(&temp);
    temp_int =(int16_t) temp;                                   //更新温度数据

	  uint16_t light_percent;
    light_percent = g_load_mode.light_power;                 
	
    uint16_t light_time_tonight;                             
		light_time_tonight = g_load_mode.work_time_tonight;         //今晚的亮灯时间
		
		uint32_t light_time_total;
		light_time_total = g_load_mode.work_time_total;             //总的亮灯时间
		
		uint16_t use_energy_tonight;
	  uint32_t use_energy_total;
		
		use_energy_tonight = g_load_mode.use_energy_tonight;
	  use_energy_total = g_load_mode.use_energy_total;
		
		
		uint16_t led_lux_value;
		float led_lux_cs;
		updata_light_value(&led_lux_cs);
		led_lux_value = (int16_t) led_lux_cs;                      //光照值

		uint16_t ppk_today;
		uint16_t ppk_history;
		
		uint16_t grid_gener_energy_today;
		uint16_t sava_money_today;
		uint16_t c02_reduce_today;
		
		uint16_t grid_gener_energy_month;
		uint16_t sava_money_month;
		uint16_t co2_reduce_month;
		
	  uint16_t grid_gener_energy_year;
		uint16_t sava_money_year;
		uint16_t co2_reduce_year; 

//PD3
    uint16_t cmd_power_onoff;
    uint16_t cmd_machine_reset;
		uint16_t cmd_restore_factory_setting;
		uint16_t cmd_clear_alarm;
		uint16_t cmd_clear_statistic;
		uint16_t cmd_clear_his_record;
		uint16_t cmd_upgrade;
		uint16_t cmd_sleep_run;
		uint16_t cmd_load_mode;
		uint16_t cmd_test_power;
    uint16_t cmd_test_time;
//PD5
    uint16_t grid_over_volt;
		uint16_t grid_over_volt_return;
		uint16_t grid_low_volt_return;
		uint16_t led_over_temper;
		uint16_t led_over_temper_return;
			
//PD7
    uint16_t controller_addr_set;
    uint16_t led_parm_reserved;
		uint16_t password_set_value;
		uint16_t password_input;
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
		
//将数据传送到寄存器
		
    BYTE1(g_pd0_r_reg[0]) = updata_time_r.tm_year - 2000;
    BYTE0(g_pd0_r_reg[0]) = updata_time_r.tm_mon;
	
    BYTE1(g_pd0_r_reg[1]) = updata_time_r.tm_mday;
	  BYTE0(g_pd0_r_reg[1]) = updata_time_r.tm_hour;
	
    BYTE1(g_pd0_r_reg[2]) = updata_time_r.tm_min;
    BYTE0(g_pd0_r_reg[2]) = updata_time_r.tm_sec;

    BYTE1(g_pd0_r_reg[3]) = updata_time_r.tm_wday;
	
	  g_pd0_r_reg[4] = password_protect_flag;

    g_pd0_r_reg[5] = current_fault_flag_h;
    g_pd0_r_reg[6] = current_fault_flag_l;

    g_pd0_r_reg[7] = rn8209_data.PubData.U;
    g_pd0_r_reg[8] = rn8209_data.PubData.Ia;
    g_pd0_r_reg[9] = rn8209_data.PubData.Frequency;
    g_pd0_r_reg[10] = rn8209_data.PubData.Pf;
		g_pd0_r_reg[11] = rn8209_data.PubData.Pw;
	  g_pd0_r_reg[12] = rn8209_data.PubData.Pw;
		
		g_pd0_r_reg[13] = temp_int;
    g_pd0_r_reg[14] = light_percent;
    g_pd0_r_reg[15] = light_time_tonight;
		
    g_pd0_r_reg[16] = (uint16_t)(light_time_total >> 16);//WORD1(light_time_total);
		g_pd0_r_reg[17] = (uint16_t)(light_time_total);//WORD0(light_time_total);
		
		g_pd0_r_reg[18] = use_energy_tonight;
	  g_pd0_r_reg[19] = (uint16_t)(use_energy_total >> 16);//WORD1(use_energy_total);
    g_pd0_r_reg[20] = (uint16_t)(use_energy_total);//WORD0(use_energy_total);
		
//		#ifdef _DBUG
//		
//		    printf("use_time_total %d\r\n",light_time_total);
//        printf("use_time_total_H %d\r\n",g_pd0_r_reg[16]);
//			  printf("use_time_total_L %d\r\n",g_pd0_r_reg[17]);
//				
//		    printf("use_energy_total %d\r\n",use_energy_total);
//        printf("use_energy_total_H %d\r\n",g_pd0_r_reg[19]);
//			  printf("use_energy_total_L %d\r\n",g_pd0_r_reg[20]);
//		#endif
		
	  g_pd0_r_reg[21] =  led_lux_value;

//将写数据传输到写寄存器
    if (g_lora_protocol_rec.w_reg_updata == 1){
		    uint16_t reg_temp = 0;
			  reg_temp = g_lora_protocol_rec.reg_w & 0XFF00;
			  if (reg_temp == 0X4000){                                //PD0数据
			      s_updata_time_w.tm_year = (uint8_t)(g_pd0_w_reg[0] >> 8);//BYTE1(g_pd0_w_reg[0]);
            s_updata_time_w.tm_mon = (uint8_t)(g_pd0_w_reg[0]);//BYTE0(g_pd0_w_reg[0]);
	
            s_updata_time_w.tm_mday = (uint8_t)(g_pd0_w_reg[1] >> 8);
            s_updata_time_w.tm_hour = (uint8_t)(g_pd0_w_reg[1]);
	
            s_updata_time_w.tm_min = (uint8_t)(g_pd0_w_reg[2] >> 8);
            s_updata_time_w.tm_sec = (uint8_t)(g_pd0_w_reg[2]);

            s_updata_time_w.tm_wday = (uint8_t)(g_pd0_w_reg[3] >> 8);
				}
				if (reg_temp == 0XDF00){                               //PD3数据
				    cmd_power_onoff = g_pd3_w_reg[0];
            cmd_machine_reset = g_pd3_w_reg[1];
            cmd_restore_factory_setting = g_pd3_w_reg[2];
            cmd_clear_alarm = g_pd3_w_reg[3];
            cmd_clear_statistic = g_pd3_w_reg[4];
            cmd_clear_his_record = g_pd3_w_reg[5];
	          cmd_upgrade = g_pd3_w_reg[6];
            cmd_sleep_run = g_pd3_w_reg[7];
            cmd_load_mode = g_pd3_w_reg[8];
            cmd_test_power = g_pd3_w_reg[9];
            cmd_test_time = g_pd3_w_reg[10];	
				}
			  if (reg_temp == 0XE000){				                      //PD7 数据
					  g_timer_mode.mode_flag = 1;
            g_timer_mode.turn_on_data_time = g_pd7_w_reg[6];
            g_timer_mode.turn_off_data_time = g_pd7_w_reg[7];
	
            g_timer_mode.load_time1 = g_pd7_w_reg[8];
            g_timer_mode.load_power1 = g_pd7_w_reg[9];
    
            g_timer_mode.load_time2 = g_pd7_w_reg[10];
            g_timer_mode.load_power2 = g_pd7_w_reg[11];
    
            g_timer_mode.load_time3 = g_pd7_w_reg[12];
            g_timer_mode.load_power3 = g_pd7_w_reg[13]; 

            g_timer_mode.load_time4 =  g_pd7_w_reg[14]; 
            g_timer_mode.load_power4 = g_pd7_w_reg[15]; 

            g_timer_mode.load_time5 =  g_pd7_w_reg[16]; 
            g_timer_mode.load_power5 = g_pd7_w_reg[17]; 

            g_timer_mode.load_time6 = g_pd7_w_reg[18]; 
            g_timer_mode.load_power6  = g_pd7_w_reg[19]; 

            g_timer_mode.load_time7 = g_pd7_w_reg[20]; 
            g_timer_mode.load_power7 = g_pd7_w_reg[21]; 
		
		        g_timer_mode.load_time8 = g_pd7_w_reg[22]; 
            g_timer_mode.load_power8 = g_pd7_w_reg[23]; 
		
		        g_timer_mode.load_time9 = g_pd7_w_reg[24]; 
            g_timer_mode.load_power9 = g_pd7_w_reg[25]; 
		
		        g_timer_mode.load_time10 = g_pd7_w_reg[26]; 
            g_timer_mode.load_power10 = g_pd7_w_reg[27]; 
				}	
		}
}


/*
 * 函数名：lora_send_timer
 * 描述  ：定时上传数据
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void lora_send_timer()
{
     uint8_t send_timer_str[91];
     uint16_t len_str = 0;
     s_lora_send_timer.address_b = LOCAL_ADDRESS;
     s_lora_send_timer.function_b = 0X03;
     s_lora_send_timer.reg_w = 0X4000;
	   s_lora_send_timer.data_len_b = R_REG_DATA_LEN;
     s_lora_send_timer.reg_data_w = g_pd0_r_reg;
	
     send_timer_str[0] = s_lora_send_timer.address_b;
     send_timer_str[1] = s_lora_send_timer.function_b;
	   send_timer_str[2] = (uint8_t)(s_lora_send_timer.reg_w >> 8);//BYTE1(s_lora_send_timer.reg_w);
     send_timer_str[3] = (uint8_t)(s_lora_send_timer.reg_w);//BYTE0(s_lora_send_timer.reg_w);
	   send_timer_str[4] = s_lora_send_timer.data_len_b;
	
     len_str = 5;
	   uint8_t j = 0;
     for (uint8_t i = 0; i < s_lora_send_timer.data_len_b;i++){
         send_timer_str[5 + j] = (uint8_t)(s_lora_send_timer.reg_data_w[i] >> 8);//BYTE1(s_lora_send_timer.reg_data_w[i]);
         send_timer_str[6 + j] = (uint8_t)(s_lora_send_timer.reg_data_w[i]);//BYTE0(s_lora_send_timer.reg_data_w[i]);
			   j = j + 2;
         len_str = len_str +2;
		 }
		 s_lora_send_timer.check_w  = crc16(send_timer_str, len_str);
		 send_timer_str[len_str] = (uint8_t)(s_lora_send_timer.check_w);//BYTE0(s_lora_send_timer.check_w);
     send_timer_str[len_str + 1] = (uint8_t)(s_lora_send_timer.check_w >> 8);//BYTE1(s_lora_send_timer.check_w);
     len_str = len_str +2;
     usart1_send_data(send_timer_str, len_str);			 
}


/*
 * 函数名：updata_time_write_data
 * 描述  ：更新写数据时间
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void updata_time_write_data(struct rtc_time *set_tm)
{
    set_tm->tm_year = s_updata_time_w.tm_year + 2000;
    set_tm->tm_mon  = s_updata_time_w.tm_mon;
	  set_tm->tm_mday = s_updata_time_w.tm_mday;
    set_tm->tm_hour = s_updata_time_w.tm_hour;
    set_tm->tm_min  = s_updata_time_w.tm_min;
    set_tm->tm_sec  = s_updata_time_w.tm_sec;
    set_tm->tm_wday = s_updata_time_w.tm_wday;
}