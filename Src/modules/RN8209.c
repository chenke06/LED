/*******************************************************************************
 * �ļ���  ��rn8209.c
 * ����    ��rn8209Ӧ�ú����� , RN8209�̶�������4800       
 * ��汾  ��ST3.5.0
**********************************************************************************/
#include "RN8209.h"
#include "delay.h"
#include "usart3.h"
#include "string.h"
#include "math_lib.h"
Com_Pack_TypeDef g_rn8209_uart_com_pack;
static RN8209_FirmParaFile_TypeDef s_RN8209_firm_para;
static RN8209_FrontTmp_TypeDef s_RN8209_front_tmp;
RN8209_Front_TypeDef s_RN8209_front;
static uint8_t s_RN8209_front_PubData_Pw_flag;
/*
 * ��������rn8209_write_reg(u8 w_reg,u8 *p_buf,u8 uc_len)
 * ����  ��
 * ����  : w_reg �Ĵ�����ַ�� *p_buf д��ֵ��uc_len д�볤��
 * ���  ����
 * ����  ���ڲ�����
 */
ErrorStatus rn8209_write_reg(u8 w_reg, u8 *p_buf, u8 uc_len)
{
    u8 i,j,temp,check_sum,repeat;
    ErrorStatus	err;
	
    if ((uc_len == 0) || (uc_len > 4)){
        return(ERROR);
		}
    for (repeat = 2; repeat != 0 ; repeat--){
        err = SUCCESS;	
        g_rn8209_uart_com_pack.pTx = &g_rn8209_uart_com_pack.TxBuf[0];	
		
		    //д����ǰ���ȷ���bit[7]=0:������;1:д����;bit[6:0]���������Ĵ�����ַ
		    temp = w_reg | 0x80;                                      //�������Ĵ�����ַ���λ��1��ʹ�����ֽ�Ϊд����
		    *(g_rn8209_uart_com_pack.pTx++) = temp;
		    check_sum = temp;		
		    for (i = uc_len; i > 0; i--){		
		        *(g_rn8209_uart_com_pack.pTx++) = p_buf[i - 1];	      //��RN8209��������	
		        check_sum += p_buf[i-1];
		    }
		    check_sum = ~check_sum;
		    *(g_rn8209_uart_com_pack.pTx++) = check_sum;
		    g_rn8209_uart_com_pack.TxLen = uc_len + 2;
		    g_rn8209_uart_com_pack.pTx = &g_rn8209_uart_com_pack.TxBuf[0];
		    usart3_send_data(g_rn8209_uart_com_pack.pTx, g_rn8209_uart_com_pack.TxLen);
		
		    //RN8209дʹ�ܻ�д��������д��WData�Ĵ�����飨�Ƿ���ȷд�룩	
		    g_rn8209_uart_com_pack.pTx = &g_rn8209_uart_com_pack.TxBuf[0];	
        if (w_reg == 0xEA){                                        //RN8209дʹ�ܻ�д����
            *(g_rn8209_uart_com_pack.pTx) = ADSYSSTATUS;
        }	
        else{//��д��WData�Ĵ������
    	      *(g_rn8209_uart_com_pack.pTx) = w_reg;
        }	
		    g_rn8209_uart_com_pack.TxLen = 1;	
		    g_rn8209_uart_com_pack.RxLen=0; 
		
		    usart3_send_data(g_rn8209_uart_com_pack.pTx, g_rn8209_uart_com_pack.TxLen);
//        memset(&g_rn8209_uart_com_pack.RxBuf[0] , 0x00 , 10);
        delay_ms(20);                    //�ȴ�ͨѶ���  
	      j = 0;
	      if (w_reg == 0xEA){//RN8209дʹ�ܻ�д����
	 		      if (p_buf[0] == 0XE5){//RN8209дʹ��
	 			        temp = g_rn8209_uart_com_pack.RxBuf[0];
	 			        if (!(temp & 0x10)) err = ERROR;
            }
            else if (p_buf[0] == 0XDC){//RN8209д����
	 			        temp = g_rn8209_uart_com_pack.RxBuf[0];
	 			        if (temp & 0x10) err = ERROR;
			      }
	 	    }
	 	    else{	//��д��WData�Ĵ������(��������g_rn8209_uart_com_pack.RxBuf[j++];��������p_buf[i-1])
		        for (i = uc_len; i > 0; i--){
				        temp = g_rn8209_uart_com_pack.RxBuf[j++]; 
                if ((w_reg == 0)&&(i==2)) temp = 0;//�쳣����
                if (temp != p_buf[i-1]){
					          err = ERROR;
					          break;
				        }
			      }
				}
	     if(err == SUCCESS) break;
		}
    return(err);
}
/*
 * ��������rn8209_read_reg(u8 w_reg,u8 *p_buf,u8 uc_len)
 * ����  ��
 * ����  : w_reg �Ĵ�����ַ�� *p_buf ����ֵ��uc_len д�볤��
 * ���  ����
 * ����  ���ڲ�����
 */
