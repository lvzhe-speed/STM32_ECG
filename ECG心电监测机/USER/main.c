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

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
    delay_init(168);  //初始化延时函数
    uart_init(115200);      //初始化串口波特率为115200


    LED_Init();                 //初始化LED
    LCD_Init();                 //LCD初始化
    KEY_Init();                 //按键初始化
    FSMC_SRAM_Init();       //初始化外部sram
    ADS1292_Init(); //初始化ads1292
    while(Set_ADS1292_Collect(0))//0 正常采集  //1 1mV1Hz内部侧试信号 //2 内部短接噪声测试
    {
        printf("1292寄存器设置失败\r\n");
        delay_s(1);
    }
    tp_dev.init();              //触摸屏初始化

    lv_init();                      //lvgl系统初始化
    lv_port_disp_init();    //lvgl显示接口初始化,放在lv_init()的后面
    lv_port_indev_init();   //lvgl输入接口初始化,放在lv_init()的后面
    ADS1292_Recv_Start();
    lv_chart_init();    //初始化图表
	EcgUsartSendInit();
	ECGDataFIFOInit();
	arm_fir_Init();
	ecg_start_heart_rate();
	lmt70_init();
	TIM3_Int_Init(999,83);  //定时器初始化(1ms中断),用于给lvgl提供1ms的心跳节拍
    while(1)
    {
        tp_dev.scan(0);
        lv_task_handler();
		arm_fir_f32_lp();
    }

}



