#include "bsp_timer.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "bsp_led.h"  


//高级 TIM1 TIM8
//通用 TIM2 TIM3 TIM4 TIM5
//基本 TIM6 TIM7

//定时器2初始化
void TIM2_Init(u16 arr,u16 psc)      
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //TIM2时钟使能

    TIM_TimeBaseStructure.TIM_Period = arr-1;; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler =psc-1; //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);

    TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //清除更新中断请求位
    TIM_ITConfig( TIM2, TIM_IT_Update, ENABLE );//TIM2 允许更新
    TIM_Cmd(TIM2, ENABLE);
}



