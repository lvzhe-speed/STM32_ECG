#include "bsp_spi.h"
					  
//SPI口初始化
//这里针是对SPI1的初始化
void SPI1_Init(void)
{	 
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);   //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);      //使能SPI1时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;  //管脚SPI复用 (<STM32F4为>PA5:SPI1_SCK,PA6:SPI1_MISO,PA7:SPI1_MOSI)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;         //复用 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          //上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
	GPIO_Init(GPIOA, &GPIO_InitStructure);                      //配置到管脚
	
	GPIO_PinAFConfig(GPIOA,5,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,6,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,7,GPIO_AF_SPI1);
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;         //SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;          //设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;      //8位帧数据结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;             //同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;          //同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                 //NSS 信号由硬件(NSS管脚)还是软件(使用 SSI位)管理:内部NSS信号由SSI位控制 
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;  //波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;       //数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;                  //CRC计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);                               //配置到SPIx寄存器
			
	SPI_Cmd(SPI1, ENABLE);    //使能SPI外设	 
 SPI1_ReadWriteByte(0xff);//启动传输
}   
//SPI1速度设置函数
//SPI速度=fAPB2/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
//fAPB2时钟一般为84Mhz：
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
	SPI1->CR1&=0XFFC7;//位3-5清零，用来设置波特率
	SPI1->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度 
	SPI_Cmd(SPI1,ENABLE); //使能SPI1
} 
//SPI1 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI1_ReadWriteByte(u8 TxData)
{		 			 
 
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空  
	
	SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个byte  数据
		
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个byte  
 
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据	
 		    
}




//SPI口初始化
//这里针是对SPI1的初始化
void SPI2_Init(void)
{	 

	
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);   //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);      //使能SPI1时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;  //管脚SPI复用 (<STM32F4为>PA5:SPI1_SCK,PA6:SPI1_MISO,PA7:SPI1_MOSI)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;         //复用 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          //上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
	GPIO_Init(GPIOB, &GPIO_InitStructure);                      //配置到管脚
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;  //管脚SPI复用 (<STM32F4为>PA5:SPI1_SCK,PA6:SPI1_MISO,PA7:SPI1_MOSI)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;         //复用 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          //上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
	GPIO_Init(GPIOC, &GPIO_InitStructure);      
	
	GPIO_PinAFConfig(GPIOB,13,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOC,2,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOC,3,GPIO_AF_SPI2);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;         //SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;          //设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;      //8位帧数据结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;             //同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;          //同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                 //NSS 信号由硬件(NSS管脚)还是软件(使用 SSI位)管理:内部NSS信号由SSI位控制 
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;  //波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;       //数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;                  //CRC计算的多项式
	SPI_Init(SPI2, &SPI_InitStructure);                               //配置到SPIx寄存器
			
	SPI_Cmd(SPI2, ENABLE);    //使能SPI外设	 
 SPI2_ReadWriteByte(0xff);//启动传输
}   


//SPI1 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI2_ReadWriteByte(u8 TxData)
{		 			 
 
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空  
	
	SPI_I2S_SendData(SPI2, TxData); //通过外设SPIx发送一个byte  数据
		
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个byte  
 
	return SPI_I2S_ReceiveData(SPI2); //返回通过SPIx最近接收的数据	
 		    
}