ErrorStatus rn8209_read_reg(u8 w_reg, u8 *p_buf, u8 uc_len)
{
    u8 i,temp,repeat;
    u8 j = 0;
    u8 check_sum = 0;	
    ErrorStatus	err;
    if (uc_len == 0) return(ERROR);
    for(repeat=2; repeat != 0 ; repeat--){
		    err  = SUCCESS;	    
		    temp = w_reg ;
		    check_sum = w_reg;
		    j = 0;	  
		
        g_rn8209_uart_com_pack.pTx = &g_rn8209_uart_com_pack.TxBuf[0];	
        *(g_rn8209_uart_com_pack.pTx) = temp;	
        g_rn8209_uart_com_pack.TxLen = 1;	
				usart3_send_data(g_rn8209_uart_com_pack.pTx, g_rn8209_uart_com_pack.TxLen);
 //       memset(&g_rn8209_uart_com_pack.RxBuf[0] , 0x00, 10);
	    
        delay_ms(20);
        for (i = uc_len; i > 0;i--){
            p_buf[i-1] = g_rn8209_uart_com_pack.RxBuf[j++];
            check_sum += p_buf[i-1];
        }
        check_sum = ~check_sum;
        temp = g_rn8209_uart_com_pack.RxBuf[j++];
		
		    if(temp!=check_sum){//��У��ʹ������������
			      err = ERROR;
            for(i = uc_len; i > 0;i--) p_buf[i-1] = 0;
        }
        if(err == SUCCESS) break;
    }
    g_rn8209_uart_com_pack.RxLen = 0;          
    return(err);
}

/*
 * ��������void rn_8209_rst(void)
 * ����  �����ָ�λ��ʽ
 * ����  : RX���Ÿ���RST,���λ��ϵͳ״̬�Ĵ��� RST ���ϵ縴λ��������1����������
 * ���  ����
 * ����  ���ڲ�����
 */
void rn_8209_rst(void)
{
	  u8 data_temp8;
    data_temp8 = ADD_RST_CMD;
    rn8209_write_reg(ADD_SPECIAL_REG, &data_temp8, 1);
    //rn8209_read_reg(ADSYSSTATUS, &temp, 1);
    delay_ms(20);
}

/*
 * ��������fn_RN8209_cominit_rn8209
 * ����  ����У�������������rn8209�Ĵ���
 * ����  : 
 * ���  ����
 * ����  ���ڲ�����
 */
