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
	uint8_t SampleStartFlag;//��ʼ������־λ
	uint8_t ChartShowStartFlag;//ͼ����ʾ��־λ
	uint8_t StartSmoothFilterFlag;
}System_State_dev;

//��������
void lv_chart_init(void);


#endif


