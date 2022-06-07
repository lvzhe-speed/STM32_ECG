#ifndef __ECG_HEART_RATE_H
#define __ECG_HEART_RATE_H
#include "sys.h"
#include "lvgl.h"
typedef struct {
    int vmax;//�����ֵ
    int vmin;
    float rate;//����
    u8 firstBeat;//��һ����
    u16 count;//����
    u8 flag;//��־λ
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

extern heart_rate_type hr;//����

void ecg_start_heart_rate(void);
void ecg_heart_rate(int data);
void set_data_into_heart_buff(int data);

#endif

