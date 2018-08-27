/*******************************************************************************
 * 文件名  ：rn8209.c
 * 描述    ：rn8209应用函数库 , RN8209固定波特率4800       
 * 库版本  ：ST3.5.0
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
 * 函数名：rn8209_write_reg(u8 w_reg,u8 *p_buf,u8 uc_len)
 * 描述  ：
 * 输入  : w_reg 寄存器地址， *p_buf 写入值，uc_len 写入长度
 * 输出  ：无
 * 调用  ：内部调用
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
		
		    //写数据前，先发送bit[7]=0:读操作;1:写操作;bit[6:0]：待操作寄存器地址
		    temp = w_reg | 0x80;                                      //待操作寄存器地址最高位或1，使命令字节为写命令
		    *(g_rn8209_uart_com_pack.pTx++) = temp;
		    check_sum = temp;		
		    for (i = uc_len; i > 0; i--){		
		        *(g_rn8209_uart_com_pack.pTx++) = p_buf[i - 1];	      //向RN8209发送数据	
		        check_sum += p_buf[i-1];
		    }
		    check_sum = ~check_sum;
		    *(g_rn8209_uart_com_pack.pTx++) = check_sum;
		    g_rn8209_uart_com_pack.TxLen = uc_len + 2;
		    g_rn8209_uart_com_pack.pTx = &g_rn8209_uart_com_pack.TxBuf[0];
		    usart3_send_data(g_rn8209_uart_com_pack.pTx, g_rn8209_uart_com_pack.TxLen);
		
		    //RN8209写使能或写保护、读写入WData寄存器检查（是否正确写入）	
		    g_rn8209_uart_com_pack.pTx = &g_rn8209_uart_com_pack.TxBuf[0];	
        if (w_reg == 0xEA){                                        //RN8209写使能或写保护
            *(g_rn8209_uart_com_pack.pTx) = ADSYSSTATUS;
        }	
        else{//读写入WData寄存器检查
    	      *(g_rn8209_uart_com_pack.pTx) = w_reg;
        }	
		    g_rn8209_uart_com_pack.TxLen = 1;	
		    g_rn8209_uart_com_pack.RxLen=0; 
		
		    usart3_send_data(g_rn8209_uart_com_pack.pTx, g_rn8209_uart_com_pack.TxLen);
//        memset(&g_rn8209_uart_com_pack.RxBuf[0] , 0x00 , 10);
        delay_ms(20);                    //等待通讯完成  
	      j = 0;
	      if (w_reg == 0xEA){//RN8209写使能或写保护
	 		      if (p_buf[0] == 0XE5){//RN8209写使能
	 			        temp = g_rn8209_uart_com_pack.RxBuf[0];
	 			        if (!(temp & 0x10)) err = ERROR;
            }
            else if (p_buf[0] == 0XDC){//RN8209写保护
	 			        temp = g_rn8209_uart_com_pack.RxBuf[0];
	 			        if (temp & 0x10) err = ERROR;
			      }
	 	    }
	 	    else{	//读写入WData寄存器检查(接收数据g_rn8209_uart_com_pack.RxBuf[j++];发送数据p_buf[i-1])
		        for (i = uc_len; i > 0; i--){
				        temp = g_rn8209_uart_com_pack.RxBuf[j++]; 
                if ((w_reg == 0)&&(i==2)) temp = 0;//异常处理
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
 * 函数名：rn8209_read_reg(u8 w_reg,u8 *p_buf,u8 uc_len)
 * 描述  ：
 * 输入  : w_reg 寄存器地址， *p_buf 读入值，uc_len 写入长度
 * 输出  ：无
 * 调用  ：内部调用
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
		
		    if(temp!=check_sum){//若校验和错误，清接收数据
			      err = ERROR;
            for(i = uc_len; i > 0;i--) p_buf[i-1] = 0;
        }
        if(err == SUCCESS) break;
    }
    g_rn8209_uart_com_pack.RxLen = 0;          
    return(err);
}

/*
 * 函数名：void rn_8209_rst(void)
 * 描述  ：三种复位方式
 * 输入  : RX引脚复用RST,命令复位，系统状态寄存器 RST ，上电复位结束后置1，读后清零
 * 输出  ：无
 * 调用  ：内部调用
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
 * 函数名：fn_RN8209_cominit_rn8209
 * 描述  ：清校表参数函数，清rn8209寄存器
 * 输入  : 
 * 输出  ：无
 * 调用  ：内部调用
 */
