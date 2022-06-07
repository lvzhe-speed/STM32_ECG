#ifndef  __ECG_LPF_FILTER_H
#define  __ECG_LPF_FILTER_H
#include "sys.h"
#include "arm_math.h"


 #define NUM_TAPS             51    /* 滤波器系数个数 */



void arm_fir_Init(void);
void arm_fir_f32_lp(void);
float alg(float new_val);

#endif

