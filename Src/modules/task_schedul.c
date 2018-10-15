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
#include "dac2.h"
#include "adc.h"
#include "RN8209.h"
#include "lora.h"
#include "timer4.h"
#include "AT24C080D.h"

#define _DBUG

//RTC 时间变量
static struct rtc_time s_set_tm;
static struct rtc_time s_start_read_rom_tm;  //读rom值时间变量
 
//计算上传时间间隔变量
int16_t g_task_min_count1 = 0;            //第一次时间赋值标志
int16_t g_task_last_min = 0;              //上次时间间隔
int16_t g_task_current_min = 0;           //当前时间
int16_t g_lora_trans_count1 = 1;          //下传的上传时间间隔，默认为1min
int16_t g_task_min_count1_flag = 0;       //时间间隔到达标志位

//故障检测状态位
uint32_t g_fault_detect_flag = 0;

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
	  dac2_init();
    adc1_init();
  	tim4_pwm_init();
    task_nvic_init();
    rtc_init(&g_tm);
  
  	 
    g_load_mode.work_time_total = read_eepom_data(LIGHE_TIME_ADD);
	  delay_ms(20);
	  g_load_mode.use_energy_total_temp = read_eepom_data(LIGHE_POWER_ADD);
	
   	#ifdef _DBUG
	      test_time_task();
//      test_timer_power_task();							      
	      printf("test timer\r\n");
				printf("init finish %d\r\n", g_load_mode.work_time_total);
	  #endif
		time_thread(&g_tm);
		s_start_read_rom_tm = g_tm;
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
    usart1_nvic_configuration();  //1-0
    tim2_nvic_configuration();    //3-0
    usart3_nvic_configuration();  //4-0
    rtc_nvic_configuration();     //0 - 0 
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
#ifdef _DBUG
    RN8209_Front_TypeDef cs_rn8209_data;
    static float s_temp_value_r;
    static float s_light_value_r;
#endif

