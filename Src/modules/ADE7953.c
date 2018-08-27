/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
 * 文件名  ：adc.c
 * 描述    ：adc应用函数库        
 * 硬件连接：----------------------------------------------------
 *          |                                                    |
 *          | PC0 - ADC123_IN10
 *          | PC1 - ADC123_IN11
            | PC2 - ADC123_IN12                                  |
 *           ----------------------------------------------------
 * 库版本  ：ST3.5.0  三个ADC，每个ADC18个通道，16个外部通道和2个内部通道
**********************************************************************************/
#include "adc.h"

#define ADC1_DR_Address    ((u32)0x40012400+0x4c)

__IO uint16_t adc_converted_value[3];

/*
 * 函数名：adc1_gpio_config
 * 描述  ：使能ADC1和DMA1的时钟，初始化PC.00,PC.01,PC.02
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void adc1_gpio_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
/* Enable DMA clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
/* Enable ADC1 and GPIOC clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);
	
/* Configure PC.01  as analog input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);				                // PC1,输入时不用设置速率
}


/* 函数名：adc1_mode_config
 * 描述  ：配置ADC1的工作模式为MDA模式
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void adc1_mode_config(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
	
/* DMA channel1 configuration */
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 //ADC地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&adc_converted_value;//内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 3;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址固定
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//DMA_MemoryInc_Disable;  //内存地址不固定
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//半字
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		//循环传输
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
/* Enable DMA channel1 */
    DMA_Cmd(DMA1_Channel1, ENABLE);
	
/* ADC1 configuration */
	
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//独立ADC模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;         //  DISABLE ; 	 //禁止扫描模式，扫描模式用于多通道采集
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//开启连续转换模式，即不停地进行ADC转换
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//不使用外部触发转换
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	//采集数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 3;	 	//要转换的通道数目1
    ADC_Init(ADC1, &ADC_InitStructure);
	
/*配置ADC时钟，为PCLK2的8分频，即9Hz*/
//	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
/*配置ADC1的通道11为55.	5个采样周期，序列为1 */ 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);//ADC_SampleTime_1Cycles5 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_55Cycles5);	
/* Enable ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);
	
/* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);
	
/*复位校准寄存器 */   
    ADC_ResetCalibration(ADC1);
/*等待校准寄存器复位完成 */
    while(ADC_GetResetCalibrationStatus(ADC1));
	
/* ADC校准 */
    ADC_StartCalibration(ADC1);
/* 等待校准完成*/
    while(ADC_GetCalibrationStatus(ADC1));
	
/* 由于没有采用外部触发，所以使用软件触发ADC转换 */ 
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/*
 * 函数名：adc1_init
 * 描述  ：无
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void adc1_init(void)
{
    adc1_gpio_config();
    adc1_mode_config();
}


/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/

