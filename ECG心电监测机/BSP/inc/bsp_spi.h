#ifndef __BSP_SPI_H
#define __BSP_SPI_H
#include "sys.h"


u8 SPI1_ReadWriteByte(u8 TxData);//SPI总线读写一个字节

void SPI1_SetSpeed(u8 SpeedSet); //设置SPI速度   			

void SPI1_Init(void);			 //初始化SPI口

u8 SPI2_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
 			
void SPI2_Init(void);			 //



#endif



