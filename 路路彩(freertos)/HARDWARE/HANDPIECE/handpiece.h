#ifndef __HANDPIECE_H__
#define __HANDPIECE_H__	

#include "sys.h"
#include <stdarg.h>

#ifdef HandPiece_GLOBALS
#define HandPiece_EXT
#else
#define HandPiece_EXT extern
#endif

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#define cHandPiece_RX_BUF_MAX_LEN 1024		  //�����ջ����ֽ���
#define cHandPiece_USART	USART3
#define HandPiece_Printf 0

typedef enum{
	HANDPIECE_READLY             =0xC4,
	HANDPIECE_LIGHTING           =0x90,
	HANDPIECE_LOTTERY_OUT        =0x91,
	HANDPIECE_GET_VERSION        =0x92,
	HANDPIECE_UPDATE_SOFT        =0x93,
	
}HANDPIECE_MODE;

HandPiece_EXT struct HandPiece_STRUCT_USART_Fram	  //����һ��ȫ�ִ�������֡�Ĵ���ṹ��
{
	u8 bData_RX_BUF[cHandPiece_RX_BUF_MAX_LEN];
	union 
	{
    	__IO u16 wInfAll;
    	struct 
		{
		  	__IO u16 wFramLength       :15;                               // 14:0 
		  	__IO u16 wFramFinishFlag   :1;                                // 15 
	  	}wInfBit;
  	}; 
}HandPiece_Fram_Record_Struct;

HandPiece_EXT struct HandPiece	  //����һ��ȫ�ִ�������֡�Ĵ���ṹ��
{
	u8 bHandPiece_State;
	u8 bHandReply_Code;
	u8 bLottery_State;
	u8 bLottery_Order[10];
}HANDPIECE;

#if HandPiece_Printf==1

void HandPiece_printf(char* fmt,...);                            //���� printf ����
#define cHandPiece_TX_BUF_MAX_LEN  1024

#endif

void HandPiece_Usart_Send(u8 *bData,u16 wLen);                   //���ڷ����ֽں���

void HandPiece_Rec_Cmd(u8 bCode);
void HandPiece_Send_Cmd(HANDPIECE_MODE mode,u8 *pDat);



u8 bHandPiece_Scan(void);			//�������ݴ����������յ��Ĵ������ݴ��ڴ��ڽ��սṹ���С�


void HandPiece_Usart_Scan(void);							//ʹ�ô��ڽ���ʱ�����ú������� 1ms ɨ��һ�εĺ�����
void HandPiece_Usart_IRQHandler(void);				//ʹ�ô��ڽ���ʱ������������滻����Ӧ���ڵ��жϷ��������������

#endif
