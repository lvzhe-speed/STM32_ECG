#ifndef __BSP_LED_H
#define __BSP_LED_H	 
#include "sys.h"


//LED�˿ڶ���
#define LED0 PFout(9)	// DS0
#define LED1 PFout(10)	// DS1	

#define LEDON 0
#define LEDOFF 1

void LED_Init(void);//��ʼ��

		 				    
#endif
