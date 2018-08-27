/*******************************************************************************
 * �ļ���  ��CRC16.c
 * ����    ��delayӦ�ú�����        
 * ��汾  ��ST3.5.0
**********************************************************************************/
#include "CRC16.h"

/*
 * ��������crc16
 * ����  ��
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */
uint16_t crc16(uint8_t *data,int length)//(uint8_t* str, uint16_t len)
{
    uint16_t Reg_CRC=0xffff;
    uint8_t Temp_reg=0x00;
    int i,j;

    for( i = 0; i<length; i ++)
    {
        Temp_reg = *data++;
        Reg_CRC ^= (uint16_t)Temp_reg;

        for (j = 0; j<8; j++)
        {
            if (Reg_CRC & 0x0001)
            {
                Reg_CRC = (Reg_CRC >> 1)^0xA001;
            }
            else
            {
                Reg_CRC >>=1;
            }

        }
    }
    return (Reg_CRC&0xffffu);	
}