/****************************************************************************
 * �ļ���  ��task_schedul.c
 * ����    ������Ӧ�ú����� 
 * Timer1 --- �����������Ƶ��
 * Timer2 --- ������ȶ�ʱ��
 * Timer3 --- PWM ���
 * ��汾  ��ST3.5.0  
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

//RTC ʱ�����
static struct rtc_time s_set_tm;
static struct rtc_time s_start_read_rom_tm;  //��romֵʱ�����
 
//�����ϴ�ʱ��������
int16_t g_task_min_count1 = 0;            //��һ��ʱ�丳ֵ��־
int16_t g_task_last_min = 0;              //�ϴ�ʱ����
int16_t g_task_current_min = 0;           //��ǰʱ��
int16_t g_lora_trans_count1 = 1;          //�´����ϴ�ʱ������Ĭ��Ϊ1min
int16_t g_task_min_count1_flag = 0;       //ʱ���������־λ

//���ϼ��״̬λ
uint32_t g_fault_detect_flag = 0;

/*
 * ��������task_init
 * ����  ������ĳ�ʼ��
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
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
 * ��������task_nvic_init
 * ����  �����������ж����ȼ���ʼ��
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */
void task_nvic_init(void)
{
    usart1_nvic_configuration();  //1-0
    tim2_nvic_configuration();    //3-0
    usart3_nvic_configuration();  //4-0
    rtc_nvic_configuration();     //0 - 0 
}
/*
 * ��������task_schedul
 * ����  ���������
         : #define FIRST_TASK_SPEED  3
           #define SECOND_TASK_SPEED 20
           #define THIRD_TASK_SPEED  500
           #define FOURTH_TASK_SPEED 750
           #define FIFTH_TASK_SPEED  1000
           #define SIXTH_TASK_SPEED  3000
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */
//�����ò���
#ifdef _DBUG
    RN8209_Front_TypeDef cs_rn8209_data;
    static float s_temp_value_r;
    static float s_light_value_r;
#endif