//uint8_t rn8209_id[3];
void rn8209_init(void)
{
	  u8 data_temp8;
    s_RN8209_firm_para.SYSCON = 0x0003;       //��ѹ����1����������16
    s_RN8209_firm_para.EMUCON = 0x0003;       
    s_RN8209_firm_para.HFConst = 0x1000;
    s_RN8209_firm_para.PStart = 0x0060;
    s_RN8209_firm_para.QStart = 0x0120;
    s_RN8209_firm_para.GPQA = 0x0000;
    s_RN8209_firm_para.GPQB = 0x0000;
    s_RN8209_firm_para.PhsA = 0x0000;
    s_RN8209_firm_para.PhsB = 0x0000;
    s_RN8209_firm_para.QPhsCal = 0x0000;
    s_RN8209_firm_para.APOSA = 0x0000;
    s_RN8209_firm_para.APOSB = 0x0000;
    s_RN8209_firm_para.RPOSA = 0x0000;
    s_RN8209_firm_para.RPOSB = 0x0000;
    s_RN8209_firm_para.IARMSOS = 0x0000;
    s_RN8209_firm_para.IBRMSOS = 0x0000;
    s_RN8209_firm_para.IBGain = 0x0000;
    s_RN8209_firm_para.EMUCON2 = 0x300;
	  s_RN8209_firm_para.KUrms=0.4926;                                        //��ѹУ׼ϵ��
	  s_RN8209_firm_para.KIArms= 0.00288372f;                                           //����У׼ϵ��
	//s_RN8209_firm_para.KIBrms= 0x00000000;
	
    s_RN8209_firm_para.KPrms= 0x00000000;

//    rn8209_read_reg(ADDeviceID, (u8 *)&rn8209_id, 3);
		
		s_RN8209_front.PriData.Flag |= FRONT_FLAG_RN8209RST;                  //��RN8209��ʼ����ʶλ

 //   memset(&s_RN8209_front_tmp , 0 , sizeof(RN8209_FrontTmp_TypeDef) );  //������õ���RAM����
 //   memset(&s_RN8209_front , 0 , sizeof(RN8209_Front_TypeDef) );        //������õ���RAM����
		
		
    data_temp8 = ADD_WRITE_EN_CMD;
    rn8209_write_reg(ADD_SPECIAL_REG , &data_temp8, 1);
	  rn_8209_rst();                                                   //��λ
		
//		
    if (rn8209_write_reg( 0x00 , (u8 *)&s_RN8209_firm_para.SYSCON , 2 ) == ERROR) return ;   //дϵͳ���ƼĴ���
    if (rn8209_write_reg( 0x01 , (u8 *)&s_RN8209_firm_para.EMUCON , 2 ) == ERROR) return ;   //д�������ƼĴ���
		
    if(rn8209_write_reg( ADHFConst , (u8 *)&s_RN8209_firm_para.HFConst , 2 ) == ERROR) return ; 
    if(rn8209_write_reg( ADPStart , (u8 *)&s_RN8209_firm_para.PStart , 2 ) == ERROR) return ; 	
    if(rn8209_write_reg( ADDStart , (u8 *)&s_RN8209_firm_para.QStart , 2 ) == ERROR) return ; 
//	
//    if(rn8209_write_reg( ADGPQA , (u8 *)&s_RN8209_firm_para.GPQA , 2 ) == ERROR) return;   //д��������Ĵ���
//    if(rn8209_write_reg( ADGPQB , (u8 *)&s_RN8209_firm_para.GPQB , 2 ) == ERROR) return;   //д��������Ĵ���
//    if(rn8209_write_reg( ADPhsA , (u8 *)&s_RN8209_firm_para.PhsA , 1 ) == ERROR) return;   //д��λУ���Ĵ���
//    if(rn8209_write_reg( ADPhsB , (u8 *)&s_RN8209_firm_para.PhsB , 1 ) == ERROR) return;   //д��λУ���Ĵ���
//    if(rn8209_write_reg( ADQPHSCAL , (u8 *)&s_RN8209_firm_para.QPhsCal , 2 ) == ERROR) return;    
//    if(rn8209_write_reg( ADAPOSA , (u8 *)&s_RN8209_firm_para.APOSA , 2 ) == ERROR) return;
//    if(rn8209_write_reg( ADAPOSB , (u8 *)&s_RN8209_firm_para.APOSB , 2 ) == ERROR) return;
//    if(rn8209_write_reg( ADRPOSA , (u8 *)&s_RN8209_firm_para.RPOSA , 2 ) == ERROR) return;
//    if(rn8209_write_reg( ADRPOSB , (u8 *)&s_RN8209_firm_para.RPOSB , 2 ) == ERROR) return;
//    if(rn8209_write_reg( ADIARMSOS , (u8 *)&s_RN8209_firm_para.IARMSOS , 2 ) == ERROR) return;
//    if(rn8209_write_reg( ADIBRMSOS , (u8 *)&s_RN8209_firm_para.IBRMSOS , 2 ) == ERROR) return;
//    if(rn8209_write_reg( ADIBGain , (u8 *)&s_RN8209_firm_para.IBGain , 2 ) == ERROR) return;
//    if(rn8209_write_reg( ADEMUCON2 , (u8 *)&s_RN8209_firm_para.EMUCON2 , 2 ) == ERROR) return;

//    data_temp8 = ADD_WRITE_CLOSE_CMD;
//    rn8209_write_reg(ADD_SPECIAL_REG, &data_temp8, 1);
}
/*
 * ��������read_rn8209_thread
 * ����  : 
 * ����  : 
 * ���  ����
 * ����  ���ڲ�����
 */
