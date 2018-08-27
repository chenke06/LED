/**********************************************************************************
 * 文件名  ：spi1.c
 * 描述    ：spi 底层应用函数库         
 * 硬件连接 ----------------------------
 *         | PA4-SPI1-NSS  : CS         |
 *         | PA5-SPI1-SCK  : CLK        |
 *         | PA6-SPI1-MISO : DO         |
 *         | PA7-SPI1-MOSI : DIO        |
 *          ----------------------------
 * 库版本  ：ST3.5.0
**********************************************************************************/
#include "spi2.h"

static void spi2_gpio_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
  
/* Enable SPI1 and GPIO clocks */
/*!< SPI_FLASH_SPI_CS_GPIO, SPI_FLASH_SPI_MOSI_GPIO, 
       SPI_FLASH_SPI_MISO_GPIO, SPI_FLASH_SPI_DETECT_GPIO 
       and SPI_FLASH_SPI_SCK_GPIO Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

/*!< SPI_FLASH_SPI Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
 
  
/*!< Configure SPI_FLASH_SPI pins: SCK */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

/*!< Configure SPI_FLASH_SPI pins: MISO */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

/*!< Configure SPI_FLASH_SPI pins: MOSI */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

/*!< Configure SPI_FLASH_SPI_CS_PIN pin: SPI_FLASH Card CS pin */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*
 * 函数名：I2C_Configuration
 * 描述  ：I2C 工作模式配置
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void spi2_mode_config(void)
{
    SPI_InitTypeDef  SPI_InitStructure;

/* SPI1 configuration */
// W25X16: data input on the DIO pin is sampled on the rising edge of the CLK. 
// Data on the DO and DIO pins are clocked out on the falling edge of CLK.
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);

/* Enable SPI1  */
    SPI_Cmd(SPI2, ENABLE);
}


/*
 * 函数名：SPI1_Init
 * 描述  ：
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 */
void spi2_init(void)
{
    spi2_gpio_config();
    spi2_mode_config();	
}
