#ifndef __LV_CHART_INIT_H__
#define __LV_CHART_INIT_H__
#include "sys.h"

typedef struct
{
    u8 tdiv;
    u8 vdiv;
} chart_type;

typedef struct System_State
{
	uint8_t SampleStartFlag;//开始采样标志位
	uint8_t ChartShowStartFlag;//图表显示标志位
	uint8_t StartSmoothFilterFlag;
}System_State_dev;

//函数申明
void lv_chart_init(void);


#endif


