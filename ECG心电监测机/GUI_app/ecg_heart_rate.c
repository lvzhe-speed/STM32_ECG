#include "ecg_heart_rate.h"
#include "bsp_timer.h"
#include "bsp_loop_buffer.h"
#include "lvgl.h"
#include "stdio.h"
#include "string.h"
#include "ecg_data_process.h"
#include "bsp_ads1292.h"

LOOP_BUFFER heart_rate_fifo;


#define  Heart_MIN   (-8388607)
#define  Heart_MAX   (8388607)
heart_rate_type hr={
	.flag=NotStart
};//心率
int16_t thresh=0;

#define HR_BUFF_SIZE  250

int rate_buffer[HR_BUFF_SIZE];
/**
  * @Brief 开始测心率
  * @Call
  * @Param
  * @Note
  * @Retval
  */
void ecg_start_heart_rate(void)
{
    hr.count=0;
    hr.firstBeat=false;
    hr.flag=StartDetected;
	hr.QRS_cnt=0;
	hr.QRS=0;
    hr.vmax=Heart_MIN;
    hr.vmin=Heart_MAX;
    hr.rate=0;
    thresh=0;
    TIM2_Init(1999,83);//开启定时2ms
    loop_buffer_create(&heart_rate_fifo,rate_buffer,HR_BUFF_SIZE);
}


/**
  * @Brief 将数据写入心率缓存
  * @Call
  * @Param
  * @Note
  * @Retval
  */
void set_data_into_heart_buff(int data)
{
	if(hr.flag!=NotStart)
    loop_buffer_write(&heart_rate_fifo,&data,4);//在主函数中被调用

}


void stop_heart_rate()
{
    hr.flag=NotStart;
    loop_buffer_free(&heart_rate_fifo);
    TIM_Cmd(TIM2,DISABLE);
}




/****************************************
**对数组进行差分
*****************************************/
void Diff_Arrray(
    int16_t *DiffArray,     //差分后的数组
    const uint16_t *OrgArray,// 原始数组
    const uint16_t OrgArrayNum		//原始数组的长度
)
{
    for( uint16_t i = 0; i <= OrgArrayNum-2; i++ )
    {
        DiffArray[i] = OrgArray[i + 1] - OrgArray[i];
    }

    return;
}

#define DATA_NUM_CAL_HR 4 //用于计算HR的数据点数 
#define  SAMPLE_RATE    250 //采样率
uint16_t DataArrayCalHR[DATA_NUM_CAL_HR] = {0}; //用于计算心率的数据
int16_t DiffDataArrayCalHR[DATA_NUM_CAL_HR - 1] = {0}; //用于计算心率的数据的差分，有符号
u8 QRScntflag=false;
//#define ABS(x) ((x)>=0?(x):-(x))
/**
  * @Brief 测量心率
  * @Call
  * @Param
  * @Note
  * @Retval
  */
void ecg_heart_rate(int data)
{
	int Signal=data;
			
	if(Signal>hr.vmax)
		hr.vmax=Signal;
	if(Signal<hr.vmin)
		hr.vmin=Signal;
	thresh=hr.vmax-(hr.vmax-hr.vmin)/5;
	  
	for( uint16_t i = 0; i <= DATA_NUM_CAL_HR - 2; i++ )
    {
        DataArrayCalHR[i] =	DataArrayCalHR[i + 1];
    }
	 DataArrayCalHR[DATA_NUM_CAL_HR - 1] = Signal;
    Diff_Arrray( DiffDataArrayCalHR, DataArrayCalHR, DATA_NUM_CAL_HR );     //差分
	
	if(hr.flag==StartDetected){
		uint8_t FlagAllDiffRise = true;

            for( uint16_t i = 0; i <= DATA_NUM_CAL_HR - 2; i++ ) //判断波形是否一直上升
            {
                if( DiffDataArrayCalHR[i] <= 0 )
                {
                    FlagAllDiffRise = false;
                    break;
                }
            }
			if(FlagAllDiffRise==true){
				hr.flag=QWave;
			}
	}
    else if(hr.flag==QWave)//已经找Q波
    {
		if(DataArrayCalHR[DATA_NUM_CAL_HR-1]>thresh){
			if(hr.count>125){
				if( hr.firstBeat==true )//如果已经找到 过R波
				{
				hr.rate=(float)60*SAMPLE_RATE/(hr.count);
				hr.count=0;//清除计数
				hr.flag=RWave;
				QRScntflag=true;
				} else if(hr.firstBeat==false) {
				hr.firstBeat=true;
				hr.count=0;//清除计数
				hr.flag=RWave;
				QRScntflag=true;
				}
			}			
		}
   }
	 else if(hr.flag==RWave ){
		if(DiffDataArrayCalHR[0]<-(hr.vmax-hr.vmin)/5){
				hr.flag=SWave;
		}
	}
	else if(hr.flag==SWave){
			if(hr.QRS_cnt<15){
			hr.flag=StartDetected;
			hr.QRS=hr.QRS_cnt*22*100/SAMPLE_RATE;
			hr.QRS_cnt=0;
			QRScntflag=false;
			}else {
			hr.flag=StartDetected;
			hr.QRS=0;
			hr.QRS_cnt=0;
			QRScntflag=false;
			}
	}
	
	
		if(hr.count>420){
				hr.firstBeat=false;
				hr.flag=StartDetected;
				hr.vmax=Heart_MIN;
				hr.vmin=Heart_MAX;
				hr.rate=0;
				thresh=0;
				hr.count=0;//清除计数
			}
	
}




int data;
void TIM2_IRQHandler(void)
{
     if(TIM2->SR&TIM_IT_Update)//溢出中断
    {
        if(hr.flag!=NotStart) {
            if(loop_buffer_read(&heart_rate_fifo,&data,4)!=0) {
                ecg_heart_rate(data);//心率值处理函数放在定时器中处理，每TIMER(2ms)处理一次
			 hr.count++;
		if(QRScntflag==true)hr.QRS_cnt++;
            }
        }
        TIM2->SR = (uint16_t)~TIM_IT_Update;
    }
}