//uint8_t rn8209_id[3];
void rn8209_init(void)
{
	  u8 data_temp8;
    s_RN8209_firm_para.SYSCON = 0x0003;       //电压增益1，电流增益16
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
	  s_RN8209_firm_para.KUrms=0.4926;                                        //电压校准系数
	  s_RN8209_firm_para.KIArms= 0.00288372f;                                           //电流校准系数
	//s_RN8209_firm_para.KIBrms= 0x00000000;
	
    s_RN8209_firm_para.KPrms= 0x00000000;

//    rn8209_read_reg(ADDeviceID, (u8 *)&rn8209_id, 3);
		
		s_RN8209_front.PriData.Flag |= FRONT_FLAG_RN8209RST;                  //置RN8209初始化标识位

 //   memset(&s_RN8209_front_tmp , 0 , sizeof(RN8209_FrontTmp_TypeDef) );  //清计量用到的RAM数据
 //   memset(&s_RN8209_front , 0 , sizeof(RN8209_Front_TypeDef) );        //清计量用到的RAM数据
		
		
    data_temp8 = ADD_WRITE_EN_CMD;
    rn8209_write_reg(ADD_SPECIAL_REG , &data_temp8, 1);
	  rn_8209_rst();                                                   //复位
		
//		
    if (rn8209_write_reg( 0x00 , (u8 *)&s_RN8209_firm_para.SYSCON , 2 ) == ERROR) return ;   //写系统控制寄存器
    if (rn8209_write_reg( 0x01 , (u8 *)&s_RN8209_firm_para.EMUCON , 2 ) == ERROR) return ;   //写计量控制寄存器
		
    if(rn8209_write_reg( ADHFConst , (u8 *)&s_RN8209_firm_para.HFConst , 2 ) == ERROR) return ; 
    if(rn8209_write_reg( ADPStart , (u8 *)&s_RN8209_firm_para.PStart , 2 ) == ERROR) return ; 	
    if(rn8209_write_reg( ADDStart , (u8 *)&s_RN8209_firm_para.QStart , 2 ) == ERROR) return ; 
//	
//    if(rn8209_write_reg( ADGPQA , (u8 *)&s_RN8209_firm_para.GPQA , 2 ) == ERROR) return;   //写功率增益寄存器
//    if(rn8209_write_reg( ADGPQB , (u8 *)&s_RN8209_firm_para.GPQB , 2 ) == ERROR) return;   //写功率增益寄存器
//    if(rn8209_write_reg( ADPhsA , (u8 *)&s_RN8209_firm_para.PhsA , 1 ) == ERROR) return;   //写相位校正寄存器
//    if(rn8209_write_reg( ADPhsB , (u8 *)&s_RN8209_firm_para.PhsB , 1 ) == ERROR) return;   //写相位校正寄存器
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
 * 函数名：read_rn8209_thread
 * 描述  : 
 * 输入  : 
 * 输出  ：无
 * 调用  ：内部调用
 */
void read_rn8209_thread(void)
{ 
    u8 i;
	  u32 TempU,TempI,TempIn;
	  u32 TempStatus;
	  u16 TempAngle;	
	  u8	PFlag;		
    if(s_RN8209_front.PriData.Flag & FRONT_FLAG_RN8209RST){//RN8209复位
        rn8209_init();
        s_RN8209_front.PriData.Flag &= ~FRONT_FLAG_RN8209RST;
		    return;
		}
	  
    //读计量状态
    TempStatus = 0;
    rn8209_read_reg(ADEMUSTATUS, (u8 *)&TempStatus, 3);
    //判断功率方向
    if (TempStatus & 0x020000){
        s_RN8209_front.PubData.PDirect =  INVERSION ;
    }//判断功率方向是反的
    else{
        s_RN8209_front.PubData.PDirect = POSITIVE ;
		}    

    //读电压、电流到缓冲区，读数为负进行处理
    for (i = 0 ; i < 3 ; i++){
        s_RN8209_front_tmp.UI[i] = 0;//清电压、电流变量
        rn8209_read_reg(ADIARMS + i, (u8 *)&s_RN8209_front_tmp.UI[i], 3) ;
        if (s_RN8209_front_tmp.UI[0] & 0x00800000){
				    s_RN8209_front_tmp.UI[i] = 0;
				}
    }
    //读频率
    rn8209_read_reg(0x25 , (u8 *)&s_RN8209_front_tmp.Frequency , 2) ;
    //读功率到缓冲区，读数为负进行处理
    rn8209_read_reg(0x26 , (u8 *)&s_RN8209_front_tmp.Pw[0] , 4) ;     //A   
    if (s_RN8209_front_tmp.Pw[0] & 0x80000000){
        s_RN8209_front_tmp.Pw[0] = (~s_RN8209_front_tmp.Pw[0]) + 1;
        PFlag = 1;
		}
		else{
        PFlag = 0;
		}
    //计算功率
    s_RN8209_front.PubData.Pw = s_RN8209_front_tmp.Pw[0];
    if (PFlag){
        s_RN8209_front_PubData_Pw_flag = 1;
		}
    //电流电压计算
    s_RN8209_front.PubData.U = (0x7fff & (s_RN8209_front_tmp.UI[2])) * 10 / 64;//s_RN8209_firm_para.KUrms;
    TempI = s_RN8209_front_tmp.UI[0] * 10 / 64;//* s_RN8209_firm_para.KIArms;   
    s_RN8209_front.PubData.Ia = TempI;	
    if(PFlag) s_RN8209_front.PubData.Ia |= 0x80000000;
//	  TempU = s_RN8209_front.PubData.U;
    //功率有效值计算，功率因数计算
    //计算功率因数
    TempU &= 0x00ffffff; 
    if (TempU & 0x00800000){
        TempU = ((~TempU) & 0x00ffffff) + 1;
		}
    s_RN8209_front.PubData.Pf = ((u16)((float)TempU / 8388.608));
    s_RN8209_front.PubData.Angle =	(TempAngle * 3600 / 32768); 
    //功率小于0.0030，清零，功率因数置0.999
    if (((BYTE2(s_RN8209_front.PubData.Pw) & 0x7f) == 0)&&
			   (BYTE1(s_RN8209_front.PubData.Pw) == 0)&&
		     (BYTE0(s_RN8209_front.PubData.Pw) < 0x30)){
        BYTE0(s_RN8209_front.PubData.Pw) = 0;
        BYTE1(s_RN8209_front.PubData.Pw) = 0;
        BYTE2(s_RN8209_front.PubData.Pw) = 0;
		}
    //电流小于启动电流，清零
    if ((s_RN8209_front.PubData.Ia&0x7fffffff) < 0x00000150) {s_RN8209_front.PubData.Ia = 0;s_RN8209_front.PubData.Pf = 0x0999;}
    if ((s_RN8209_front.PubData.In&0x7fffffff) < 0x00000150) s_RN8209_front.PubData.In = 0;
	
     //电压频率计算
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
	
    //读电能脉冲，加入脉冲计算器
    s_RN8209_front_tmp.Pulse = 0;
    rn8209_read_reg( 0x2a , (u8 *)&s_RN8209_front_tmp.Pulse , 3) ;
    if(s_RN8209_front_tmp.Pulse > 100){
        s_RN8209_front_tmp.Pulse = 0;	//容错，脉冲个数过大，清除
        s_RN8209_front_tmp.Pulse_Eg += s_RN8209_front_tmp.Pulse;
    }
    #if  RN8209SOFT_DEBUG
    s_RN8209_front_tmp.Pulse_Eg += 1;
    #endif	
}
/***************************************************
 * 函数名：updata_rn8209_data
	u16			U;			    		//---电压---NNN.N
	u32	    Ia;			    		//电流NNNN.NNNN(电流值要求3整3小，整定值要求2整4小，最高位表示方向)---		
	u32	    In;         		//---零线电流
	int16_t	Pw;			        //---瞬时有功p
	u16			Pf;			    		//---功率因数N.NNN---	最高位表示方向{Pf Pfa Pfb Pfc}	
	u16			Angle;		  		//---相角NNN.N---		
	u16			Frequency;			//---频率NN.NN
***********************************************************/
void updata_rn8209_data(RN8209_Front_TypeDef *rn8209_data)
{
    rn8209_data->PubData.U = s_RN8209_front.PubData.U;
    rn8209_data->PubData.Ia = s_RN8209_front.PubData.Ia;
    rn8209_data->PubData.Pw = s_RN8209_front.PubData.Pw;
    rn8209_data->PubData.Pf = s_RN8209_front.PubData.Pf;
    rn8209_data->PubData.Frequency = s_RN8209_front.PubData.Frequency;
}

