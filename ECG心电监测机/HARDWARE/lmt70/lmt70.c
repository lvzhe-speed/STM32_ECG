#include "lmt70.h"
#include "bsp_ads1118.h"

int lmt70_init(void)
{
	ads1118_init();
	
	return 0;
}

float lmt70_get_temperature(void)
{
	const float v0 = 1375.219;
	const float t0 = -55.0;
	const float v1 = 302.785;
	const float t1 = 150.0;
	const float k = (t1 - t0) / (v1 - v0); 
	const float b = t0 - k * v0;
	
	int lmt70_output_voltage = 0;
	float lmt70_temperature = 0;
	
	lmt70_output_voltage = ads1118_get_voltage(0) * 0.125;
	
	lmt70_temperature = k * lmt70_output_voltage + b;
	
	
	return lmt70_temperature;
}









