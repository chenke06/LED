/**********************************************************************************
 * 文件名  ：dac1.c
 * 描述    ：dac1 底层应用函数库         
 * 硬件连接 ----------------------------
 *         | PA4           : DAC1       |
 *         |                            |
 *         |                            |
 *         |                            |
 *          ----------------------------
 * 库版本  ：ST3.5.0
**********************************************************************************/
#include "dac1.h"
#include "delay.h"
/*
 * 函数名：dac1_Init
 * 描述  ：
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 */
void dac1_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitType;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
//    GPIO_SetBits(GPIOA, GPIO_Pin_4);
    
    DAC_InitType.DAC_Trigger = DAC_Trigger_None;                        //不使用触发功能
    DAC_InitType.DAC_WaveGeneration = DAC_WaveGeneration_None;          //不使用波形发生
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;//屏蔽幅值设置 
    DAC_InitType.DAC_OutputBuffer = DAC_OutputBuffer_Disable ;          //DAC1输出缓存关闭BOFF1=1
    DAC_Init(DAC_Channel_1, &DAC_InitType);                              
    
    DAC_Cmd(DAC_Channel_1, ENABLE);
    DAC_SetChannel1Data(DAC_Align_12b_R, 0);                             //12位右对齐数据格式设置DAC值
    delay_ms(1);
		#ifdef PWM_MODE
    dac1_set_vol(2000);
		#else
    dac1_set_vol(0);
		#endif
}

/*
 * 描述  ：
 * 函数名：设置通道1输出电压
 * 输入  ： 0~3300 
 * 输出  ： 0~3.3V
 * 调用  ：内部调用
 */
void dac1_set_vol(u16 vol)
{
    float temp = vol;
    temp /= 1000;
    temp = temp * 4096 / 2.5;
    DAC_SetChannel1Data(DAC_Align_12b_R,temp);
}
