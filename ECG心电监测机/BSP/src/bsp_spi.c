#include "bsp_spi.h"
					  
//SPI�ڳ�ʼ��
//�������Ƕ�SPI1�ĳ�ʼ��
void SPI1_Init(void)
{	 
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);   //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);      //ʹ��SPI1ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;  //�ܽ�SPI���� (<STM32F4Ϊ>PA5:SPI1_SCK,PA6:SPI1_MISO,PA7:SPI1_MOSI)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;         //���� 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
	GPIO_Init(GPIOA, &GPIO_InitStructure);                      //���õ��ܽ�
	
	GPIO_PinAFConfig(GPIOA,5,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,6,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,7,GPIO_AF_SPI1);
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;         //SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;          //����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;      //8λ֡���ݽṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;             //ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;          //ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                 //NSS �ź���Ӳ��(NSS�ܽ�)�������(ʹ�� SSIλ)����:�ڲ�NSS�ź���SSIλ���� 
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;  //������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;       //���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;                  //CRC����Ķ���ʽ
	SPI_Init(SPI1, &SPI_InitStructure);                               //���õ�SPIx�Ĵ���
			
	SPI_Cmd(SPI1, ENABLE);    //ʹ��SPI����	 
 SPI1_ReadWriteByte(0xff);//��������
}   
//SPI1�ٶ����ú���
//SPI�ٶ�=fAPB2/��Ƶϵ��
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
//fAPB2ʱ��һ��Ϊ84Mhz��
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
	SPI1->CR1&=0XFFC7;//λ3-5���㣬�������ò�����
	SPI1->CR1|=SPI_BaudRatePrescaler;	//����SPI1�ٶ� 
	SPI_Cmd(SPI1,ENABLE); //ʹ��SPI1
} 
//SPI1 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI1_ReadWriteByte(u8 TxData)
{		 			 
 
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������  
	
	SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ��byte  ����
		
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�������һ��byte  
 
	return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����	
 		    
}




//SPI�ڳ�ʼ��
//�������Ƕ�SPI1�ĳ�ʼ��
void SPI2_Init(void)
{	 

	
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);   //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);      //ʹ��SPI1ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;  //�ܽ�SPI���� (<STM32F4Ϊ>PA5:SPI1_SCK,PA6:SPI1_MISO,PA7:SPI1_MOSI)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;         //���� 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
	GPIO_Init(GPIOB, &GPIO_InitStructure);                      //���õ��ܽ�
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;  //�ܽ�SPI���� (<STM32F4Ϊ>PA5:SPI1_SCK,PA6:SPI1_MISO,PA7:SPI1_MOSI)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;         //���� 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
	GPIO_Init(GPIOC, &GPIO_InitStructure);      
	
	GPIO_PinAFConfig(GPIOB,13,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOC,2,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOC,3,GPIO_AF_SPI2);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;         //SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;          //����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;      //8λ֡���ݽṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;             //ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;          //ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                 //NSS �ź���Ӳ��(NSS�ܽ�)�������(ʹ�� SSIλ)����:�ڲ�NSS�ź���SSIλ���� 
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;  //������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;       //���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;                  //CRC����Ķ���ʽ
	SPI_Init(SPI2, &SPI_InitStructure);                               //���õ�SPIx�Ĵ���
			
	SPI_Cmd(SPI2, ENABLE);    //ʹ��SPI����	 
 SPI2_ReadWriteByte(0xff);//��������
}   


//SPI1 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI2_ReadWriteByte(u8 TxData)
{		 			 
 
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������  
	
	SPI_I2S_SendData(SPI2, TxData); //ͨ������SPIx����һ��byte  ����
		
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�������һ��byte  
 
	return SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����	
 		    
}




