#ifndef __ECG_DATA_PROCESS_H
#define __ECG_DATA_PROCESS_H
#include "sys.h"

/*****ECG������Ϣ******/
typedef struct
{
    u8 right_lead_wrie_state;
    u8 left_lead_wire_state;
	u8 ecg_state;
    int respirat_impedance;//�����迹
    int ecg_data;         //�ĵ�����
} ECG_TYPE;


typedef struct
{
    int y_min;
    int y_pose;
	int sacle;//��������
    u8 send_type;
} ECG_Graph_Type;

/***�����ߵ�״̬****/
typedef enum
{
    LEAD_WIRE_OFF = 0x01,
    LEAD_WIRE_ON  = 0X02
} LEAD_STAT;


typedef enum
{
    ECG_ON  = 0x01,
    ECG_OFF = 0X02
} ECG_STAT;

typedef enum
{
    GRAPH = 0x01,
    USART  = 0X02
} SEND_TYPE;

#define BLOCK_SIZE           25 /* ����һ��arm_fir_f32����Ĳ�������� */
#define PACK_NUM             10
#define FIFO_SIZE (PACK_NUM * BLOCK_SIZE)//��������С

extern int32_t AdsInBuffer[FIFO_SIZE];
extern int32_t EcgOutBuffer[FIFO_SIZE];


//����FIFO���п���ͼ����Ľ���
typedef struct FIFO
{
	int32_t* rp; //��ָ��
	int32_t* wp;//дָ��
	u8 state[PACK_NUM];//״̬��
	u8 read_front;					 //���е�ͷ
	u8 writer_rear;					//���е�β
}FIFO_TypeDef;

extern FIFO_TypeDef InFifoDev;
extern FIFO_TypeDef OutFifoDev;
/***FIFO��״̬���״̬****/
typedef enum
{
    Empty = 0x00,
    Full  = 0X01
}FIFO_STAT;

extern ECG_TYPE ecg_info;
extern ECG_Graph_Type ecg_graph;
void Cheack_lead_stata(u8 *p);
void Update_ECG_Data(u8 *pdata);
int S24toS32(int input);
void EcgUsartSendInit(void);
void EcgSendByUart(int value);
void ECGDataFIFOInit(void);
u8 ReadAdsInBuffer(void);
u8 WriterEcgOutBuffer(void);
u8 ReadEcgOutBuffer(int32_t *p);

#endif

