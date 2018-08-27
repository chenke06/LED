/**********************************************************************************
 * �ļ���  ��dac1.c
 * ����    ��dac1 �ײ�Ӧ�ú�����         
 * Ӳ������ ----------------------------
 *         | PA4           : DAC1       |
 *         |                            |
 *         |                            |
 *         |                            |
 *          ----------------------------
 * ��汾  ��ST3.5.0
**********************************************************************************/
#include "dac1.h"
#include "delay.h"
/*
 * ��������dac1_Init
 * ����  ��
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
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
    
    DAC_InitType.DAC_Trigger = DAC_Trigger_None;                        //��ʹ�ô�������
    DAC_InitType.DAC_WaveGeneration = DAC_WaveGeneration_None;          //��ʹ�ò��η���
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;//���η�ֵ���� 
    DAC_InitType.DAC_OutputBuffer = DAC_OutputBuffer_Disable ;          //DAC1�������ر�BOFF1=1
    DAC_Init(DAC_Channel_1, &DAC_InitType);                              
    
    DAC_Cmd(DAC_Channel_1, ENABLE);
    DAC_SetChannel1Data(DAC_Align_12b_R, 0);                             //12λ�Ҷ������ݸ�ʽ����DACֵ
    delay_ms(1);
		#ifdef PWM_MODE
    dac1_set_vol(2000);
		#else
    dac1_set_vol(0);
		#endif
}

/*
 * ����  ��
 * ������������ͨ��1�����ѹ
 * ����  �� 0~3300 
 * ���  �� 0~3.3V
 * ����  ���ڲ�����
 */
void dac1_set_vol(u16 vol)
{
    float temp = vol;
    temp /= 1000;
    temp = temp * 4096 / 2.5;
    DAC_SetChannel1Data(DAC_Align_12b_R,temp);
}
