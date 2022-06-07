#ifndef __ECG_DATA_PROCESS_H
#define __ECG_DATA_PROCESS_H
#include "sys.h"

/*****ECG数据信息******/
typedef struct
{
    u8 right_lead_wrie_state;
    u8 left_lead_wire_state;
	u8 ecg_state;
    int respirat_impedance;//呼吸阻抗
    int ecg_data;         //心电数据
} ECG_TYPE;


typedef struct
{
    int y_min;
    int y_pose;
	int sacle;//放缩比例
    u8 send_type;
} ECG_Graph_Type;

/***导联线的状态****/
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

#define BLOCK_SIZE           25 /* 调用一次arm_fir_f32处理的采样点个数 */
#define PACK_NUM             10
#define FIFO_SIZE (PACK_NUM * BLOCK_SIZE)//缓存区大小

extern int32_t AdsInBuffer[FIFO_SIZE];
extern int32_t EcgOutBuffer[FIFO_SIZE];


//采用FIFO队列控制图像包的进出
typedef struct FIFO
{
	int32_t* rp; //读指针
	int32_t* wp;//写指针
	u8 state[PACK_NUM];//状态表
	u8 read_front;					 //队列的头
	u8 writer_rear;					//队列的尾
}FIFO_TypeDef;

extern FIFO_TypeDef InFifoDev;
extern FIFO_TypeDef OutFifoDev;
/***FIFO的状态表的状态****/
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

