#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"
#include "touch.h"
#include "timer.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "sram.h"
#include "bsp_ads1292.h"
#include "lv_chart_init.h"
#include "ecg_data_process.h"
#include "ecg_lpf_filter.h"
#include "ecg_heart_rate.h"
#include "lmt70.h"

int main(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
    delay_init(168);  //��ʼ����ʱ����
    uart_init(115200);      //��ʼ�����ڲ�����Ϊ115200


    LED_Init();                 //��ʼ��LED
    LCD_Init();                 //LCD��ʼ��
    KEY_Init();                 //������ʼ��
    FSMC_SRAM_Init();       //��ʼ���ⲿsram
    ADS1292_Init(); //��ʼ��ads1292
    while(Set_ADS1292_Collect(0))//0 �����ɼ�  //1 1mV1Hz�ڲ������ź� //2 �ڲ��̽���������
    {
        printf("1292�Ĵ�������ʧ��\r\n");
        delay_s(1);
    }
    tp_dev.init();              //��������ʼ��

    lv_init();                      //lvglϵͳ��ʼ��
    lv_port_disp_init();    //lvgl��ʾ�ӿڳ�ʼ��,����lv_init()�ĺ���
    lv_port_indev_init();   //lvgl����ӿڳ�ʼ��,����lv_init()�ĺ���
    ADS1292_Recv_Start();
    lv_chart_init();    //��ʼ��ͼ��
	EcgUsartSendInit();
	ECGDataFIFOInit();
	arm_fir_Init();
	ecg_start_heart_rate();
	lmt70_init();
	TIM3_Int_Init(999,83);  //��ʱ����ʼ��(1ms�ж�),���ڸ�lvgl�ṩ1ms����������
    while(1)
    {
        tp_dev.scan(0);
        lv_task_handler();
		arm_fir_f32_lp();
    }

}



