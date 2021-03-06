/*********************************************************************************
 * 文件名  ：AT24C080D.c
 * 描述    ：AT24C080D应用函数库        
 * 硬件连接：----------------------------------------------------
             i2c2                                       
                                          
 *           ----------------------------------------------------
 * 库版本  ：ST3.5.0  
**********************************************************************************/
#include "AT24C080D.h"
#include "i2c2.h"
#include "lora.h"
#include "delay.h"

#define I2C_PageSize           16			/* AT24C08每页有8个字节 */
uint16_t EEPROM_ADDRESS;

/*
 * 函数名：at24c08_init
 * 描述  ：
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
void at24c08_init(void)
{
    i2c2_init();
    /* 根据头文件i2c_ee.h中的定义来选择EEPROM要写入的地址 */
    #ifdef EEPROM_Block0_ADDRESS
    /* 选择 EEPROM Block0 来写入 */
    EEPROM_ADDRESS = EEPROM_Block0_ADDRESS;
    #endif

    #ifdef EEPROM_Block1_ADDRESS  
	  /* 选择 EEPROM Block1 来写入 */
    EEPROM_ADDRESS = EEPROM_Block1_ADDRESS;
    #endif

    #ifdef EEPROM_Block2_ADDRESS  
	  /* 选择 EEPROM Block2 来写入 */
    EEPROM_ADDRESS = EEPROM_Block2_ADDRESS;
    #endif

    #ifdef EEPROM_Block3_ADDRESS  
	  /* 选择 EEPROM Block3 来写入 */
    EEPROM_ADDRESS = EEPROM_Block3_ADDRESS;
    #endif
}
/*
 * 函数名：i2c_ee_buffer_write
 * 描述  ：将缓冲区中的数据写到I2C EEPROM中
 * 输入  ：-p_buffer 缓冲区指针
 *         -write_addr 接收数据的EEPROM的地址
 *         -num_byte_write 要写入EEPROM的字节数
 * 输出  ：无
 * 返回  ：无
 * 调用  ：外部调用
 */
void i2c_ee_buffer_write(u8* p_buffer, u8 write_addr, u16 num_byte_write)
{
    u8 num_page = 0, num_single = 0, Addr = 0, count = 0;

    Addr = write_addr % I2C_PageSize;
    count = I2C_PageSize - Addr;
    num_page =  num_byte_write / I2C_PageSize;
    num_single = num_byte_write % I2C_PageSize;
 
    /* If write_addr is I2C_PageSize aligned  */
    if(Addr == 0){
        /* If num_byte_write < I2C_PageSize */
        if(num_page == 0){
            i2c_ee_page_write(p_buffer, write_addr, num_single);
            i2c_ee_wait_eeprom_stand_state();
        }
        /* If num_byte_write > I2C_PageSize */
        else{
            while (num_page--){
                i2c_ee_page_write(p_buffer, write_addr, I2C_PageSize); 
    	          i2c_ee_wait_eeprom_stand_state();
                write_addr +=  I2C_PageSize;
                p_buffer += I2C_PageSize;
            }
            if (num_single!=0){
                i2c_ee_page_write(p_buffer, write_addr, num_single);
                i2c_ee_wait_eeprom_stand_state();
			      }
         }
    }
    /* If write_addr is not I2C_PageSize aligned  */
    else{
        /* If num_byte_write < I2C_PageSize */
        if (num_page== 0){
            i2c_ee_page_write(p_buffer, write_addr, num_single);
            i2c_ee_wait_eeprom_stand_state();
        }
        /* If num_byte_write > I2C_PageSize */
        else{
            num_byte_write -= count;
            num_page =  num_byte_write / I2C_PageSize;
            num_single = num_byte_write % I2C_PageSize;	
            if (count != 0){  
                i2c_ee_page_write(p_buffer, write_addr, count);
                i2c_ee_wait_eeprom_stand_state();
                write_addr += count;
                p_buffer += count;
            }
						while (num_page--){
                i2c_ee_page_write(p_buffer, write_addr, I2C_PageSize);
                i2c_ee_wait_eeprom_stand_state();
                write_addr +=  I2C_PageSize;
                p_buffer += I2C_PageSize;
						}
            if (num_single != 0){
                i2c_ee_page_write(p_buffer, write_addr, num_single); 
                i2c_ee_wait_eeprom_stand_state();
						}
				}
		}  
}
/*
 * 函数名：i2c_ee_page_write
 * 描述  ：在EEPROM的一个写循环中可以写多个字节，但一次写入的字节数
 *         不能超过EEPROM页的大小。AT24C02每页有8个字节。
 * 输入  ：-p_buffer 缓冲区指针
 *         -write_addr 接收数据的EEPROM的地址 
 *         -num_byte_write 要写入EEPROM的字节数
 * 输出  ：无
 * 返回  ：无
 * 调用  ：外部调用
 */
