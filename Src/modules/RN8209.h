#ifndef __RN8209_H
#define	__RN8209_H


#include "stm32f10x.h"

#define					ADSYSCON        0x00 
#define        	ADEMUCON        0x01
#define        	ADHFConst     	0x02 
#define        	ADPStart      	0x03 
#define        	ADDStart      	0x04 
#define					ADGPQA        	0x05 
#define        	ADGPQB        	0x06 
#define        	ADPhsA        	0x07 
#define        	ADPhsB        	0x08
#define					ADQPHSCAL				0x09    
#define					ADAPOSA 				0x0a
#define        	ADAPOSB 				0x0b
#define        	ADRPOSA 				0x0c
#define        	ADRPOSB 				0x0d
#define        	ADIARMSOS     	0x0e
#define        	ADIBRMSOS     	0x0f
#define        	ADIBGain      	0x10
#define					ADD2FPL       	0x11
#define        	ADD2FPH       	0x12
#define        	ADDCIAH       	0x13
#define        	ADDCIBH       	0x14
#define         ADDCUH		      0x15   
#define         ADDCL   	      0x16 
#define         ADEMUCON2	      0x17
#define					ADPFCnt    	    0x20
#define        	ADDFcnt    	    0x21
#define        	ADIARMS       	0x22
#define        	ADIBRMS       	0x23
#define        	ADURMS        	0x24
#define					ADUFreq       	0x25
#define        	ADPowerPA     	0x26
#define        	ADPowerPB     	0x27
#define         ADEnergyP  	    0x29
#define         ADEnergyP2 	    0x2a
#define         ADEnergyD  	    0x2b
#define         ADEnergyD2    	0x2c
#define         ADEMUSTATUS   	0x2d
#define         ADSPL_IA      	0x30
#define         ADSPL_IB      	0x31
#define         ADSPL_U       	0x32
#define         ADIE  		      0x40
#define         ADIF  		      0x41
#define         ADRIF    	      0x42
#define         ADSYSSTATUS  	  0x43
#define         ADRData      	  0x44
#define         ADWData      	  0x45
#define         ADDeviceID   	  0x7f

#define  ADD_SPECIAL_REG  	 0XEA
#define  ADD_WRITE_EN_CMD    0XE5
#define  ADD_WRITE_CLOSE_CMD 0XDC 
#define  ADD_CURRENT_A_CMD   0X5A
#define  ADD_CURRENT_B_CMD   0XA5
#define  ADD_RST_CMD         0XFA



#define MAX_COMPACK_SIZE 10

#define FRONT_FLAG_RN8209RST 1
#define INVERSION  0
#define POSITIVE   1
//У������ļ�
typedef struct 
{		
    u16			SYSCON;
    u16			EMUCON;
    u16			HFConst;
    u16			PStart;	
    u16			QStart;  			        //10
    u16			GPQA;    	
    u16			GPQB;    	
    u16			IAGain;  	
    u16			UGain;   	
    u16			IBGain;  	
    u16			PhsA;	   	
    u16			PhsB;    	
    u16			QPhsCal; 			        //22
    u16			APOSA;   	
    u16			APOSB;	 	
    u16			RPOSA;   	
    u16			RPOSB;   	
    u16			IARMSOS; 			        //32
    u16			IBRMSOS;			        //34
    u16			EMUCON2; 	
    float		KUrms;								//��ѹϵ��
    float		KIArms;								// Aͨ������ϵ��
    float		KIBrms;								// Bͨ������ϵ��
    float		KPrms;								// ����ϵ��
    u16			RealUI[2];						// ������ʾֵ�����ʴ��ڴ�ֵʱ��ʾ0.2%
    u32			RealPw;								//������ʾֵ���������ڴ�ֵʱ��ʾ0.2%
    u32			check_sum;             	
    u16			RTCDota0;							// RTCУ���Ĵ���
    u8			TemperAdj[2];					// �ߵ��²���ֵ
    u8			RTCAdj[4];						// RTC�ߵ���ʱ����ֵ
    u8			CurAdj;								// ����Ӱ�첹��ֵ
    u8 			OfsetAdjAcVolt[2]; 		//���ݵ�ѹ����OFFSET��ֵ
    u16			CorrectionTemper;  		//У��ʱ�̱�Ƶ��¶�
}RN8209_FirmParaFile_TypeDef;		  //58 Byte

