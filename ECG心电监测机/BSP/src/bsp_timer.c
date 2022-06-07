#include "bsp_timer.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "bsp_led.h"  


//�߼� TIM1 TIM8
//ͨ�� TIM2 TIM3 TIM4 TIM5
//���� TIM6 TIM7

//��ʱ��2��ʼ��
void TIM2_Init(u16 arr,u16 psc)      
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //TIM2ʱ��ʹ��

    TIM_TimeBaseStructure.TIM_Period = arr-1;; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler =psc-1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);

    TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //��������ж�����λ
    TIM_ITConfig( TIM2, TIM_IT_Update, ENABLE );//TIM2 �������
    TIM_Cmd(TIM2, ENABLE);
}