void i2c_ee_page_write(u8* p_buffer, u8 write_addr, u8 num_byte_write)
{
    while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY)); // Added by Najoua 27/08/2008
    
     /* Send START condition */
    I2C_GenerateSTART(I2C2, ENABLE);
  
    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)); 
  
    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C2, EEPROM_ADDRESS, I2C_Direction_Transmitter);
  
    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));  

    /* Send the EEPROM's internal address to write to */    
    I2C_SendData(I2C2, write_addr);  

    /* Test on EV8 and clear it */
    while (! I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* While there is data to be written */
    while (num_byte_write--){
        /* Send the current byte */
        I2C_SendData(I2C2, *p_buffer); 

        /* Point to the next byte to be written */
        p_buffer++; 
  
        /* Test on EV8 and clear it */
        while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    }

    /* Send STOP condition */
    I2C_GenerateSTOP(I2C2, ENABLE);
}


/*
 * 函数名：I2C_EE_BufferRead
 * 描述  ：从EEPROM里面读取一块数据。 
 * 输入  ：-p_buffer 存放从EEPROM读取的数据的缓冲区指针。
 *         -write_addr 接收数据的EEPROM的地址。 
 *         -num_byte_write 要从EEPROM读取的字节数。
 * 输出  ：无
 * 返回  ：无
 * 调用  ：外部调用
 */
void I2C_EE_BufferRead(u8* p_buffer, u8 read_addr, u16 num_byte_read)
{  
    //*((u8 *)0x4001080c) |=0x80; 
    while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY)); // Added by Najoua 27/08/2008
    
    
    /* Send START condition */
    I2C_GenerateSTART(I2C2, ENABLE);
    //*((u8 *)0x4001080c) &=~0x80;
  
    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C2, EEPROM_ADDRESS, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
    /* Clear EV6 by setting again the PE bit */
    I2C_Cmd(I2C2, ENABLE);

    /* Send the EEPROM's internal address to write to */
    I2C_SendData(I2C2, read_addr);  

     /* Test on EV8 and clear it */
     while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
     /* Send STRAT condition a second time */  
    I2C_GenerateSTART(I2C2, ENABLE);
  
    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
  
    /* Send EEPROM address for read */
    I2C_Send7bitAddress(I2C2, EEPROM_ADDRESS, I2C_Direction_Receiver);
  
    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
  
    /* While there is data to be read */
    while (num_byte_read){
        if (num_byte_read == 1){
            /* Disable Acknowledgement */
            I2C_AcknowledgeConfig(I2C2, DISABLE);
      
            /* Send STOP Condition */
            I2C_GenerateSTOP(I2C2, ENABLE);
				}

        /* Test on EV7 and clear it */
        if (I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)){      
            /* Read a byte from the EEPROM */
            *p_buffer = I2C_ReceiveData(I2C1);

            /* Point to the next location where the byte read will be saved */
            p_buffer++; 
      
            /* Decrement the read bytes counter */
            num_byte_read--;        
         }   
    }

    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(I2C2, ENABLE);
}


/*
 * 函数名：i2c_ee_wait_eeprom_stand_state
 * 描述  ：Wait for EEPROM Standby state 
 * 输入  ：无
 * 输出  ：无
 * 返回  ：无
 * 调用  ： 
 */
void i2c_ee_wait_eeprom_stand_state(void)      
{
    u16 SR1_Tmp = 0;
    do{
        /* Send START condition */
        I2C_GenerateSTART(I2C2, ENABLE);
        /* Read I2C1 SR1 register */
        SR1_Tmp = I2C_ReadRegister(I2C2, I2C_Register_SR1);
        /* Send EEPROM address for write */
        I2C_Send7bitAddress(I2C2, EEPROM_ADDRESS, I2C_Direction_Transmitter);
    }while(!(I2C_ReadRegister(I2C2, I2C_Register_SR1) & 0x0002));
  
     /* Clear AF flag */
    I2C_ClearFlag(I2C2, I2C_FLAG_AF);
    /* STOP condition */    
    I2C_GenerateSTOP(I2C2, ENABLE); 
}



