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
};//����
int16_t thresh=0;

#define HR_BUFF_SIZE  250

int rate_buffer[HR_BUFF_SIZE];
/**
  * @Brief ��ʼ������
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
    TIM2_Init(1999,83);//������ʱ2ms
    loop_buffer_create(&heart_rate_fifo,rate_buffer,HR_BUFF_SIZE);
}


/**
  * @Brief ������д�����ʻ���
  * @Call
  * @Param
  * @Note
  * @Retval
  */
void set_data_into_heart_buff(int data)
{
	if(hr.flag!=NotStart)
    loop_buffer_write(&heart_rate_fifo,&data,4);//���������б�����

}


void stop_heart_rate()
{
    hr.flag=NotStart;
    loop_buffer_free(&heart_rate_fifo);
    TIM_Cmd(TIM2,DISABLE);
}




/****************************************
**��������в��
*****************************************/
void Diff_Arrray(
    int16_t *DiffArray,     //��ֺ������
    const uint16_t *OrgArray,// ԭʼ����
    const uint16_t OrgArrayNum		//ԭʼ����ĳ���
)
{
    for( uint16_t i = 0; i <= OrgArrayNum-2; i++ )
    {
        DiffArray[i] = OrgArray[i + 1] - OrgArray[i];
    }

    return;
}

#define DATA_NUM_CAL_HR 4 //���ڼ���HR�����ݵ��� 
#define  SAMPLE_RATE    250 //������
uint16_t DataArrayCalHR[DATA_NUM_CAL_HR] = {0}; //���ڼ������ʵ�����
int16_t DiffDataArrayCalHR[DATA_NUM_CAL_HR - 1] = {0}; //���ڼ������ʵ����ݵĲ�֣��з���
u8 QRScntflag=false;
//#define ABS(x) ((x)>=0?(x):-(x))
/**
  * @Brief ��������
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
    Diff_Arrray( DiffDataArrayCalHR, DataArrayCalHR, DATA_NUM_CAL_HR );     //���
	
	if(hr.flag==StartDetected){
		uint8_t FlagAllDiffRise = true;

            for( uint16_t i = 0; i <= DATA_NUM_CAL_HR - 2; i++ ) //�жϲ����Ƿ�һֱ����
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
    else if(hr.flag==QWave)//�Ѿ���Q��
    {
		if(DataArrayCalHR[DATA_NUM_CAL_HR-1]>thresh){
			if(hr.count>125){
				if( hr.firstBeat==true )//����Ѿ��ҵ� ��R��
				{
				hr.rate=(float)60*SAMPLE_RATE/(hr.count);
				hr.count=0;//�������
				hr.flag=RWave;
				QRScntflag=true;
				} else if(hr.firstBeat==false) {
				hr.firstBeat=true;
				hr.count=0;//�������
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
				hr.count=0;//�������
			}
	
}




int data;
void TIM2_IRQHandler(void)
{
     if(TIM2->SR&TIM_IT_Update)//����ж�
    {
        if(hr.flag!=NotStart) {
            if(loop_buffer_read(&heart_rate_fifo,&data,4)!=0) {
                ecg_heart_rate(data);//����ֵ���������ڶ�ʱ���д���ÿTIMER(2ms)����һ��
			 hr.count++;
		if(QRScntflag==true)hr.QRS_cnt++;
            }
        }
        TIM2->SR = (uint16_t)~TIM_IT_Update;
    }
}






