/****************************************************************************
 * 文件名  ：task_schedul.c
 * 描述    ：任务应用函数库 
 * Timer1 --- 捕获输出方波频率
 * Timer2 --- 任务调度定时器
 * Timer3 --- PWM 输出
 * 库版本  ：ST3.5.0  
**********************************************************************************/
#include "task_schedul.h"
#include "Timer2.h"
#include "stm32f10x_it.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "delay.h"
#include "rtc.h"
#include "io.h"
#include "dac1.h"
#include "adc.h"
#include "RN8209.h"
#include "lora.h"
#include "timer4.h"
#include "AT24C080D.h"
static RN8209_FrontTmp_TypeDef s_RN8209_front_tmp_task;
static struct rtc_time s_set_tm;

int16_t g_task_min_count1 = 0;
int16_t g_task_last_min = 0;
int16_t g_task_current_min = 0;
int16_t g_lora_trans_count1 = 1;
int16_t g_task_min_count1_flag = 0;


int16_t g_task_last_hour = 0;
int16_t g_task_current_hour = 0;

int16_t g_task_last_day = 0;
int16_t g_task_current_day = 0;

int16_t cs_send_count = 0;
/*
 * 函数名：task_init
 * 描述  ：任务的初始化
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void task_init(void)
{
	  io_init();
	  delay_init();
    tim2_count_init();
    usart1_init();
    usart2_init();
	  at24c08_init();
    usart3_init();
    rn8209_init();
    dac1_init();
    adc1_init();
  	tim4_pwm_init();
    task_nvic_init();
    rtc_init(&g_tm);
//	  write_eepom_data(0, LIGHE_TIME_ADD); 
    g_load_mode.work_time_total = read_eepom_data(LIGHE_TIME_ADD);
	  printf("init finish %d\r\n", g_load_mode.work_time_total);
	
    START_TIME2;
}

/*
 * 函数名：task_nvic_init
 * 描述  ：任务驱动中断优先级初始化
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void task_nvic_init(void)
{
    usart1_nvic_configuration(); //1-0
    tim2_nvic_configuration();  //3-0
    usart3_nvic_configuration(); //4-0
    rtc_nvic_configuration();   //0 - 0 
}
/*
 * 函数名：task_schedul
 * 描述  ：任务调度
         : #define FIRST_TASK_SPEED  3
           #define SECOND_TASK_SPEED 20
           #define THIRD_TASK_SPEED  500
           #define FOURTH_TASK_SPEED 750
           #define FIFTH_TASK_SPEED  1000
           #define SIXTH_TASK_SPEED  3000
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
//测试用参数
RN8209_Front_TypeDef cs_rn8209_data;
u16 cs_opt3001_value;
u16 cs_temp;
static float s_temp_value_r;
static float s_light_value_r;
void task_schedul(void)
{
    if (g_task_state_flag.first == 1){                   //3ms进程   lora 解析
        updata_lora_reg_data_thread();
        lora_analysis_thread();
        g_task_state_flag.first = 0;
    }
	  if (g_task_state_flag.second == 1){                 //20ms进程   光照 读取     
        g_task_state_flag.second = 0;
    }
	  if (g_task_state_flag.third== 1){                   //500ms进程         
       if (g_lora_protocol_rec.w_reg_updata == 1){      //lora 时间修改，此版本只能同时修改
            if (g_lora_protocol_rec.reg_w == PD0_SYS_DATA_TIME){
                updata_time_write_data(&s_set_tm);
                rtc_set_time(&s_set_tm);
							  //printf("time set");
						}
				}
				else{
				    		time_thread(&g_tm);  
                printf ("%d-%d-%d-%d-%d-%d\r\n",g_tm.tm_year, g_tm.tm_mon, g_tm.tm_mday,
                           g_tm.tm_hour, g_tm.tm_min, g_tm.tm_sec);					
					      uint16_t dt;
                g_task_current_min = g_tm.tm_min;
					                                                         //计算上传时间间隔
 
					      if (g_task_min_count1 == 0)
								{
								    g_task_last_min = g_tm.tm_min;
								}
								if (g_task_min_count1 ==0){                        //第一次赋值
					          g_task_min_count1 = 1;
								}
					      if ((g_task_current_min < g_task_last_min) ){
                    dt = g_task_current_min +  60 - g_task_last_min;
								}
								else{
								    dt = g_task_current_min - g_task_last_min;
								}
								if (dt >= g_lora_trans_count1){
								    g_task_min_count1_flag = 1;
                    g_task_last_min = g_task_current_min;	
                    if (g_load_mode.work_time_tonight_flag == 1){
										    g_load_mode.work_time_tonight = g_load_mode.work_time_tonight + g_lora_trans_count1;
                        g_load_mode.work_time_total = g_load_mode.work_time_total + g_lora_trans_count1;
										}
                    else{
										    g_load_mode.work_time_tonight = 0;    
										}											
								}

				}  
        g_task_state_flag.third = 0;
    }		
	  if (g_task_state_flag.fourth == 1){                //750ms 进程
        updata_light_value(&s_light_value_r);
        updata_ntc_value(&s_temp_value_r);	     			
				lora_cmd_task_exe ();
			  g_task_state_flag.fourth = 0;
    }		
	  if (g_task_state_flag.fifth == 1){                 //1s进程 lora 执行操作 
       if (g_lora_protocol_rec.w_reg_updata == 1){    
            lora_send_exe_cmd_analy();
				    g_lora_protocol_rec.w_reg_updata = 0;	
				}
				if (g_task_min_count1_flag){                  //上传状态数据
					  g_task_min_count1_flag = 0; 
            lora_send_timer();		   
				}
        g_task_state_flag.fifth = 0;
    }
		
    if (g_task_state_flag.sixth == 1){                //3s 获取电量数据      		
	      read_rn8209_thread();
        updata_rn8209_data(&cs_rn8209_data);
			  printf("voltage %d\r\n",cs_rn8209_data.PubData.U);
		    g_task_state_flag.sixth	= 0;
		}
}
/*
 * 函数名：得到上lora传时间间隔
 * 描述  ：
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void get_updata_lora_time_interval()
{
    uint16_t dt;
    g_task_current_min = g_tm.tm_min;                  //计算上传时间间隔
    if (g_task_min_count1 == 0){
        g_task_last_min = g_tm.tm_min;
    }
    if (g_task_min_count1 ==0){                        //第一次赋值
        g_task_min_count1 = 1;
    }
    if ((g_task_current_min < g_task_last_min) ){
        dt = g_task_current_min +  60 - g_task_last_min;
    }
    else{
        dt = g_task_current_min - g_task_last_min;
    }
    if (dt >= g_lora_trans_count1){                  //到达上传时间就置标志位
        g_task_min_count1_flag = 1;
        g_task_last_min = g_task_current_min;							
    }
}

/*
 * 函数名：lora_send_exe_cmd_analy
 * 描述  ：lora执行命令解析
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void lora_send_exe_cmd_analy()
{
    if (g_lora_protocol_rec.reg_w == PD3_CMD_RESET){
        __set_FAULTMASK(1);
        NVIC_SystemReset();
    }
    if (g_lora_protocol_rec.reg_w == PD3_CMD_LOAD){
        if (g_pd3_w_reg[9] ==	0X0000){      //光控模式
            g_load_mode.light_control_auto_flag = 1;
            g_load_mode.test_flag = 0;   									
		    }
        else if (g_pd3_w_reg[9] ==	0X01A0){ // 开灯
            g_load_mode.test_flag = 1;
            g_load_mode.light_control_auto_flag = 0;
            g_load_mode.led_on_flag = 1;
            g_load_mode.led_off_flag = 0;     	
        }
        else if (g_pd3_w_reg[9] ==	0X015F){// 关灯
            g_load_mode.test_flag = 1;
	          g_load_mode.light_control_auto_flag = 0;
					  g_load_mode.led_on_flag = 0;
            g_load_mode.led_off_flag = 1;     	
			  }
			  else if (g_pd3_w_reg[9] ==	0X0300){// 定时模式
            g_load_mode.timer_mode_flag = 1;  	
			  }
    }
    if (g_lora_protocol_rec.reg_w == PD3_CMD_TEST_POWER){
        g_load_mode.light_test_power_flag = 1;
        g_load_mode.light_power = g_pd3_w_reg[10];							
    }
		if (g_lora_protocol_rec.reg_w == PD3_CMD_PATROL_TIME){
        g_load_mode.patrol_time_flag  = 1; 
			  g_load_mode.patrol_time_value = g_pd3_w_reg[10];	
		}
		
}

/*
 * 函数名：lora_cmd_task_exe
 * 描述  ：lora的执行操作
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void lora_cmd_task_exe ()
{
    if (g_load_mode.light_control_auto_flag == 1){    //自动亮灯模式     
        if (s_light_value_r > 1000){
            if (g_load_mode.work_time_tonight_flag == 0){
                light_on_off_task (1);
						}
        }
        if (s_light_value_r < 920){                
            if (g_load_mode.work_time_tonight_flag == 1){
                light_on_off_task (0);
						}			
        }	
    }
    if (g_load_mode.test_flag == 1){                 //手动开关灯模式									
        if(g_load_mode.led_on_flag == 1){
					  if (g_load_mode.work_time_tonight_flag == 0){
                light_on_off_task (1);
						}
				}
        else{
					  if (g_load_mode.work_time_tonight_flag == 1){
                light_on_off_task (0);
						}				 
        }
    }
    if (g_load_mode.light_test_power_flag == 1){     //调灯模式
			  adjust_light_func(g_load_mode.light_power);
        g_load_mode.timer_mode_flag = 0; 			
    }	
		if (g_load_mode.timer_mode_flag ==1 ){          //定时模式
		    uint16_t start_time_h;
        uint16_t start_time_m;

			  uint16_t end_time_h;
        uint16_t end_time_m;
			
			  static uint8_t s_step = 0;
			  static struct rtc_time s_start_tm;
			 
			  static uint16_t dt;
			
			  start_time_h = (g_timer_mode.turn_on_data_time & 0XFF00) >> 8;
			  start_time_m = (g_timer_mode.turn_on_data_time & 0X00FF) ;
      
        end_time_h = (g_timer_mode.turn_off_data_time & 0XFF00) >> 8;
	      end_time_m = (g_timer_mode.turn_off_data_time & 0X00FF) ;
           
        if((g_tm.tm_hour  >=  start_time_h) && (g_tm.tm_min >= start_time_m)){
			      g_timer_mode.mode_flag = 1;	
					  s_start_tm =  g_tm;
			  }
			  if((g_tm.tm_hour  >=  end_time_h) && (g_tm.tm_min >= end_time_m)){
			      g_timer_mode.mode_flag = 0;
            s_step = 0;					
			  }
				
			  if (g_timer_mode.mode_flag == 1){
				    switch (s_step){
						    case 0:	
                    adjust_light_func(g_timer_mode.load_power1);
                    dt = get_rtc_time_interval_min(&s_start_tm);
                    if (dt > g_timer_mode.load_time1){
										    s_step = 1; 
											  s_start_tm = g_tm;	
										}
                    else{
										    s_step = 0; 
										}
										
                    break;
                case 1:              			
                    adjust_light_func(g_timer_mode.load_power2);
								    dt = get_rtc_time_interval_min(&s_start_tm);
                    if (dt > g_timer_mode.load_time2){
										    s_step = 2; 
											  s_start_tm = g_tm;
										}
                    else{
										    s_step = 1; 
										}	
                    break;
                case 2:                
                    adjust_light_func(g_timer_mode.load_power3);
								    dt = get_rtc_time_interval_min(&s_start_tm);
                    if (dt > g_timer_mode.load_time3){
										    s_step = 3;
                        s_start_tm = g_tm;											
										}
                    else{
										    s_step = 2; 
										}	
                    break;
								case 3:
                    adjust_light_func(g_timer_mode.load_power4);
								    dt = get_rtc_time_interval_min(&s_start_tm);
                    if (dt > g_timer_mode.load_time4){
										    s_step = 4; 
											  s_start_tm = g_tm;
										}
                    else{
										    s_step = 3; 
										}
										break;
							  case 4:
                    adjust_light_func(g_timer_mode.load_power5);
								    dt = get_rtc_time_interval_min(&s_start_tm);
                    if (dt > g_timer_mode.load_time5){
										    s_step = 5;
											  s_start_tm = g_tm;
										}
                    else{
										    s_step = 4; 
										}
										break;
							  case 5:
                    adjust_light_func(g_timer_mode.load_power6);
								    dt = get_rtc_time_interval_min(&s_start_tm);
                    if (dt > g_timer_mode.load_time6){
										    s_step = 7;
                        s_start_tm = g_tm;											
										}
                    else{
										    s_step = 6; 
										}
										break;
                case 6:
                    adjust_light_func(g_timer_mode.load_power7);
								    dt = get_rtc_time_interval_min(&s_start_tm);
                    if (dt > g_timer_mode.load_time7){
										    s_step = 8;
                        s_start_tm = g_tm;											
										}
                    else{
										    s_step = 7; 
										}
										break;
                case 7:
                    adjust_light_func(g_timer_mode.load_power8);
								    dt = get_rtc_time_interval_min(&s_start_tm);
                    if (dt > g_timer_mode.load_time8){
										    s_step = 8;
											  s_start_tm = g_tm;
										}
                    else{
										    s_step = 7; 
										}
										break;
                case 8:
                    adjust_light_func(g_timer_mode.load_power9);
								    dt = get_rtc_time_interval_min(&s_start_tm);
                    if (dt > g_timer_mode.load_time9){
										    s_step = 9;
                        s_start_tm = g_tm;											
										}
                    else{
										    s_step = 8; 
										}
										break;										
                case 9:
                    adjust_light_func(g_timer_mode.load_power10);
								    dt = get_rtc_time_interval_min(&s_start_tm);
                    if (dt > g_timer_mode.load_time10){
										    s_step = 0;
                        s_start_tm = g_tm;											
										}
                    else{
										    s_step = 9; 
										}
										break;
                default:
									  s_step = 0;
								    s_start_tm = g_tm;
                    break;									
						}
				}
        else{
            adjust_light_func(0);
				}	       	   	
		}
		
		if (g_load_mode.patrol_time_flag == 1){        //设置上传时间
        g_lora_trans_count1 = g_load_mode.patrol_time_value;
		    g_load_mode.patrol_time_flag  = 0; 			
		}
		
}

/*
 * 函数名：adjust_light_func
 * 描述  ：调光操作
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void adjust_light_func(uint16_t light_value)
{
	  uint16_t test_power_temp;
	  RELAY(1);
    g_load_mode.light_power = light_value;
    #ifdef PWM_MODE
    test_power_temp = 50000 - g_load_mode.light_power * 500;
    tim4_set_pwm_duty(test_power_temp);	
    #else
    test_power_temp = g_load_mode.light_power * 20;
    dac1_set_vol(test_power_temp);
    #endif
}


/*
 * 函数名：light_on_off_task
 * 描述  ：开关灯操作
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void light_on_off_task (uint8_t key)
{
	  uint16_t  test_power;
    if (key == 1){
        RELAY(1);
        g_load_mode.work_time_tonight_flag = 1;                   //开灯标志
        g_load_mode.write_read_eerom_flag = 2;                    //读eerom 标志
        
        g_load_mode.work_time_total = read_eepom_data(LIGHE_TIME_ADD);
        			
        g_load_mode.light_power = 50;
			  test_power = g_load_mode.light_power * 20;
        dac1_set_vol(test_power);
//        test_power = 50000 - g_load_mode.light_power * 500;
//        tim4_set_pwm_duty(test_power);
		}
		else{
		    RELAY(0);
			  g_load_mode.work_time_tonight_flag = 0;                          //关灯标志
			  g_load_mode.write_read_eerom_flag = 1;                          //写eerom 标志
			  write_eepom_data(g_load_mode.work_time_total, LIGHE_TIME_ADD);  //关灯存数据
				g_load_mode.light_power = 0;
        test_power = g_load_mode.light_power * 20;
        dac1_set_vol(test_power);			
        //test_power = 50000 - g_load_mode.light_power * 500;
        //tim4_set_pwm_duty(test_power);	
		}
}
/*
 * 函数名：task_cs_schedul
 * 描述  ：任务亮度调节测试
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void light_cs_thread()
{
	  static uint16_t s_count = 0;
	  static uint16_t flag;

    for (uint8_t i = 0; i < 100; i++){
	     
			 tim4_set_pwm_duty(s_count);
			 delay_ms(50);
			 if(s_count < 1000){
		       flag = 0; 
			 }
			 if(s_count > 49000){
		       flag = 1; 
			 }
			 if (flag == 0){
           s_count = s_count + 500;
			 }
			 else{
				   s_count = s_count - 500; 
			 }
	 }
	
}

/*
 * 函数名：test_task
 * 描述  ：测试函数
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void test_task()
{
	  delay_ms(60000);
	  eepom_test();
	  s_set_tm.tm_year = 2018;
    s_set_tm.tm_mon = 8;
    s_set_tm.tm_mday = 26;
    s_set_tm.tm_hour = 11;
    s_set_tm.tm_min = 25;
		s_set_tm.tm_sec = 30;
	
    rtc_set_time(&s_set_tm);
    time_thread(&g_tm);	
}
