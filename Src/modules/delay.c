/*******************************************************************************
 * 文件名  ：delay.c
 * 描述    ：delay应用函数库        
 * 库版本  ：ST3.5.0
**********************************************************************************/
#include "delay.h"

__IO uint32_t g_timing_delay;

/*
 * 函数名：sys_tick_init
 * 描述  ：
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void delay_init(void)
{
    //1us定时器
    if (SysTick_Config(SystemCoreClock/1000000)){
        while(1);
    }
    //SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;       //如果无法启动则关闭
}
/*
 * 函数名：delay_us
 * 描述  ：
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void delay_us(uint16_t ntime)
{
    g_timing_delay = ntime;
    while (g_timing_delay != 0);
}

/*
 * 函数名：delay_ms
 * 描述  ：
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void delay_ms(uint16_t ntime)
{
    uint16_t i;
     for (i = 0; i < ntime; i++){
         delay_us(1000);
     }
}

///*
// * 函数名：delay_us
// * 描述  ：
// * 输入  : 无
// * 输出  ：无
// * 调用  ：内部调用
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
// * 函数名：delay_ms
// * 描述  ：
// * 输入  : 无
// * 输出  ：无
// * 调用  ：内部调用
// */
//void delay_ms(u16 time)
//{    
//    u16 i = 0;  
//    while(time--){
//        i = 12000;
//        while(i--) ;    
//    }
//}

