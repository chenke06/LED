#ifndef __RTC_H
#define	__RTC_H


#include "stm32f10x.h"
#include "usart2.h"

#define FEBRUARY		2
#define	STARTOFTIME		1970
#define SECDAY			86400L
#define SECYR			(SECDAY * 365)
#define	leapyear(year)		((year) % 4 == 0)
#define	days_in_year(a) 	(leapyear(a) ? 366 : 365)
#define	days_in_month(a) 	(month_days[(a) - 1])

static int month_days[12] = {	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

struct rtc_time {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
};

extern __IO uint8_t g_rtc_count_irq;
extern struct rtc_time g_tm;


/* 如果定义了下面这个宏的话,PC13就会输出频率为RTC Clock/64的时钟 */   
//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */

void time_thread(struct rtc_time *tm);
void time_adjust(struct rtc_time *tm, struct rtc_time *set_tm);
void time_display(uint32_t time_var, struct rtc_time *tm);
void time_regulate(struct rtc_time *tm, struct rtc_time *set_tm);
static uint8_t usart_scanf(uint32_t value);
void rtc_configuration(void);
void rtc_check_config(struct rtc_time *tm);
void rtc_nvic_configuration(void);
void to_tm(uint32_t tim, struct rtc_time * tm);
void gregorianday(struct rtc_time * tm);
u32 mktimev(struct rtc_time *tm);
void rtc_init(struct rtc_time *tm);
void updata_time_data(struct rtc_time *updata_time);
void rtc_set_time(struct rtc_time *set_tm);
uint16_t get_rtc_time_interval_min(struct rtc_time *start_tm);

#endif /* __XXX_H */
