#ifndef __BSP_ADS1118_H
#define __BSP_ADS1118_H

#include "stm32f4xx.h"


#define ADS118_CS			PFout(6)

int ads1118_init(void);
int16_t ads1118_get_voltage(uint8_t adc_channel);
float ads1118_get_temperature(void);

#endif