void read_rn8209_thread(void)
{ 
    u8 i;
	  u32 TempU,TempI,TempIn;
	  u32 TempStatus;
	  u16 TempAngle;	
	  u8	PFlag;		
    if(s_RN8209_front.PriData.Flag & FRONT_FLAG_RN8209RST){//RN8209��λ
        rn8209_init();
        s_RN8209_front.PriData.Flag &= ~FRONT_FLAG_RN8209RST;
		    return;
		}
	  
    //������״̬
    TempStatus = 0;
    rn8209_read_reg(ADEMUSTATUS, (u8 *)&TempStatus, 3);
    //�жϹ��ʷ���
    if (TempStatus & 0x020000){
        s_RN8209_front.PubData.PDirect =  INVERSION ;
    }//�жϹ��ʷ����Ƿ���
    else{
        s_RN8209_front.PubData.PDirect = POSITIVE ;
		}    

    //����ѹ��������������������Ϊ�����д���
    for (i = 0 ; i < 3 ; i++){
        s_RN8209_front_tmp.UI[i] = 0;//���ѹ����������
        rn8209_read_reg(ADIARMS + i, (u8 *)&s_RN8209_front_tmp.UI[i], 3) ;
        if (s_RN8209_front_tmp.UI[0] & 0x00800000){
				    s_RN8209_front_tmp.UI[i] = 0;
				}
    }
    //��Ƶ��
    rn8209_read_reg(0x25 , (u8 *)&s_RN8209_front_tmp.Frequency , 2) ;
    //�����ʵ�������������Ϊ�����д���
    rn8209_read_reg(0x26 , (u8 *)&s_RN8209_front_tmp.Pw[0] , 4) ;     //A   
    if (s_RN8209_front_tmp.Pw[0] & 0x80000000){
        s_RN8209_front_tmp.Pw[0] = (~s_RN8209_front_tmp.Pw[0]) + 1;
        PFlag = 1;
		}
		else{
        PFlag = 0;
		}
    //���㹦��
    s_RN8209_front.PubData.Pw = s_RN8209_front_tmp.Pw[0];
    if (PFlag){
        s_RN8209_front_PubData_Pw_flag = 1;
		}
    //������ѹ����
    s_RN8209_front.PubData.U = (0x7fff & (s_RN8209_front_tmp.UI[2])) * 10 / 64;//s_RN8209_firm_para.KUrms;
    TempI = s_RN8209_front_tmp.UI[0] * 10 / 64;//* s_RN8209_firm_para.KIArms;   
    s_RN8209_front.PubData.Ia = TempI;	
    if(PFlag) s_RN8209_front.PubData.Ia |= 0x80000000;
//	  TempU = s_RN8209_front.PubData.U;
    //������Чֵ���㣬������������
    //���㹦������
    TempU &= 0x00ffffff; 
    if (TempU & 0x00800000){
        TempU = ((~TempU) & 0x00ffffff) + 1;
		}
    s_RN8209_front.PubData.Pf = ((u16)((float)TempU / 8388.608));
    s_RN8209_front.PubData.Angle =	(TempAngle * 3600 / 32768); 
    //����С��0.0030�����㣬����������0.999
    if (((BYTE2(s_RN8209_front.PubData.Pw) & 0x7f) == 0)&&
			   (BYTE1(s_RN8209_front.PubData.Pw) == 0)&&
		     (BYTE0(s_RN8209_front.PubData.Pw) < 0x30)){
        BYTE0(s_RN8209_front.PubData.Pw) = 0;
        BYTE1(s_RN8209_front.PubData.Pw) = 0;
        BYTE2(s_RN8209_front.PubData.Pw) = 0;
		}
    //����С����������������
    if ((s_RN8209_front.PubData.Ia&0x7fffffff) < 0x00000150) {s_RN8209_front.PubData.Ia = 0;s_RN8209_front.PubData.Pf = 0x0999;}
    if ((s_RN8209_front.PubData.In&0x7fffffff) < 0x00000150) s_RN8209_front.PubData.In = 0;
	
     //��ѹƵ�ʼ���
     s_RN8209_front.PubData.Frequency = ((u16)(((u32)357954500)/((u32)8*s_RN8209_front_tmp.Frequency)));	
     if((s_RN8209_front.PubData.Ia & 0x7fffffff) > 0x00550000){	
         if(s_RN8209_front.PubData.tMaxI < 2405){
				     s_RN8209_front.PubData.tMaxI++;
				 }
		 }
     else{
         s_RN8209_front.PubData.tMaxI = 0;
		 }
     if (s_RN8209_front_PubData_Pw_flag){
         s_RN8209_front.PubData.Pf |= 0x8000;
		 }
	
    //���������壬�������������
    s_RN8209_front_tmp.Pulse = 0;
    rn8209_read_reg( 0x2a , (u8 *)&s_RN8209_front_tmp.Pulse , 3) ;
    if(s_RN8209_front_tmp.Pulse > 100){
        s_RN8209_front_tmp.Pulse = 0;	//�ݴ���������������
        s_RN8209_front_tmp.Pulse_Eg += s_RN8209_front_tmp.Pulse;
    }
    #if  RN8209SOFT_DEBUG
    s_RN8209_front_tmp.Pulse_Eg += 1;
    #endif	
}
/***************************************************
 * ��������updata_rn8209_data
	u16			U;			    		//---��ѹ---NNN.N
	u32	    Ia;			    		//����NNNN.NNNN(����ֵҪ��3��3С������ֵҪ��2��4С�����λ��ʾ����)---		
	u32	    In;         		//---���ߵ���
	int16_t	Pw;			        //---˲ʱ�й�p
	u16			Pf;			    		//---��������N.NNN---	���λ��ʾ����{Pf Pfa Pfb Pfc}	
	u16			Angle;		  		//---���NNN.N---		
	u16			Frequency;			//---Ƶ��NN.NN
***********************************************************/
void updata_rn8209_data(RN8209_Front_TypeDef *rn8209_data)
{
    rn8209_data->PubData.U = s_RN8209_front.PubData.U;
    rn8209_data->PubData.Ia = s_RN8209_front.PubData.Ia;
    rn8209_data->PubData.Pw = s_RN8209_front.PubData.Pw;
    rn8209_data->PubData.Pf = s_RN8209_front.PubData.Pf;
    rn8209_data->PubData.Frequency = s_RN8209_front.PubData.Frequency;
}

