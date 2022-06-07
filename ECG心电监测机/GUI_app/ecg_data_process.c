#include "ecg_data_process.h"
#include "bsp_ads1292.h"
#include "lv_chart_init.h"
#include "lvgl.h"
#include "bsp_dma.h"
#include "usart.h"
ECG_TYPE ecg_info;
ECG_Graph_Type ecg_graph= {0,0,200,GRAPH};

extern System_State_dev state_pcb;

u8 ads1292_Cache[9];    //数据缓冲区
//读取72位的数据1100+LOFF_STAT[4:0]+GPIO[1:0]+13个0+2CHx24位，共9字节
//  1100    LOFF_STAT[4         3           2           1           0   ]   //导联脱落相关的信息在LOFF_STAT寄存器里
//                    RLD     1N2N         1N2P         1N1N    1N1P
//  例  C0 00 00 FF E1 1A FF E1 52

int32_t AdsInBuffer[FIFO_SIZE];
int32_t EcgOutBuffer[FIFO_SIZE];

FIFO_TypeDef InFifoDev;
FIFO_TypeDef OutFifoDev;

void ECGDataFIFOInit(void)
{
	u8 i;
	InFifoDev.read_front=0;
	InFifoDev.writer_rear=0;
	InFifoDev.rp=&AdsInBuffer[0];
	InFifoDev.wp=&AdsInBuffer[0];
	
	OutFifoDev.read_front=0;
	OutFifoDev.writer_rear=0;
	OutFifoDev.rp=&EcgOutBuffer[0];
	OutFifoDev.wp=&EcgOutBuffer[0];
	
	for(i=0;i<PACK_NUM;i++){
		InFifoDev.state[i]=Empty;
		OutFifoDev.state[i]=Empty;
	}

}

/**
  * @Brief 检测导联线是否脱落
  * @Call   Cheack_lead_stata
  * @Param
  * @Retval None
  */
void Cheack_lead_stata(u8 *p)
{
    u8 data=p[0];
	u8 i=0;
    if(data&0x04)
    {
        ecg_info.left_lead_wire_state=LEAD_WIRE_OFF;
    } else {
        ecg_info.left_lead_wire_state=LEAD_WIRE_ON;
		i++;
    }

    if(data&0x02)
    {
        ecg_info.right_lead_wrie_state=LEAD_WIRE_OFF;
    } else {
        ecg_info.right_lead_wrie_state=LEAD_WIRE_ON;
		i++;
    }
	if(i==2)ecg_info.ecg_state=ECG_ON;
	else ecg_info.ecg_state=ECG_OFF;
}

/**
  * @Brief 将有符号的24位数转换成32位有符号数据，并更新ECG数据
  * @Call   Update_ECG_Data
  * @Param
  * @Retval None
  */
void Update_ECG_Data(u8 *pdata)
{
    int respirat=0;
    int ecgdata=0;
    respirat=pdata[3]<<16 | pdata[4]<<8 | pdata[5];
    ecgdata=pdata[6]<<16 | pdata[7]<<8 | pdata[8];
    ecg_info.respirat_impedance=S24toS32(respirat);
    ecg_info.ecg_data=S24toS32(ecgdata);
}


/**
  * @Brief 24位数转32位
  * @Call   Update_ECG_Data
  * @Param
  * @Retval None
  */
int S24toS32(int input)
{
    if((input&0x800000)==0x800000)      //如果最高位为1，则是负数
    {
        input |= 0xff000000;        //高位补1
    }
    return input;
}

u8 data_to_send[13];//串口发送缓存

void EcgUsartSendInit(void)
{
    MYDMA_Config(DMA2_Stream7,DMA_Channel_4,(u32)&USART1->DR,(u32)data_to_send,13);//串口1DMA设置
}


void EcgSendByUart(int value)
{
    u8 i=0,sum=0;
    data_to_send[0]=0xAA;   //初始化串口初值
    data_to_send[1]=0xAA;
    data_to_send[2]=0xF1;
    data_to_send[3]=8;
	data_to_send[4]=ecg_info.respirat_impedance>>24;      //25-32位
    data_to_send[5]=ecg_info.respirat_impedance>>16;      //17-24
    data_to_send[6]=ecg_info.respirat_impedance>>8;      //9-16
    data_to_send[7]=ecg_info.respirat_impedance;          //1-8
    data_to_send[8]=value;      //25-32位
    data_to_send[9]=value>>8;      //17-24
    data_to_send[10]=value>>16;       //9-16
    data_to_send[11]=value>>24;          //1-8

    for(i=0; i<12; i++)
        sum += data_to_send[i];
    data_to_send[12] = sum; //校验和

    USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE); //使能串口 1 的 DMA 发送
    MYDMA_Enable(DMA2_Stream7,13); //开始一次 DMA 传输！
}


static void WriteAdsInBuffer(int date)
{
	static u8 cnt=0;
	if(InFifoDev.state[InFifoDev.writer_rear]==Empty){//缓存块可写
		InFifoDev.wp=&AdsInBuffer[InFifoDev.writer_rear*(BLOCK_SIZE)];//将写指针定位写缓存块
		InFifoDev.wp[cnt++]=date;
		if(cnt==BLOCK_SIZE){
			cnt=0;
			InFifoDev.state[InFifoDev.writer_rear]=Full;
			InFifoDev.writer_rear=(InFifoDev.writer_rear+1)%PACK_NUM;//切换写缓存块
			
		}
	}
	
}

//定位读指针
//成功则返回1，不成功则返回0
u8 ReadAdsInBuffer(void)
{
	if(InFifoDev.state[InFifoDev.read_front]==Full){//缓存块可读
		InFifoDev.rp=&AdsInBuffer[InFifoDev.read_front*(BLOCK_SIZE)];//将读指针定位读缓存块
		return 1;
	}
	return 0;
}


//定位读指针
u8 WriterEcgOutBuffer(void)
{
	if(OutFifoDev.state[OutFifoDev.writer_rear]==Empty){//缓存块可写
		OutFifoDev.wp=&EcgOutBuffer[OutFifoDev.writer_rear*(BLOCK_SIZE)];//将读指针定位读缓存块
		return 1;
	}
	return 0;
}

//成功则返回1，不成功则返回0
u8 ReadEcgOutBuffer(int32_t *p)
{
	static u8 cnt=0;
	if(OutFifoDev.state[OutFifoDev.read_front]==Full){//缓存块可读
		OutFifoDev.rp=&EcgOutBuffer[OutFifoDev.read_front*(BLOCK_SIZE)];//将写指针定位读缓存块
		*p=OutFifoDev.rp[cnt++];
		if(cnt==BLOCK_SIZE){
			cnt=0;
			OutFifoDev.state[OutFifoDev.read_front]=Empty;
			OutFifoDev.read_front=(OutFifoDev.read_front+1)%PACK_NUM;//切换写读缓存块
		}
		return 1;
	}
	return 0;
}


void EXTI9_5_IRQHandler(void)
{

    if(EXTI->IMR&EXTI_Line5 && ADS_DRDY==0)//数据接收中断
    {
        ADS1292_Read_Data(ads1292_Cache);//数据存到9字节缓冲区
        Update_ECG_Data(ads1292_Cache);
        Cheack_lead_stata(ads1292_Cache);
		if(state_pcb.SampleStartFlag==true)
			WriteAdsInBuffer(ecg_info.ecg_data);//数据写入缓存区
    } 
	EXTI_ClearITPendingBit(EXTI_Line5);

}
