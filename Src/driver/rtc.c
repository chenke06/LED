/*******************************************************************************
 * �ļ���  ��rtc.c
 * Ӳ�����ӣ�-------------------
 *          |                   |
 *          | VBAT��������ӵ��|
 *          |                   |
 *           -----------------
 * ��汾  ��ST3.5.0
 * ����    ������
**********************************************************************************/
#include "rtc.h"
#include "usart2.h"
/* ���жϱ�־���������ж�ʱ��1����ʱ�䱻ˢ��֮����0 */
__IO uint8_t g_rtc_count_irq = 0;
struct rtc_time g_tm;
/*
 * ��������rtc_nvic_configuration
 * ����  ������RTC���жϵ����ж����ȼ�Ϊ2�������ȼ�Ϊ0
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 * 2�����ȼ� ��ռ���ȼ�����Ӧ���ȼ�
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
 * ��������rtc_init
 * ����  ����鲢����RTC
 * ����  �����ڶ�ȡRTCʱ��Ľṹ��ָ��
 * ���  ����
 * ����  ���ⲿ����
 */
void rtc_init(struct rtc_time *tm)
{
    rtc_check_config(tm);
}
/*
 * ��������rtc_set_time
 * ����  ����鲢����RTC
 * ����  �����ڶ�ȡRTCʱ��Ľṹ��ָ��
 * ���  ����
 * ����  ���ⲿ����
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
 * ��������rtc_check_config
 * ����  ����鲢����RTC
 * ����  �����ڶ�ȡRTCʱ��Ľṹ��ָ��
 * ���  ����
 * ����  ���ⲿ����
 */
void rtc_check_config(struct rtc_time *tm)
{
//    /*������ʱ��鱸�ݼĴ���BKP_DR1��������ݲ���0xA5A5,
//    ������������ʱ�䲢ѯ���û�����ʱ��*/
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
        /*��������������ʱ��*/
        /*����Ƿ��������*/
        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET){
            printf("\r\n\r\n Power On Reset occurred....");
        }
        /*����Ƿ�Reset��λ*/
        else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET){
            printf("\r\n\r\n External Reset occurred....");
        }
        printf("\r\n No need to configure RTC....");
        /*�ȴ��Ĵ���ͬ��*/
        RTC_WaitForSynchro();
        /*����RTC���ж�*/
        RTC_ITConfig(RTC_IT_SEC, ENABLE);
        /*�ȴ��ϴ�RTC�Ĵ���д�������*/
        RTC_WaitForLastTask();
    }
    /*������ʱ������꣬������У��ʱ�������PC13*/
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
 * ��������rtc_configuration
 * ����  ������RTC
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
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
* ������  : usart_scanf
* ����    : ��΢�������ն˻�ȡ����ֵ(��ASCII��ת��Ϊ����)
*			      ������ר����RTC��ȡʱ�䣬��������������Ӧ�ã�Ҫ�޸�һ��
* ����    : - value �û��ڳ����ն����������ֵ
* ���    : �����ַ���ASCII���Ӧ����ֵ
* ����    ���ڲ�����
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
        /*����0��9��ASCII��Ϊ0x30��0x39*/
        if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39)){ 		  
            printf("\n\rPlease enter valid number between 0 and 9 -->:  ");
            index--;
        }
    }
    /* ���������ַ���ASCII��ת��Ϊ����*/
    index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
    /* Checks */
	  if (index > value){
        printf("\n\rPlease enter valid number between 0 and %d", value);
        return 0xFF;
    }
    return index;
}

/*
 * ��������time_regulate
 * ����  �������û��ڳ����ն��������ʱ��ֵ������ֵ������
 *         RTC �����Ĵ����С�
 * ����  �����ڶ�ȡRTCʱ��Ľṹ��ָ��
 * ���  ���û��ڳ����ն��������ʱ��ֵ����λΪ s
 * ����  ���ڲ�����
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
 * ��������time_show
 * ����  ���ڳ����ն�����ʾ��ǰʱ��ֵ
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */ 
void time_thread(struct rtc_time *tm)
{
        /* ÿ��1s */
        if (g_rtc_count_irq == 1){
            /* Display current time */
            time_display(RTC_GetCounter(), tm); 		  
            g_rtc_count_irq = 0;
        }
}

/*
 * ��������time_adjust
 * ����  ��ʱ�����
 * ����  �����ڶ�ȡRTCʱ��Ľṹ��ָ��
 * ���  ����
 * ����  ���ⲿ����
 */