void task_schedul(void)
{
    if (g_task_state_flag.first == 1){                    //3ms进程   lora 寄存器解析
        updata_lora_reg_data_thread();                    //更新读写寄存器
        lora_analysis_thread();                           //一帧数据接收完成后，返回数据，以及将收到的数据放到缓存区
        g_task_state_flag.first = 0;
    }
	  if (g_task_state_flag.second == 1){                 //20ms进程 
        g_task_state_flag.second = 0;
    }
	  if (g_task_state_flag.third== 1){                   //500ms进程 ,时间 读取修改       
       if (g_lora_protocol_rec.w_reg_updata == 1){    
            if (g_lora_protocol_rec.reg_w == PD0_SYS_DATA_TIME){  //lora 时间修改，此版本只能同时修改               			  
							 // #ifdef _DBUG
							 //     test_time_task();
							 // #else
							      updata_time_write_data(&s_set_tm);
							      rtc_set_time(&s_set_tm);			
							//  #endif
						}
				}
				else{
				    		time_thread(&g_tm);
                send_lora_time_and_cal_total_time();  					
					      #ifdef _DBUG
                    printf ("%d-%d-%d-%d-%d-%d\r\n",g_tm.tm_year, g_tm.tm_mon, g_tm.tm_mday,
                           g_tm.tm_hour, g_tm.tm_min, g_tm.tm_sec);	
					         //printf ("timer_mode_flag = %d, mode_flag = %d\r\n",g_load_mode.timer_mode_flag ,g_timer_mode.mode_flag);
					      #endif
				}  
        g_task_state_flag.third = 0;
    }		
	  if (g_task_state_flag.fourth == 1){                //750ms 进程
        updata_light_value(&s_light_value_r);          //获取亮度值 
        updata_ntc_value(&s_temp_value_r);	           //获取温度值   			
				lora_cmd_task_exe ();                          //lora 动作执行
			  g_task_state_flag.fourth = 0;
    }		
	  if (g_task_state_flag.fifth == 1){                 //1s进程
       if (g_lora_protocol_rec.w_reg_updata == 1){    
            lora_send_exe_cmd_analy();                 //lora 模式解析
				    g_lora_protocol_rec.w_reg_updata = 0;	
				}
				if (g_task_min_count1_flag){                  //上传状态数据
					  g_task_min_count1_flag = 0; 
            lora_send_timer();		   
				}
				fault_detect_thread();
        g_task_state_flag.fifth = 0;
    }
		
    if (g_task_state_flag.sixth == 1){                //3s 获取电量数据      		
	      read_rn8209_thread();
        updata_rn8209_data(&cs_rn8209_data);
			  #ifdef _DBUG
			      printf("voltage %d\r\n",cs_rn8209_data.PubData.U);
			      printf("crrent %d\r\n",cs_rn8209_data.PubData.Ia);
			      printf("power %d\r\n",cs_rn8209_data.PubData.Pw);
			      printf("pf %d\r\n",cs_rn8209_data.PubData.Pf);
			      printf("use_energy_tonight %d\r\n",g_load_mode.use_energy_tonight);
			      printf("use_energy_total %d\r\n",g_load_mode.use_energy_total); 
//			      printf("angle %d\r\n",cs_rn8209_data.PubData.Angle);
			  #endif
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
           g_load_mode.current_work_mode = 1  光控模式
           g_load_mode.current_work_mode = 2  调灯模式
           g_load_mode.current_work_mode = 3  定时模式 

 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void lora_send_exe_cmd_analy()
{
	  
	  if (g_lora_protocol_rec.reg_w == PD3_CMD_POWER_ONOFF){    //掉电重启
        if (g_pd3_w_reg[0] == 1){                             //开电
				     LED0(0);    
	           LED1(0);
				}
				else{                                                 //断电
						 LED0(1);
	           LED1(1);
					   delay_ms(6000);
					   LED0(0);    
	           LED1(0);
					
				}
    }
    if (g_lora_protocol_rec.reg_w == PD3_CMD_RESET){           //复位
        __set_FAULTMASK(1);
        NVIC_SystemReset();
    }
		if (g_lora_protocol_rec.reg_w == PD3_CMD_RESTORE_FACTORY){  //恢复出厂设置
		    write_eepom_data(0, LIGHE_POWER_ADD);
		    delay_ms(20);
		    write_eepom_data(0, LIGHE_TIME_ADD);
		}
    if (g_lora_protocol_rec.reg_w == PD3_CMD_LOAD){
        if (g_pd3_w_reg[9] ==	0X0000){                     //光控模式
     				g_load_mode.current_work_mode = 1;					
		    }
        else if (g_pd3_w_reg[9] ==	0X01A0){               // 开灯
            g_load_mode.led_stastus_flag = 1;  	
        }
        else if (g_pd3_w_reg[9] ==	0X015F){               // 关灯
  	        g_load_mode.led_stastus_flag = 0; 
			  }
			  else if (g_pd3_w_reg[9] ==	0X0300){                // 定时模式
            g_load_mode.current_work_mode = 3;	
					  #ifdef _DBUG
                test_timer_power_task();
            #endif					
			  }
    }
    if (g_lora_protocol_rec.reg_w == PD3_CMD_TEST_POWER){   //手动调灯模式
        g_load_mode.current_work_mode = 2;	  
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
    if (g_load_mode.current_work_mode == 1){        //自动亮灯模式     
        if (s_light_value_r > 1000){
            g_load_mode.led_stastus_flag = 1;    
        }
        if (s_light_value_r < 920){                
            g_load_mode.led_stastus_flag = 0;		
        }	
    }
    if (g_load_mode.current_work_mode == 2){       //调灯模式
			  adjust_light_func(g_load_mode.light_power);	
    }	
		if (g_load_mode.current_work_mode == 3){       //定时模式 - 完成后进入调灯模式
		    uint16_t start_time_h;
        uint16_t start_time_m;

			  uint16_t end_time_h;
        uint16_t end_time_m;
			
			  static uint8_t s_step = 100;
			  static struct rtc_time s_start_tm;
			 
			  static uint16_t dt;
			
        start_time_h = (g_timer_mode.turn_on_data_time & 0XFF00) >> 8;
        start_time_m = (g_timer_mode.turn_on_data_time & 0X00FF) ;
      
        end_time_h = (g_timer_mode.turn_off_data_time & 0XFF00) >> 8;
        end_time_m = (g_timer_mode.turn_off_data_time & 0X00FF) ;
			
			  #ifdef _DBUG
           // printf ("ch-%d cm-%d\r\n", g_tm.tm_hour, g_tm.tm_min);	
			     // printf ("start_time_h = %d, start_time_m = %d, end_time_h = %d, end_time_m = %d\r\n",start_time_h, start_time_m, end_time_h, end_time_m);
				#endif
			  
           
//        if((g_tm.tm_hour  >=  start_time_h) && (g_tm.tm_min >= start_time_m)){
//			      g_timer_mode.mode_flag = 1;
//			  }
			  if((g_tm.tm_hour  >=  end_time_h) && (g_tm.tm_min >= end_time_m)){
			      g_timer_mode.mode_flag = 0;
            s_step = 100;					
			  }
				
			  if (g_timer_mode.mode_flag == 1){
				    switch (s_step){
							  case 100:
								    s_start_tm = 	g_tm;
								    if (g_tm.tm_hour == 0){
										    s_step = 100;	
										}
										else{
										    s_step = 0;	
										}
								    
                    #ifdef _DBUG
									      printf("step100\r\n");
								        //printf ("s_start_tm.min  hour = %d ,%d\r\n",s_start_tm.tm_min, s_start_tm.tm_hour);
								    #endif								
								    break;
						    case 0:	
                    adjust_light_func(g_timer_mode.load_power1);							    
                    dt = get_rtc_time_interval_min(&s_start_tm);
								    
    								#ifdef _DBUG
								        //printf ("s_start_tm.min = %d\r\n",s_start_tm.tm_min);
								        //printf("dt = %d\r\n", dt);
									      printf("step0\r\n");
								    #endif
                    if (dt >= g_timer_mode.load_time1){
										    s_step = 1; 
											  s_start_tm = g_tm;	
										}
                    else{
										    s_step = 0; 
										}
										break;
                case 1:
                    #ifdef _DBUG
									      printf("step1\r\n");
								    #endif									
                    adjust_light_func(g_timer_mode.load_power2);
								    dt = get_rtc_time_interval_min(&s_start_tm);
                    if (dt >= g_timer_mode.load_time2){
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
    								#ifdef _DBUG
								        //printf("dt = %d\r\n", dt);
									      printf("step2\r\n");
								    #endif	
                    if (dt >= g_timer_mode.load_time3){
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
								    #ifdef _DBUG
								        printf("dt = %d\r\n", dt);
									      printf("step3\r\n");
								    #endif	
                    if (dt >= g_timer_mode.load_time4){
										    s_step = 4; 
											  s_start_tm = g_tm;
										}
                    else{
										    s_step = 3; 
										}
										break;
							  case 4:
									  #ifdef _DBUG
									      printf("step4\r\n");
								    #endif									
                    adjust_light_func(g_timer_mode.load_power5);
								    dt = get_rtc_time_interval_min(&s_start_tm);
                    if (dt >= g_timer_mode.load_time5){
										    s_step = 5;
											  s_start_tm = g_tm;
										}
                    else{
										    s_step = 4; 
										}
										break;
							  case 5:
									  #ifdef _DBUG
									      printf("step5\r\n");
								    #endif									
                    adjust_light_func(g_timer_mode.load_power6);
								    dt = get_rtc_time_interval_min(&s_start_tm);
                    if (dt >= g_timer_mode.load_time6){
										    s_step = 7;
                        s_start_tm = g_tm;											
										}
                    else{
										    s_step = 6; 
										}
										break;
                case 6:
									  #ifdef _DBUG
									      printf("step6\r\n");
								    #endif									
                    adjust_light_func(g_timer_mode.load_power7);
								    dt = get_rtc_time_interval_min(&s_start_tm);
                    if (dt >= g_timer_mode.load_time7){
										    s_step = 8;
                        s_start_tm = g_tm;											
										}
                    else{
										    s_step = 7; 
										}
										break;
                case 7:
									  #ifdef _DBUG
									      printf("step7\r\n");
								    #endif									
                    adjust_light_func(g_timer_mode.load_power8);
								    dt = get_rtc_time_interval_min(&s_start_tm);
                    if (dt >= g_timer_mode.load_time8){
										    s_step = 8;
											  s_start_tm = g_tm;
										}
                    else{
										    s_step = 7; 
										}
										break;
                case 8:
									  #ifdef _DBUG
									      printf("step8\r\n");
								    #endif									
                    adjust_light_func(g_timer_mode.load_power9);
								    dt = get_rtc_time_interval_min(&s_start_tm);
                    if (dt >= g_timer_mode.load_time9){
										    s_step = 9;
                        s_start_tm = g_tm;											
                     }
                     else{
										        s_step = 8; 
										 }
										 break;										
                case 9:
									  #ifdef _DBUG
									      printf("step9\r\n");
								    #endif									
                    adjust_light_func(g_timer_mode.load_power10);
								    dt = get_rtc_time_interval_min(&s_start_tm);
                    if (dt >= g_timer_mode.load_time10){
										    s_step = 100;
											  g_timer_mode.mode_flag = 0;
                        s_start_tm = g_tm;											
										}
                    else{
										    s_step = 9; 
										}
										break;
                default:
									  s_step = 100;
								    s_start_tm = g_tm;
                    break;									
						}
				}       	   	
		}
		
		if (g_load_mode.patrol_time_flag == 1){        //设置上传时间
        g_lora_trans_count1 = g_load_mode.patrol_time_value;
		    g_load_mode.patrol_time_flag  = 0; 			
		}
		
		light_on_off_task_thread();
		
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
	  uint16_t test_power_temp_pwm;
	  uint16_t test_power_temp_vol;

    g_load_mode.light_power = light_value;

    test_power_temp_pwm = 50000 - g_load_mode.light_power * 500;
    tim4_set_pwm_duty(test_power_temp_pwm);	

    test_power_temp_vol  = g_load_mode.light_power * 20;
    dac2_set_vol(test_power_temp_vol);

	  if (g_load_mode.light_power < 1){
			  g_load_mode.led_stastus_flag = 0;
		}
		else{
			  g_load_mode.led_stastus_flag = 1;
		}
}


/*
 * 函数名：light_on_off_task
 * 描述  ：开关灯操作
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void light_on_off_task_thread()
{
	  uint16_t dt_min = 0;
		if (g_load_mode.led_stastus_flag == 1){
		    RELAY(0);	          
		}
		else{
		    RELAY(1);
			  g_load_mode.light_power = 0;			 	  
		}	
		
	  dt_min = get_rtc_time_interval_min(&s_start_read_rom_tm);
		
		if (dt_min > 1440){                                                //一天存一次     		
				write_eepom_data(g_load_mode.work_time_total, LIGHE_TIME_ADD); 
        delay_ms(20);			
        write_eepom_data(g_load_mode.use_energy_total_temp, LIGHE_POWER_ADD);		
			  s_start_read_rom_tm = g_tm;
		}
}

/*
 * 函数名：send_lora_time_and_cal_total_time
 * 描述  ：计算发送时间
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void send_lora_time_and_cal_total_time()
{
    uint16_t dt;
    g_task_current_min = g_tm.tm_min;                              //计算上传时间间隔
		if (g_task_min_count1 == 0){
		    g_task_last_min = g_tm.tm_min;
		}
		if (g_task_min_count1 ==0){                                   //第一次赋值
		    g_task_min_count1 = 1;
		}
	  if ((g_task_current_min < g_task_last_min) ){
        dt = g_task_current_min +  60 - g_task_last_min;
		}
		else{
				dt = g_task_current_min - g_task_last_min;
		}
		if (dt >= g_lora_trans_count1){
				g_task_min_count1_flag = 1;                             //发送完置零
        g_task_last_min = g_task_current_min;	
        if (g_load_mode.led_stastus_flag  == 1){               //灯开启时计时
				    g_load_mode.work_time_tonight = g_load_mode.work_time_tonight + g_lora_trans_count1;
            g_load_mode.work_time_total = g_load_mode.work_time_total + g_lora_trans_count1;
					  
					  g_load_mode.use_energy_tonight_temp = g_load_mode.use_energy_tonight_temp + g_lora_trans_count1 * cs_rn8209_data.PubData.Pw / 60;         // w/h
            g_load_mode.use_energy_total_temp = g_load_mode.use_energy_total_temp + g_lora_trans_count1 * cs_rn8209_data.PubData.Pw  / 60;             // w/h
		        g_load_mode.use_energy_tonight = (uint16_t) (g_load_mode.use_energy_tonight_temp / 1000);
					  g_load_mode.use_energy_total = (uint32_t) (g_load_mode.use_energy_total_temp / 1000);
				}
        else{
					  g_load_mode.work_time_tonight = 0; 
            g_load_mode.use_energy_tonight_temp = 0;					
				}											
		}
}

/*
 * 函数名：
 * 描述  ：计算发送时间
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void fault_detect_thread()
{
    static uint32_t s_rn8209_ia_old;
	  static uint32_t s_rn8209_ia_count;
    uint32_t s_rn8209_ia_err_temp;
	  
    if (cs_rn8209_data.PubData.U < POWER_SUPPLY_ANOMALY_MIN){     //电压小于最低值
		    g_fault_detect_flag = 1;
		}
		if (cs_rn8209_data.PubData.U < POWER_SUPPLY_ANOMALY_MIN){     //电压大于最大值
		    g_fault_detect_flag = 2;
		}
		//灯亮度大于10，但电流值小于100ma                            //灯坏或者驱动器坏
	  if ((cs_rn8209_data.PubData.Ia < 100) && g_load_mode.light_power > 10){
		    g_fault_detect_flag = 3;	
		}
		
		//电流不停的跳变则说明灯在闪
		s_rn8209_ia_err_temp = abs(cs_rn8209_data.PubData.Ia - s_rn8209_ia_old);
	  if (s_rn8209_ia_err_temp > 100){
		    s_rn8209_ia_count ++ ;  
		}
		if (s_rn8209_ia_count > 20){
		    s_rn8209_ia_count = 0;
			  g_fault_detect_flag = 4;
		}
	  if (g_load_mode.led_stastus_flag =0){
		    s_rn8209_ia_count = 0;
		}
		
	  s_rn8209_ia_old = cs_rn8209_data.PubData.Ia;
		
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
void test_time_task()
{
//	  delay_ms(60000);
//	  eepom_test();
	  s_set_tm.tm_year = 2018;
    s_set_tm.tm_mon = 1;
    s_set_tm.tm_mday = 26;
    s_set_tm.tm_hour = 11;
    s_set_tm.tm_min = 00;
		s_set_tm.tm_sec = 30;
	
    rtc_set_time(&s_set_tm);
//    time_thread(&g_tm);	
}

/*
 * 函数名：test_timer_power_task
 * 描述  ：测试函数
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void test_timer_power_task()
{
    g_timer_mode.mode_flag = 1;
    BYTE1(g_timer_mode.turn_on_data_time) = g_tm.tm_hour;
	  BYTE0(g_timer_mode.turn_on_data_time) = g_tm.tm_min;
	
    BYTE1(g_timer_mode.turn_off_data_time) = g_tm.tm_hour;
	  BYTE0(g_timer_mode.turn_off_data_time) = g_tm.tm_min + 10;
	
	
    g_timer_mode.load_time1 = 1;
    g_timer_mode.load_power1 = 10;
    
    g_timer_mode.load_time2 = 1;
    g_timer_mode.load_power2 = 20;
    
    g_timer_mode.load_time3 = 1;
    g_timer_mode.load_power3 = 30; 

    g_timer_mode.load_time4 =  1; 
    g_timer_mode.load_power4 = 40; 

    g_timer_mode.load_time5 =  1; 
    g_timer_mode.load_power5 = 50; 

    g_timer_mode.load_time6 = 1; 
    g_timer_mode.load_power6 = 60; 

    g_timer_mode.load_time7 = 1; 
    g_timer_mode.load_power7 = 70; 
		
		g_timer_mode.load_time8 = 1; 
    g_timer_mode.load_power8 = 80; 
		
		g_timer_mode.load_time9 = 1; 
    g_timer_mode.load_power9 = 90; 
		
		g_timer_mode.load_time10 = 1; 
    g_timer_mode.load_power10 = 100; 	
}
