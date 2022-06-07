#include "ecg_data_process.h"
#include "bsp_ads1292.h"
#include "lv_chart_init.h"
#include "lvgl.h"
#include "bsp_dma.h"
#include "usart.h"
ECG_TYPE ecg_info;
ECG_Graph_Type ecg_graph= {0,0,200,GRAPH};

extern System_State_dev state_pcb;

u8 ads1292_Cache[9];    //���ݻ�����
//��ȡ72λ������1100+LOFF_STAT[4:0]+GPIO[1:0]+13��0+2CHx24λ����9�ֽ�
//  1100    LOFF_STAT[4         3           2           1           0   ]   //����������ص���Ϣ��LOFF_STAT�Ĵ�����
//                    RLD     1N2N         1N2P         1N1N    1N1P
//  ��  C0 00 00 FF E1 1A FF E1 52

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
  * @Brief ��⵼�����Ƿ�����
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
  * @Brief ���з��ŵ�24λ��ת����32λ�з������ݣ�������ECG����
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
  * @Brief 24λ��ת32λ
  * @Call   Update_ECG_Data
  * @Param
  * @Retval None
  */
int S24toS32(int input)
{
    if((input&0x800000)==0x800000)      //������λΪ1�����Ǹ���
    {
        input |= 0xff000000;        //��λ��1
    }
    return input;
}

u8 data_to_send[13];//���ڷ��ͻ���

void EcgUsartSendInit(void)
{
    MYDMA_Config(DMA2_Stream7,DMA_Channel_4,(u32)&USART1->DR,(u32)data_to_send,13);//����1DMA����
}


void EcgSendByUart(int value)
{
    u8 i=0,sum=0;
    data_to_send[0]=0xAA;   //��ʼ�����ڳ�ֵ
    data_to_send[1]=0xAA;
    data_to_send[2]=0xF1;
    data_to_send[3]=8;
	data_to_send[4]=ecg_info.respirat_impedance>>24;      //25-32λ
    data_to_send[5]=ecg_info.respirat_impedance>>16;      //17-24
    data_to_send[6]=ecg_info.respirat_impedance>>8;      //9-16
    data_to_send[7]=ecg_info.respirat_impedance;          //1-8
    data_to_send[8]=value;      //25-32λ
    data_to_send[9]=value>>8;      //17-24
    data_to_send[10]=value>>16;       //9-16
    data_to_send[11]=value>>24;          //1-8

    for(i=0; i<12; i++)
        sum += data_to_send[i];
    data_to_send[12] = sum; //У���

    USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE); //ʹ�ܴ��� 1 �� DMA ����
    MYDMA_Enable(DMA2_Stream7,13); //��ʼһ�� DMA ���䣡
}


static void WriteAdsInBuffer(int date)
{
	static u8 cnt=0;
	if(InFifoDev.state[InFifoDev.writer_rear]==Empty){//������д
		InFifoDev.wp=&AdsInBuffer[InFifoDev.writer_rear*(BLOCK_SIZE)];//��дָ�붨λд�����
		InFifoDev.wp[cnt++]=date;
		if(cnt==BLOCK_SIZE){
			cnt=0;
			InFifoDev.state[InFifoDev.writer_rear]=Full;
			InFifoDev.writer_rear=(InFifoDev.writer_rear+1)%PACK_NUM;//�л�д�����
			
		}
	}
	
}

//��λ��ָ��
//�ɹ��򷵻�1�����ɹ��򷵻�0
u8 ReadAdsInBuffer(void)
{
	if(InFifoDev.state[InFifoDev.read_front]==Full){//�����ɶ�
		InFifoDev.rp=&AdsInBuffer[InFifoDev.read_front*(BLOCK_SIZE)];//����ָ�붨λ�������
		return 1;
	}
	return 0;
}


//��λ��ָ��
u8 WriterEcgOutBuffer(void)
{
	if(OutFifoDev.state[OutFifoDev.writer_rear]==Empty){//������д
		OutFifoDev.wp=&EcgOutBuffer[OutFifoDev.writer_rear*(BLOCK_SIZE)];//����ָ�붨λ�������
		return 1;
	}
	return 0;
}

//�ɹ��򷵻�1�����ɹ��򷵻�0
u8 ReadEcgOutBuffer(int32_t *p)
{
	static u8 cnt=0;
	if(OutFifoDev.state[OutFifoDev.read_front]==Full){//�����ɶ�
		OutFifoDev.rp=&EcgOutBuffer[OutFifoDev.read_front*(BLOCK_SIZE)];//��дָ�붨λ�������
		*p=OutFifoDev.rp[cnt++];
		if(cnt==BLOCK_SIZE){
			cnt=0;
			OutFifoDev.state[OutFifoDev.read_front]=Empty;
			OutFifoDev.read_front=(OutFifoDev.read_front+1)%PACK_NUM;//�л�д�������
		}
		return 1;
	}
	return 0;
}


void EXTI9_5_IRQHandler(void)
{

    if(EXTI->IMR&EXTI_Line5 && ADS_DRDY==0)//���ݽ����ж�
    {
        ADS1292_Read_Data(ads1292_Cache);//���ݴ浽9�ֽڻ�����
        Update_ECG_Data(ads1292_Cache);
        Cheack_lead_stata(ads1292_Cache);
		if(state_pcb.SampleStartFlag==true)
			WriteAdsInBuffer(ecg_info.ecg_data);//����д�뻺����
    } 
	EXTI_ClearITPendingBit(EXTI_Line5);

}