//����ֵ ˲ʱֵ��ת�ļ�
typedef struct
{
    u8		ChkErrCnt;
    u32 	Pw[2];   		    //pa,pb   
    u32 	UI[3];          // Ia=UI[0] Inal U         
    u16 	Frequency;   		//����Ƶ��                         	
    u32		Pulse;		    	//ǰ̨����
    u16     Pstart;
    //��������
    u32		Pulse_Eg;	    	//�������
    u32 	PDirect;				//���ʷ���
    u32 	check_sum1;				//����EMU��У�����У��
    //У��ʹ�ò���
    u16		RatintU;				// ���ѹ
    u16		RatingI;				// �����
    u32		TempU;					// ��ǰУ�����ѹ
    u32		TempI;					// ��ǰУ�������
    u32		TempPw;					// ��ǰУ���㹦��
}RN8209_FrontTmp_TypeDef;

//����ֵ˲ʱֵ�ļ�
typedef struct
{	
    struct RN8209_Front_PubData_TypeDef  
    {
        u16			U;			    		//---��ѹ---NNN.N
        u32	    Ia;			    		//����NNNN.NNNN(����ֵҪ��3��3С������ֵҪ��2��4С�����λ��ʾ����)---		
        u32	    In;         		//---���ߵ���
        int16_t		  Pw;			    //---˲ʱ�й�p
        u16			Pf;			    		//---��������N.NNN---	���λ��ʾ����{Pf Pfa Pfb Pfc}	
        u16			Angle;		  		//---���NNN.N---		
        u16			Frequency;			//---Ƶ��NN.NN
        u32			PPwave;					//---һ���ӵ�ƽ������NN.NNNN
        u8      Chnsel;         //    		
        u16			Temperature;		//---NNN.N �¶�
        u16			ClockBat;				//---NN.NN  ��ص�ѹ
		    u32			tWorkBat;				//---NNNN  ʱ�ӹ���ʱ�䣨���ӣ�
        u8			PDirect;				//---ԭ���ʷ���
		                    		
        u16    	CfIn; 					//���������ƽ�ж�
        u8    	CfTime;					//
        u8    	Step;       		
        u16   	FrontStamp; 		
        u16			tMaxI;					// ����������ʱ�䣬0.5sΪ��λ
        u8			SafeCurFlag;		// �������޲�Ϊ���־
    }PubData;
	
    struct RN8209FrontPriData_TypeDef  
    {		
        u8			Flag;						//�����쳣��־
    } PriData;	
	
    struct RN8209FrontPriPara_TypeDef  
    {		
        u32	 PConstE;						//�й�����
        u16	 Crc;
    }PriPara;		
} RN8209_Front_TypeDef;	

//---����ͨѶ�����ļ�-------
typedef struct
{
	u16		EFlag;							//ͨѶ״̬
	
  u16 	RxLen;							//�������ݳ���
  u16  	TxLen;              //�������ݳ���
  u32		TimeOutStamp;				
  u8 		*pTx;
  
  u8		fBps;								//�����ʱ����־
  u8		NewBps;							//�²�����
  u32 	NewBpsStamp;				//������ʱ��
//u8 		TxAddr;
  
  u8 		RxBuf[MAX_COMPACK_SIZE];//���ջ���
  u8		TxBuf[MAX_COMPACK_SIZE];//���ͻ���
}Com_Pack_TypeDef;

extern Com_Pack_TypeDef g_rn8209_uart_com_pack;

ErrorStatus rn8209_write_reg(u8 w_reg, u8 *p_buf, u8 uc_len);
ErrorStatus rn8209_read_reg(u8 w_reg, u8 *p_buf, u8 uc_len);
void rn8209_init(void);
void read_rn8209_thread(void);
void rn_8209_rst(void);
void updata_rn8209_data(RN8209_Front_TypeDef *rn8209_data);
#endif /* __8209_H */

