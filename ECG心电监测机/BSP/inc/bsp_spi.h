#ifndef __BSP_SPI_H
#define __BSP_SPI_H
#include "sys.h"


u8 SPI1_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�

void SPI1_SetSpeed(u8 SpeedSet); //����SPI�ٶ�   			

void SPI1_Init(void);			 //��ʼ��SPI��

u8 SPI2_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�
 			
void SPI2_Init(void);			 //



#endif



