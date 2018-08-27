/*******************************************************************************
 * �ļ���  ��adc.c
 * ����    ��adcӦ�ú�����        
 * Ӳ�����ӣ�----------------------------------------------------
 *          |                                                    |
 *          | PA0 - ADC123_IN10
 *          | PA1 - ADC123_IN11
            |                                                    |
 *           ----------------------------------------------------
 * ��汾  ��ST3.5.0  ����ADC��ÿ��ADC18��ͨ����16���ⲿͨ����2���ڲ�ͨ��
**********************************************************************************/
#include "adc.h"
#include "ntc.h"

#define ADC1_DR_Address    ((u32)0x40012400+0x4c)

__IO uint16_t adc_converted_value[3];

/*
 * ��������adc1_gpio_config
 * ����  ��ʹ��ADC1��DMA1��ʱ�ӣ���ʼ��PC.00,PC.01,PC.02
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */
static void adc1_gpio_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
/* Enable DMA clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
/* Enable ADC1 and GPIOC clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
	
/* Configure PC.01  as analog input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);				                // PA0 / PA1,����ʱ������������
}


/* ��������adc1_mode_config
 * ����  ������ADC1�Ĺ���ģʽΪMDAģʽ
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */
static void adc1_mode_config(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
	
/* DMA channel1 configuration */
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 //ADC��ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&adc_converted_value;//�ڴ��ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 2;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ�̶�
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//DMA_MemoryInc_Disable;  //�ڴ��ַ���̶�
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//����
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		//ѭ������
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
/* Enable DMA channel1 */
    DMA_Cmd(DMA1_Channel1, ENABLE);
	
/* ADC1 configuration */
	
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//����ADCģʽ
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;         //  DISABLE ; 	 //��ֹɨ��ģʽ��ɨ��ģʽ���ڶ�ͨ���ɼ�
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//��������ת��ģʽ������ͣ�ؽ���ADCת��
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//��ʹ���ⲿ����ת��
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	//�ɼ������Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 2;	 	//Ҫת����ͨ����Ŀ1
    ADC_Init(ADC1, &ADC_InitStructure);
	
/*����ADCʱ�ӣ�ΪPCLK2��8��Ƶ����9Hz*/
//	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
/*����ADC1��ͨ��11Ϊ55.	5���������ڣ�����Ϊ1 */ 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);//ADC_SampleTime_1Cycles5 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
/* Enable ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);
	
/* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);
	
/*��λУ׼�Ĵ��� */   
    ADC_ResetCalibration(ADC1);
/*�ȴ�У׼�Ĵ�����λ��� */
    while(ADC_GetResetCalibrationStatus(ADC1));
	
/* ADCУ׼ */
    ADC_StartCalibration(ADC1);
/* �ȴ�У׼���*/
    while(ADC_GetCalibrationStatus(ADC1));
	
/* ����û�в����ⲿ����������ʹ���������ADCת�� */ 
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/*
 * ��������adc1_init
 * ����  ����
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void adc1_init(void)
{
    adc1_gpio_config();
    adc1_mode_config();
}

/*
 * ��������updata_adc1_value
 * ����  ����
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void updata_adc1_value(uint16_t *temp, uint16_t *light)
{
   *temp = adc_converted_value[0];
   *light = adc_converted_value[1];
}

/*
 * ��������updata_ntc_value
 * ����  ����
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void updata_ntc_value(float *value_r)
{
	  float temp_v;
    temp_v = (float)(adc_converted_value[0]) /4096.0f;
	  *value_r = 1.75f * temp_v  / (1 - temp_v) * 1000;  
	  *value_r = updata_ntc_temp_value((uint32_t)(*value_r *10));
}

/*
 * ��������updata_light_value
 * ����  ����
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void updata_light_value(float *value_r)
{
	  float temp_v;
    temp_v = (float)(adc_converted_value[1]) /4096.0f;
	  *value_r = 200.0f * temp_v  / (1 - temp_v);  
}

/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/