void time_adjust(struct rtc_time *tm, struct rtc_time *set_tm)
{
    /* Wait until last write operation on RTC registers has finished */
	  RTC_WaitForLastTask();
	
	  /* Get time entred by the user on the hyperterminal */
	  time_regulate(tm, set_tm);
	  
	  /* Get wday */
	  gregorianday(tm);

	  /* �޸ĵ�ǰRTC�����Ĵ������� */
	  RTC_SetCounter(mktimev(tm));

	  /* Wait until last write operation on RTC registers has finished */
	  RTC_WaitForLastTask();
}

/*
 * ��������time_display
 * ����  ����ʾ��ǰʱ��ֵ
 * ����  ��-time_var RTC����ֵ����λΪ s
 * ���  ����
 * ����  ���ڲ�����
 */	
void time_display(uint32_t time_var, struct rtc_time *tm)
{
    uint32_t bj_time_var;
    uint8_t str[15]; // �ַ����ݴ�  	

    /*  �ѱ�׼ʱ��ת��Ϊ����ʱ��*/
    bj_time_var = time_var + 8*60*60;

    to_tm(bj_time_var, tm);/*�Ѷ�ʱ����ֵת��Ϊ����ʱ��*/	
	  	
    /* ���ʱ���������ʱ�� */
//	  printf("\r ��ǰʱ��Ϊ: %d�� %d�� %d��   %0.2d:%0.2d:%0.2d %0.2d\r\n",
//	                    tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, 
//	                    tm->tm_min, tm->tm_sec, tm->tm_wday);
}

/*
 * ��������updata 
 * ����  �����µ�ǰʱ��
 * ����  ��-time_var                                                                                                RTC����ֵ����λΪ s
 * ���  ����
 * ����  ���ڲ�����
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
 * ��������gregorianday
 * ����  �����㹫��
 * ����  ��
 * ���  ��
 * ����  ���ڲ�����
 */	
void gregorianday(struct rtc_time * tm)
{
    int leapstodate;
    int lastyear;
    int day;
    int monthoffset[] = { 0,31,59,90,120,151,181,212,243,273,304,334 };

    lastyear = tm->tm_year - 1;

    /*����ӹ�ԪԪ�굽������ǰһ��֮��һ�������˶��ٸ�����*/
    leapstodate = lastyear / 4 - lastyear / 100 + lastyear / 400;      

    /*������������һ��Ϊ���꣬�Ҽ������·���2��֮����������1�����򲻼�1*/
    if((tm->tm_year%4==0) &&
	   ((tm->tm_year%100!=0) || (tm->tm_year%400==0)) &&
	   (tm->tm_mon>2)) {
        day = 1;
    }
    else{
		    day = 0;
    }

    day += lastyear * 365 + leapstodate + monthoffset[tm->tm_mon-1] + tm->tm_mday; /*����ӹ�ԪԪ��Ԫ������������һ���ж�����*/

    tm->tm_wday=day%7;
}

/*
 * ��������to_tm
 * ����  ���Ѷ�ʱ����ֵת��Ϊ����ʱ��
 * ����  ��
 * ���  ��������ת����ʱ���ʱ���Ҫ�ӱ���ʱ��ת��Ϊ��׼ʱ���ʱ���
 * ����  ���ڲ�����
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
    /*Add by fire: -8*60*60 ������ı���ʱ��ת��Ϊ��׼ʱ�䣬
    ��д���ʱ���У�ȷ����ʱ��������Ϊ��׼��UNIXʱ���*/	 
}
/*
 * ��������to_tm
 * ����  ���Ѷ�ʱ����ֵת��Ϊ����ʱ��
 * ����  ��
 * ���  ����
 * ����  ���ڲ�����
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

    /* Number of years in days */ /*�����ǰ��ݣ���ʼ�ļ������Ϊ1970��*/
    for (i = STARTOFTIME; day >= days_in_year(i); i++) {
        day -= days_in_year(i);
    }
    tm->tm_year = i;

    /* Number of months in days left */ /*���㵱ǰ���·�*/
    if (leapyear(tm->tm_year)) {
		    days_in_month(FEBRUARY) = 29;
    }
    for (i = 1; day >= days_in_month(i); i++) {
        day -= days_in_month(i);
    }
    days_in_month(FEBRUARY) = 28;
    tm->tm_mon = i;

    /* Days are what is left over (+1) from all that. *//*���㵱ǰ����*/
    tm->tm_mday = day + 1;


    // Determine the day of week
    gregorianday(tm);
	
}

/*
 * ���������õ�rtc��ʱ����
 * ����  ��
 * ����  : ��ʼʱ��
 * ���  ��min
 * ����  ���ڲ�����
 */
uint16_t get_rtc_time_interval_min(struct rtc_time *start_tm)
{
    uint16_t dt;
	  uint8_t current_min = g_tm.tm_min; 
    uint8_t current_hour = g_tm.tm_hour;                  //�����ϴ�ʱ����
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
