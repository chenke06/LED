/*******************************************************************************
 * 文件名  ：rtc.c
 * 硬件连接：-------------------
 *          |                   |
 *          | VBAT引脚需外接电池|
 *          |                   |
 *           -----------------
 * 库版本  ：ST3.5.0
 * 作者    ：保留
**********************************************************************************/
#include "rtc.h"
#include "usart2.h"
/* 秒中断标志，进入秒中断时置1，当时间被刷新之后清0 */
__IO uint8_t g_rtc_count_irq = 0;
struct rtc_time g_tm;
/*
 * 函数名：rtc_nvic_configuration
 * 描述  ：配置RTC秒中断的主中断优先级为2，次优先级为0
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 * 2个优先级 抢占优先级和响应优先级
 */
void rtc_nvic_configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
 * 函数名：rtc_init
 * 描述  ：检查并配置RTC
 * 输入  ：用于读取RTC时间的结构体指针
 * 输出  ：无
 * 调用  ：外部调用
 */
void rtc_init(struct rtc_time *tm)
{
    rtc_check_config(tm);
}
/*
 * 函数名：rtc_set_time
 * 描述  ：检查并配置RTC
 * 输入  ：用于读取RTC时间的结构体指针
 * 输出  ：无
 * 调用  ：外部调用
 */
void rtc_set_time(struct rtc_time *set_tm)
{
        rtc_configuration();
        //printf("\r\n\r\n RTC configured....");	
        /* Adjust time by users typed on the hyperterminal */
        time_adjust(&g_tm, set_tm);
        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
}

/*
 * 函数名：rtc_check_config
 * 描述  ：检查并配置RTC
 * 输入  ：用于读取RTC时间的结构体指针
 * 输出  ：无
 * 调用  ：外部调用
 */
void rtc_check_config(struct rtc_time *tm)
{
//    /*在启动时检查备份寄存器BKP_DR1，如果内容不是0xA5A5,
//    则需重新配置时间并询问用户调整时间*/
//    if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5){
//        printf("\r\n\r\n RTC not yet configured....");
//        /* RTC Configuration */
        rtc_configuration();
//        printf("\r\n\r\n RTC configured....");	
//        /* Adjust time by users typed on the hyperterminal */
//        time_adjust(tm);
//        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
//    }
//    else
    {
        /*启动无需设置新时钟*/
        /*检查是否掉电重启*/
        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET){
            printf("\r\n\r\n Power On Reset occurred....");
        }
        /*检查是否Reset复位*/
        else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET){
            printf("\r\n\r\n External Reset occurred....");
        }
        printf("\r\n No need to configure RTC....");
        /*等待寄存器同步*/
        RTC_WaitForSynchro();
        /*允许RTC秒中断*/
        RTC_ITConfig(RTC_IT_SEC, ENABLE);
        /*等待上次RTC寄存器写操作完成*/
        RTC_WaitForLastTask();
    }
    /*定义了时钟输出宏，则配置校正时钟输出到PC13*/
    #ifdef RTCClockOutput_Enable
    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);
	
    /* Disable the Tamper Pin */
    BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
	                                 functionality must be disabled */
    /* Enable RTC Clock Output on Tamper Pin */
    BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
    #endif	
    /* Clear reset flags */
    RCC_ClearFlag();
}

/*
 * 函数名：rtc_configuration
 * 描述  ：配置RTC
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void rtc_configuration(void)
{
    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);
	
    /* Reset Backup Domain */
    BKP_DeInit();
	
    /* Enable LSE */
    RCC_LSEConfig(RCC_LSE_ON);
    /* Wait till LSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {}
    /* Select LSE as RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    /* Enable RTC Clock */
    RCC_RTCCLKCmd(ENABLE);
    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    /* Enable the RTC Second */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    /* Set RTC prescaler: set RTC period to 1sec */
    RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
}

/*
* 函数名  : usart_scanf
* 描述    : 从微机超级终端获取数字值(把ASCII码转换为数字)
*			      本函数专用于RTC获取时间，若进行其它输入应用，要修改一下
* 输入    : - value 用户在超级终端中输入的数值
* 输出    : 输入字符的ASCII码对应的数值
* 调用    ：内部调用
*/

