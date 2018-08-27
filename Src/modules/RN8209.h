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
//校表参数文件
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
    float		KUrms;								//电压系数
    float		KIArms;								// A通道电流系数
    float		KIBrms;								// B通道电流系数
    float		KPrms;								// 功率系数
    u16			RealUI[2];						// 功率显示值，功率大于此值时显示0.2%
    u32			RealPw;								//电流显示值，电流大于此值时显示0.2%
    u32			check_sum;             	
    u16			RTCDota0;							// RTC校正寄存器
    u8			TemperAdj[2];					// 高低温补偿值
    u8			RTCAdj[4];						// RTC高低温时补偿值
    u8			CurAdj;								// 自热影响补偿值
    u8 			OfsetAdjAcVolt[2]; 		//根据电压调整OFFSET的值
    u16			CorrectionTemper;  		//校表时刻表计的温度
}RN8209_FirmParaFile_TypeDef;		  //58 Byte

//计量值 瞬时值中转文件
typedef struct
{
    u8		ChkErrCnt;
    u32 	Pw[2];   		    //pa,pb   
    u32 	UI[3];          // Ia=UI[0] Inal U         
    u16 	Frequency;   		//电网频率                         	
    u32		Pulse;		    	//前台脉冲
    u16     Pstart;
    //电能脉冲
    u32		Pulse_Eg;	    	//脉冲个数
    u32 	PDirect;				//功率方向
    u32 	check_sum1;				//读出EMU的校表参数校验
    //校表使用参数
    u16		RatintU;				// 额定电压
    u16		RatingI;				// 额定电流
    u32		TempU;					// 当前校正点电压
    u32		TempI;					// 当前校正点电流
    u32		TempPw;					// 当前校正点功率
}RN8209_FrontTmp_TypeDef;

//计量值瞬时值文件
typedef struct
{	
    struct RN8209_Front_PubData_TypeDef  
    {
        u16			U;			    		//---电压---NNN.N
        u32	    Ia;			    		//电流NNNN.NNNN(电流值要求3整3小，整定值要求2整4小，最高位表示方向)---		
        u32	    In;         		//---零线电流
        int16_t		  Pw;			    //---瞬时有功p
        u16			Pf;			    		//---功率因数N.NNN---	最高位表示方向{Pf Pfa Pfb Pfc}	
        u16			Angle;		  		//---相角NNN.N---		
        u16			Frequency;			//---频率NN.NN
        u32			PPwave;					//---一分钟的平均功率NN.NNNN
        u8      Chnsel;         //    		
        u16			Temperature;		//---NNN.N 温度
        u16			ClockBat;				//---NN.NN  电池电压
		    u32			tWorkBat;				//---NNNN  时钟工作时间（分钟）
        u8			PDirect;				//---原功率方向
		                    		
        u16    	CfIn; 					//脉冲输入电平判断
        u8    	CfTime;					//
        u8    	Step;       		
        u16   	FrontStamp; 		
        u16			tMaxI;					// 最大电流持续时间，0.5s为单位
        u8			SafeCurFlag;		// 电流门限不为零标志
    }PubData;
	
    struct RN8209FrontPriData_TypeDef  
    {		
        u8			Flag;						//工作异常标志
    } PriData;	
	
    struct RN8209FrontPriPara_TypeDef  
    {		
        u32	 PConstE;						//有功常数
        u16	 Crc;
    }PriPara;		
} RN8209_Front_TypeDef;	

//---串口通讯变量文件-------
typedef struct
{
	u16		EFlag;							//通讯状态
	
  u16 	RxLen;							//接收数据长度
  u16  	TxLen;              //发送数据长度
  u32		TimeOutStamp;				
  u8 		*pTx;
  
  u8		fBps;								//波特率变更标志
  u8		NewBps;							//新波特率
  u32 	NewBpsStamp;				//新速率时标
//u8 		TxAddr;
  
  u8 		RxBuf[MAX_COMPACK_SIZE];//接收缓存
  u8		TxBuf[MAX_COMPACK_SIZE];//发送缓存
}Com_Pack_TypeDef;

extern Com_Pack_TypeDef g_rn8209_uart_com_pack;

ErrorStatus rn8209_write_reg(u8 w_reg, u8 *p_buf, u8 uc_len);
ErrorStatus rn8209_read_reg(u8 w_reg, u8 *p_buf, u8 uc_len);
void rn8209_init(void);
void read_rn8209_thread(void);
void rn_8209_rst(void);
void updata_rn8209_data(RN8209_Front_TypeDef *rn8209_data);
#endif /* __8209_H */

