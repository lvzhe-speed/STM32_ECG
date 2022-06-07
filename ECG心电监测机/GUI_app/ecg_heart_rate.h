#ifndef __ECG_HEART_RATE_H
#define __ECG_HEART_RATE_H
#include "sys.h"
#include "lvgl.h"
typedef struct {
    int vmax;//脉冲峰值
    int vmin;
    float rate;//心率
    u8 firstBeat;//第一节拍
    u16 count;//计数
    u8 flag;//标志位
	int QRS;
	int QRS_cnt;
} heart_rate_type;


typedef enum
{
	StartDetected=0x01,
    QWave  = 0x02,
    RWave = 0X03,
	SWave = 0x04,
	NotStart=0x06
} HR_STAT;

extern heart_rate_type hr;//心率

void ecg_start_heart_rate(void);
void ecg_heart_rate(int data);
void set_data_into_heart_buff(int data);

#endif