static uint8_t usart_scanf(uint32_t value)
{
    uint32_t index = 0;
	  uint32_t tmp[2] = {0, 0};
	
	  while (index < 2){
        /* Loop until RXNE = 1 */
        while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET){
        }
        tmp[index++] = (USART_ReceiveData(USART2));
        /*数字0到9的ASCII码为0x30至0x39*/
        if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39)){ 		  
            printf("\n\rPlease enter valid number between 0 and 9 -->:  ");
            index--;
        }
    }
    /* 计算输入字符的ASCII码转换为数字*/
    index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
    /* Checks */
	  if (index > value){
        printf("\n\rPlease enter valid number between 0 and %d", value);
        return 0xFF;
    }
    return index;
}

/*
 * 函数名：time_regulate
 * 描述  ：返回用户在超级终端中输入的时间值，并将值储存在
 *         RTC 计数寄存器中。
 * 输入  ：用于读取RTC时间的结构体指针
 * 输出  ：用户在超级终端中输入的时间值，单位为 s
 * 调用  ：内部调用
 */
void time_regulate(struct rtc_time *tm, struct rtc_time *set_tm)
{
	  tm->tm_year = set_tm->tm_year;
    tm->tm_mon = set_tm->tm_mon;
	  tm->tm_mday = set_tm->tm_mday;
	  tm->tm_hour = set_tm->tm_hour;
	  tm->tm_min = set_tm->tm_min;
	  tm->tm_sec = set_tm->tm_sec;
}

/*
 * 函数名：time_show
 * 描述  ：在超级终端中显示当前时间值
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */ 
void time_thread(struct rtc_time *tm)
{
        /* 每过1s */
        if (g_rtc_count_irq == 1){
            /* Display current time */
            time_display(RTC_GetCounter(), tm); 		  
            g_rtc_count_irq = 0;
        }
}

/*
 * 函数名：time_adjust
 * 描述  ：时间调节
 * 输入  ：用于读取RTC时间的结构体指针
 * 输出  ：无
 * 调用  ：外部调用
 */
void time_adjust(struct rtc_time *tm, struct rtc_time *set_tm)
{
    /* Wait until last write operation on RTC registers has finished */
	  RTC_WaitForLastTask();
	
	  /* Get time entred by the user on the hyperterminal */
	  time_regulate(tm, set_tm);
	  
	  /* Get wday */
	  gregorianday(tm);

	  /* 修改当前RTC计数寄存器内容 */
	  RTC_SetCounter(mktimev(tm));

	  /* Wait until last write operation on RTC registers has finished */
	  RTC_WaitForLastTask();
}

/*
 * 函数名：time_display
 * 描述  ：显示当前时间值
 * 输入  ：-time_var RTC计数值，单位为 s
 * 输出  ：无
 * 调用  ：内部调用
 */	
void time_display(uint32_t time_var, struct rtc_time *tm)
{
    uint32_t bj_time_var;
    uint8_t str[15]; // 字符串暂存  	

    /*  把标准时间转换为北京时间*/
    bj_time_var = time_var + 8*60*60;

    to_tm(bj_time_var, tm);/*把定时器的值转换为北京时间*/	
	  	
    /* 输出时间戳，公历时间 */
//	  printf("\r 当前时间为: %d年 %d月 %d日   %0.2d:%0.2d:%0.2d %0.2d\r\n",
//	                    tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, 
//	                    tm->tm_min, tm->tm_sec, tm->tm_wday);
}

/*
 * 函数名：updata 
 * 描述  ：更新当前时间
 * 输入  ：-time_var                                                                                                RTC计数值，单位为 s
 * 输出  ：无
 * 调用  ：内部调用
 */	
void updata_time_data(struct rtc_time *updata_time)
{
    updata_time->tm_year = g_tm.tm_year;
    updata_time->tm_mon = g_tm.tm_mon;
    updata_time->tm_mday = g_tm.tm_year;
    updata_time->tm_hour = g_tm.tm_hour;
    updata_time->tm_min = g_tm.tm_min;
    updata_time->tm_sec = g_tm.tm_sec;
    updata_time->tm_wday = g_tm.tm_wday;   
}
/*
 * 函数名：gregorianday
 * 描述  ：计算公历
 * 输入  ：
 * 输出  ：
 * 调用  ：内部调用
 */	
