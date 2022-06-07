#ifndef __BSP_DMA_H
#define	__BSP_DMA_H	   
#include "sys.h"

void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx,u32 par,u32 mar,u16 ndtr);
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr);
	   
#endif