void task_schedul(void)
{
    if (g_task_state_flag.first == 1){                    //3ms����   lora �Ĵ�������
        updata_lora_reg_data_thread();                    //���¶�д�Ĵ���
        lora_analysis_thread();                           //һ֡���ݽ�����ɺ󣬷������ݣ��Լ����յ������ݷŵ�������
        g_task_state_flag.first = 0;
    }
	  if (g_task_state_flag.second == 1){                 //20ms���� 
        g_task_state_flag.second = 0;
    }
	  if (g_task_state_flag.third== 1){                   //500ms���� ,ʱ�� ��ȡ�޸�       
       if (g_lora_protocol_rec.w_reg_updata == 1){    
            if (g_lora_protocol_rec.reg_w == PD0_SYS_DATA_TIME){  //lora ʱ���޸ģ��˰汾ֻ��ͬʱ�޸�               			  
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
	  if (g_task_state_flag.fourth == 1){                //750ms ����
        updata_light_value(&s_light_value_r);          //��ȡ����ֵ 
        updata_ntc_value(&s_temp_value_r);	           //��ȡ�¶�ֵ   			
				lora_cmd_task_exe ();                          //lora ����ִ��
			  g_task_state_flag.fourth = 0;
    }		
	  if (g_task_state_flag.fifth == 1){                 //1s����
       if (g_lora_protocol_rec.w_reg_updata == 1){    
            lora_send_exe_cmd_analy();                 //lora ģʽ����
				    g_lora_protocol_rec.w_reg_updata = 0;	
				}
				if (g_task_min_count1_flag){                  //�ϴ�״̬����
					  g_task_min_count1_flag = 0; 
            lora_send_timer();		   
				}
				fault_detect_thread();
        g_task_state_flag.fifth = 0;
    }
		
    if (g_task_state_flag.sixth == 1){                //3s ��ȡ��������      		
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
 * ���������õ���lora��ʱ����
 * ����  ��
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */
void get_updata_lora_time_interval()
{
    uint16_t dt;
    g_task_current_min = g_tm.tm_min;                  //�����ϴ�ʱ����
    if (g_task_min_count1 == 0){
        g_task_last_min = g_tm.tm_min;
    }
    if (g_task_min_count1 ==0){                        //��һ�θ�ֵ
        g_task_min_count1 = 1;
    }
    if ((g_task_current_min < g_task_last_min) ){
        dt = g_task_current_min +  60 - g_task_last_min;
    }
    else{
        dt = g_task_current_min - g_task_last_min;
    }
    if (dt >= g_lora_trans_count1){                  //�����ϴ�ʱ����ñ�־λ
        g_task_min_count1_flag = 1;
        g_task_last_min = g_task_current_min;							
    }
}

/*
 * ��������lora_send_exe_cmd_analy
 * ����  ��loraִ���������
           g_load_mode.current_work_mode = 1  ���ģʽ
           g_load_mode.current_work_mode = 2  ����ģʽ
           g_load_mode.current_work_mode = 3  ��ʱģʽ 

 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */
void lora_send_exe_cmd_analy()
{
	  
	  if (g_lora_protocol_rec.reg_w == PD3_CMD_POWER_ONOFF){    //��������
        if (g_pd3_w_reg[0] == 1){                             //����
				     LED0(0);    
	           LED1(0);
				}
				else{                                                 //�ϵ�
						 LED0(1);
	           LED1(1);
					   delay_ms(6000);
					   LED0(0);    
	           LED1(0);
					
				}
    }
    if (g_lora_protocol_rec.reg_w == PD3_CMD_RESET){           //��λ
        __set_FAULTMASK(1);
        NVIC_SystemReset();
    }
		if (g_lora_protocol_rec.reg_w == PD3_CMD_RESTORE_FACTORY){  //�ָ���������
		    write_eepom_data(0, LIGHE_POWER_ADD);
		    delay_ms(20);
		    write_eepom_data(0, LIGHE_TIME_ADD);
		}
    if (g_lora_protocol_rec.reg_w == PD3_CMD_LOAD){
        if (g_pd3_w_reg[9] ==	0X0000){                     //���ģʽ
     				g_load_mode.current_work_mode = 1;					
		    }
        else if (g_pd3_w_reg[9] ==	0X01A0){               // ����
            g_load_mode.led_stastus_flag = 1;  	
        }
        else if (g_pd3_w_reg[9] ==	0X015F){               // �ص�
  	        g_load_mode.led_stastus_flag = 0; 
			  }
			  else if (g_pd3_w_reg[9] ==	0X0300){                // ��ʱģʽ
            g_load_mode.current_work_mode = 3;	
					  #ifdef _DBUG
                test_timer_power_task();
            #endif					
			  }
    }
    if (g_lora_protocol_rec.reg_w == PD3_CMD_TEST_POWER){   //�ֶ�����ģʽ
        g_load_mode.current_work_mode = 2;	  
        g_load_mode.light_power = g_pd3_w_reg[10];							
    }
		if (g_lora_protocol_rec.reg_w == PD3_CMD_PATROL_TIME){
        g_load_mode.patrol_time_flag  = 1; 
			  g_load_mode.patrol_time_value = g_pd3_w_reg[10];	
		}
		
}

/*
 * ��������lora_cmd_task_exe
 * ����  ��lora��ִ�в���
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */
void lora_cmd_task_exe ()
{
    if (g_load_mode.current_work_mode == 1){        //�Զ�����ģʽ     
        if (s_light_value_r > 1000){
            g_load_mode.led_stastus_flag = 1;    
        }
        if (s_light_value_r < 920){                
            g_load_mode.led_stastus_flag = 0;		
        }	
    }
    if (g_load_mode.current_work_mode == 2){       //����ģʽ
			  adjust_light_func(g_load_mode.light_power);	
    }	
		if (g_load_mode.current_work_mode == 3){       //��ʱģʽ - ��ɺ�������ģʽ
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
		
		if (g_load_mode.patrol_time_flag == 1){        //�����ϴ�ʱ��
        g_lora_trans_count1 = g_load_mode.patrol_time_value;
		    g_load_mode.patrol_time_flag  = 0; 			
		}
		
		light_on_off_task_thread();
		
}

/*
 * ��������adjust_light_func
 * ����  ���������
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
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
 * ��������light_on_off_task
 * ����  �����صƲ���
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
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
		
		if (dt_min > 1440){                                                //һ���һ��     		
				write_eepom_data(g_load_mode.work_time_total, LIGHE_TIME_ADD); 
        delay_ms(20);			
        write_eepom_data(g_load_mode.use_energy_total_temp, LIGHE_POWER_ADD);		
			  s_start_read_rom_tm = g_tm;
		}
}

/*
 * ��������send_lora_time_and_cal_total_time
 * ����  �����㷢��ʱ��
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */
void send_lora_time_and_cal_total_time()
{
    uint16_t dt;
    g_task_current_min = g_tm.tm_min;                              //�����ϴ�ʱ����
		if (g_task_min_count1 == 0){
		    g_task_last_min = g_tm.tm_min;
		}
		if (g_task_min_count1 ==0){                                   //��һ�θ�ֵ
		    g_task_min_count1 = 1;
		}
	  if ((g_task_current_min < g_task_last_min) ){
        dt = g_task_current_min +  60 - g_task_last_min;
		}
		else{
				dt = g_task_current_min - g_task_last_min;
		}
		if (dt >= g_lora_trans_count1){
				g_task_min_count1_flag = 1;                             //����������
        g_task_last_min = g_task_current_min;	
        if (g_load_mode.led_stastus_flag  == 1){               //�ƿ���ʱ��ʱ
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
 * ��������
 * ����  �����㷢��ʱ��
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */
void fault_detect_thread()
{
    static uint32_t s_rn8209_ia_old;
	  static uint32_t s_rn8209_ia_count;
    uint32_t s_rn8209_ia_err_temp;
	  
    if (cs_rn8209_data.PubData.U < POWER_SUPPLY_ANOMALY_MIN){     //��ѹС�����ֵ
		    g_fault_detect_flag = 1;
		}
		if (cs_rn8209_data.PubData.U < POWER_SUPPLY_ANOMALY_MIN){     //��ѹ�������ֵ
		    g_fault_detect_flag = 2;
		}
		//�����ȴ���10��������ֵС��100ma                            //�ƻ�������������
	  if ((cs_rn8209_data.PubData.Ia < 100) && g_load_mode.light_power > 10){
		    g_fault_detect_flag = 3;	
		}
		
		//������ͣ��������˵��������
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
 * ��������task_cs_schedul
 * ����  ���������ȵ��ڲ���
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
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
 * ��������test_task
 * ����  �����Ժ���
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
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
 * ��������test_timer_power_task
 * ����  �����Ժ���
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
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