void gregorianday(struct rtc_time * tm)
{
    int leapstodate;
    int lastyear;
    int day;
    int monthoffset[] = { 0,31,59,90,120,151,181,212,243,273,304,334 };

    lastyear = tm->tm_year - 1;

    /*计算从公元元年到计数的前一年之中一共经历了多少个闰年*/
    leapstodate = lastyear / 4 - lastyear / 100 + lastyear / 400;      

    /*如若计数的这一年为闰年，且计数的月份在2月之后，则日数加1，否则不加1*/
    if((tm->tm_year%4==0) &&
	   ((tm->tm_year%100!=0) || (tm->tm_year%400==0)) &&
	   (tm->tm_mon>2)) {
        day = 1;
    }
    else{
		    day = 0;
    }

    day += lastyear * 365 + leapstodate + monthoffset[tm->tm_mon-1] + tm->tm_mday; /*计算从公元元年元旦到计数日期一共有多少天*/

    tm->tm_wday=day%7;
}

/*
 * 函数名：to_tm
 * 描述  ：把定时器的值转换为北京时间
 * 输入  ：
 * 输出  ：所以在转换成时间戳时最后要从北京时间转换为标准时间的时间戳
 * 调用  ：内部调用
 */	

u32 mktimev(struct rtc_time *tm)
{
    if (0 >= (int) (tm->tm_mon -= 2)){	/* 1..12 -> 11,12,1..10 */
        tm->tm_mon += 12;		/* Puts Feb last since it has leap day */
        tm->tm_year -= 1;
    }
    return (((
        (u32) (tm->tm_year/4 - tm->tm_year/100 + tm->tm_year/400 + 367*tm->tm_mon/12 + tm->tm_mday) +
        tm->tm_year*365 - 719499
        )*24 + tm->tm_hour /* now have hours */
        )*60 + tm->tm_min /* now have minutes */
        )*60 + tm->tm_sec-8*60*60; /* finally seconds */
    /*Add by fire: -8*60*60 把输入的北京时间转换为标准时间，
    再写入计时器中，确保计时器的数据为标准的UNIX时间戳*/	 
}
/*
 * 函数名：to_tm
 * 描述  ：把定时器的值转换为北京时间
 * 输入  ：
 * 输出  ：无
 * 调用  ：内部调用
 */	
void to_tm(u32 tim, struct rtc_time * tm)
{
    register u32    i;
    register long   hms, day;

    day = tim / SECDAY;
    hms = tim % SECDAY;

    /* Hours, minutes, seconds are easy */
    tm->tm_hour = hms / 3600;
    tm->tm_min = (hms % 3600) / 60;
    tm->tm_sec = (hms % 3600) % 60;

    /* Number of years in days */ /*算出当前年份，起始的计数年份为1970年*/
    for (i = STARTOFTIME; day >= days_in_year(i); i++) {
        day -= days_in_year(i);
    }
    tm->tm_year = i;

    /* Number of months in days left */ /*计算当前的月份*/
    if (leapyear(tm->tm_year)) {
		    days_in_month(FEBRUARY) = 29;
    }
    for (i = 1; day >= days_in_month(i); i++) {
        day -= days_in_month(i);
    }
    days_in_month(FEBRUARY) = 28;
    tm->tm_mon = i;

    /* Days are what is left over (+1) from all that. *//*计算当前日期*/
    tm->tm_mday = day + 1;


    // Determine the day of week
    gregorianday(tm);
	
}

/*
 * 函数名：得到rtc的时间间隔
 * 描述  ：
 * 输入  : 初始时间
 * 输出  ：min
 * 调用  ：内部调用
 */
uint16_t get_rtc_time_interval_min(struct rtc_time *start_tm)
{
    uint16_t dt;
	  uint8_t current_min = g_tm.tm_min; 
    uint8_t current_hour = g_tm.tm_hour;                  //计算上传时间间隔
	  uint8_t dt_hour;
	  uint8_t dt_min;
    uint16_t total_dt_min  = 0;
	  
    dt_hour =  current_hour - start_tm -> tm_hour;
    if (dt_hour < 0){
	      dt_hour = dt_hour + 24;  
		}
		
	  dt_min = current_min - start_tm -> tm_min;
		if (dt_min < 0){
        dt_min = dt_min + 60;   
		}

    total_dt_min = dt_hour * 60 + dt_min;
		
		return 	total_dt_min;
}