/*******************************************************************************
* Function Name  : IIC_Byte_Write
* Description    : 对eerpom（at24c02）的写操作，是一个字节的写操作。
* Input          : u8* pbuffer, u8 Word_Address （这里的地址要注意一下规则）
* Output         : None
* Return         : None 
*******************************************************************************//**/
void IIC_Byte_Write( u8 pBuffer, u8 Word_Address)
{
	 //IIC_Wait_EEprom();                 /*等待总线空闲，等待设备器件空闲（其中需要软件清除SR1的AF位，即应答成功）*/
	
	 I2C_GenerateSTART(I2C2, ENABLE);   /*发送一个s，也就是起始信号，因为前面的函数没有终止，这次开始也是需要重新的一个开始信号*/
	 //EV5事件
	 while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT)); /*判断EV5,看下函数定义可以发现，该事件是SB=1，MSL=1，BUSY=1 
	                                                            意思是起始条件已经发送了，然后是主模式，总线在通讯*/
	 
	 I2C_Send7bitAddress(I2C2, EEPROM_ADDRESS, I2C_Direction_Transmitter);  /*发送器件地址，最后一个参数表示地址bit0为0，意思
	                                                                        是写操作，同时由于写了DR寄存器，故会清除SB位变成0*/
	
	 //EV6事件 EV8_1（该事件判断同时判断了EV8_1事件）
	 while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));/*判断EV6和EV8_1的条件，此时SB=0，ADDR=1地址发送结束，
	                                                                      TXE=1数据寄存器DR为空，BUSY=1总不空闲,MSL=1主模式,TRA=1
	                                                                        数据已经发送了（因为是写操作，其实是地址数据已经发送了）
	                                                                        如果是主收模式，这里的EV6，TRA=1表示数据还没收到，0表示
	                                                                        收到数据，注意这里TRA=1表示已经发送了，ADDR=1才是发送完成
	                                                                        了,做完该事件，ADDR=0了又*/                                                                    
	 //EV8_1事件
	 /*while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED));*/ /*用判断EV8_2事件的条件,BTF=1表示字节发送完成，此时DR为0而且
	                                                                      要发新数据了，8_1事件中没有数据要发送，这里和上面操作这
	                                                                      句话会让ADDR=0(操作了SR1 SR2),不要加这句话，因为这里读了
	                                                                      SR1下面操作DR了会清除BTF为0，EV8事件倒是可以发生，
	                                                                      但是没有这句话效果一样，这里不该有这句话*/  
	                                                                                                                           
	 
	 I2C_SendData(I2C2,Word_Address);      /*EV8_1事件，因为这一步时候DR已经为空，该事件是写入data1，对于EEPROM来说这个data1是要写入
	                                      字节的的地址，data2是要写入的内容，data1为8位1k的有效字节是低7位，2kbit的有效字节是8位，32页，
	                                      每页8个字节，一共2k位，16kbit需要11位？怎么送？用硬件的A1，2，3接GPIO来选择存储的页*/
	                         
	 
	 //EV8事件
	 while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTING)); /*为下一步执行EV8事件，判断TXE=1 DR寄存器为空，准备写入DR寄存器
	                                                                 data2，注意此时也判定TRA=1意思是data1已经发送了不表示发送完成，
	                                                                 移位寄存器非空表示数还在发，另外BTF=0说明data1字节发送没完成，
																																	 NOSTRETCH为0时候，BTF=1还表示新数据要被发送了（意味着字节发送）
	                                                                 完成了但是新数据还没有写入到DR里面，这里的EV8事件对于的一段一直
																																	 是有数据发送的，不存在BTF=1的情况*/ 
																																	 																															                                                                  
	 I2C_SendData(I2C2,pBuffer);                                    /*数据寄存器DR为空，这里是写入data2，该步骤隶属于EV8事件*/
	
	/*EV8_2事件（这里就发送两个data，所以就只有一个EV8事件，
	  EV8_1—data1，EV8_1—data2，EV8_2和EV8的区别是检测的
		差个BTF,且EV8_2不写DR而是程序要求停止）*/
	  
	 while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED)); /*EV8_2的事件判断和EV8事件不一样，TRA=1表示data2已经发送了，
	                                                                  BTF=1字节发送结束，说明这一时刻没有字节在发送，其实表示在
																																		data2正在发送的时候，没有data3写入到DR里面， 然后现在该轮到要
	                                                                  发送data3了，但是DR里面是空的（其实发送data2的时候，中间某时
																																		刻DR就已经空了）*/
	                                                                																																
	 I2C_GenerateSTOP(I2C2,ENABLE);                                  /*EV8_2事件中的程序写停止*/
}

