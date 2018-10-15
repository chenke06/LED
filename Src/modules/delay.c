/*******************************************************************************
 * �ļ���  ��delay.c
 * ����    ��delayӦ�ú�����        
 * ��汾  ��ST3.5.0
**********************************************************************************/
#include "delay.h"

__IO uint32_t g_timing_delay;

/*
 * ��������sys_tick_init
 * ����  ��
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */
void delay_init(void)
{
    //1us��ʱ��
    if (SysTick_Config(SystemCoreClock/1000000)){
        while(1);
    }
    //SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;       //����޷�������ر�
}
/*
 * ��������delay_us
 * ����  ��
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */
void delay_us(uint16_t ntime)
{
    g_timing_delay = ntime;
    while (g_timing_delay != 0);
}

/*
 * ��������delay_ms
 * ����  ��
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */
void delay_ms(uint16_t ntime)
{
    uint16_t i;
     for (i = 0; i < ntime; i++){
         delay_us(1000);
     }
}

///*
// * ��������delay_us
// * ����  ��
// * ����  : ��
// * ���  ����
// * ����  ���ڲ�����
// */
//void delay_us(u16 time)
//{    
//    u16 i = 0;  
//    while (time--){
//        i = 10; 
//        while(i--);    
//    }
//}
///*
// * ��������delay_ms
// * ����  ��
// * ����  : ��
// * ���  ����
// * ����  ���ڲ�����
// */
//void delay_ms(u16 time)
//{    
//    u16 i = 0;  
//    while(time--){
//        i = 12000;
//        while(i--) ;    
//    }
//}