/*******************************************************************************
* Function Name  : IIC_Byte_Read
* Description    : 对eerpom（at24c02）的读操作，是一个字节的读操作，给定一个地址Word_Address然后读取其值并将值赋给pRead。
* Input          : u8* pRead, u8 Word_Address（这里的地址要注意一下规则1,2kbit的就是8位数就能表示满了，4,8,16k的要用硬件引脚A0，1,2，来由GPIO选择页）
* Output         : None
* Return         : None 
*******************************************************************************//**/
void IIC_Byte_Read( u8* pRead, u8 Word_Address)
{
	/*首先写操作，发送设备地址写操作，找到设备应答了，写入要读数据的地址*/
   //IIC_Wait_EEprom(); 
   I2C_GenerateSTART(I2C2, ENABLE);
	
	 //EV5事件
   while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT));
   I2C_Send7bitAddress(I2C2, EEPROM_ADDRESS, I2C_Direction_Transmitter); 
	
	 //EV6 EV8_1事件（该事件判断同时判断了EV8_1事件）
	 while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	 {}
	 I2C_SendData(I2C2,Word_Address); 
		 
	 while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED)); //采用EV8_2的判断条件反应此时已经收到ACK，Word_Address发送完成。
	 
	/*给Start条件，发送设备地址读操作，找到设备应答了，收数据，主机不应答，终止*/
	 I2C_GenerateSTART(I2C2, ENABLE);
		 
	//EV5事件
   while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT));
   I2C_Send7bitAddress(I2C2, EEPROM_ADDRESS, I2C_Direction_Receiver ); 
	
  //EV6事件
	 while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
		 
	//EV6_1事件，没有标志位，要设置ACK失能和停止位产生
	 I2C2->CR1 &= 0xFBFF ;   //失能ACK
	 I2C2->CR1 |= 0x0020 ;   //使能Stop
	
	//EV7事件，读DR寄存器
	 while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED));
	 * pRead = I2C2->DR;		 
}

/*
 * 函数名：read_eepom_data
 * 描述  ：
 * 输入  ：无
 * 输出  ：无
 * 返回  ：无
 * 调用  ： 
 */
void write_eepom_data(uint32_t data, uint32_t add)
{
    IIC_Byte_Write( BYTE0(data), add);
    delay_ms(5);
	  IIC_Byte_Write( BYTE1(data), add + 1);
	  delay_ms(5);
	  IIC_Byte_Write( BYTE2(data), add + 2);
	  delay_ms(5);
	  IIC_Byte_Write( BYTE3(data), add + 3);
	   delay_ms(5);    
	  g_load_mode.write_read_eerom_flag = 0;
}

/*
 * 函数名：read_eepom_data
 * 描述  ：
 * 输入  ：无
 * 输出  ：无
 * 返回  ：无
 * 调用  ： 
 */
uint32_t read_eepom_data(uint32_t add)
{
	  uint32_t read_data;
	  u8 data[4];
    IIC_Byte_Read(&data[0], add);
    delay_ms(5);
	  IIC_Byte_Read(&(data[1]), add + 1);
	  delay_ms(5);
	  IIC_Byte_Read(&(data[2]), add + 2);
	  delay_ms(5);
	  IIC_Byte_Read(&(data[3]), add + 3);
	  delay_ms(5);
    BYTE0(read_data) = data[0];
    BYTE1(read_data) = data[1];
	  BYTE2(read_data) = data[2];
	  BYTE3(read_data) = data[3];
	  g_load_mode.write_read_eerom_flag = 0;
	  return read_data;
}
/*
 * 函数名：eepom_test
 * 描述  ：
 * 输入  ：无
 * 输出  ：无
 * 返回  ：无
 * 调用  ： 
 */
#define  EEP_Firstpage      0x00
u8 I2c_Buf_Write[2] = {1, 2};
u8 I2c_Buf_Read[2] = {0, 0};
void eepom_test()
{
	
//`	IIC_Byte_Write( I2c_Buf_Write[1], EEP_Firstpage);
	
	IIC_Byte_Read( I2c_Buf_Read, EEP_Firstpage);
	
	
}
