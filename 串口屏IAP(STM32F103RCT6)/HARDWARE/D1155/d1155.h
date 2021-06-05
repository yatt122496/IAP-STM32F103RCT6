#ifndef __D1155_H__
#define __D1155_H__	
#include "sys.h"
#include <stdarg.h>
#include <string.h>
#include "timer.h"
#include "usart.h"

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#define cD1155_RX_BUF_MAX_LEN 1024		  //�����ջ����ֽ���

#define cD1155_USART	USART1

enum {
MODE0=0,					//��ʾ����
MODE1,						//�л��ٶ�
MODE2,						//�л�����
};
	
enum{
SPEED0=0,					//��ʾ�ٶȣ�0����
SPEED1,
SPEED2,
SPEED3,
SPEED4,
SPEED5,
SPEED6,
SPEED7,
};
	
enum{
BRIGHT0=0,				//��ʾ���ȣ�0����
BRIGHT1,
BRIGHT2,
BRIGHT3,
};

enum{
WORD1=1,					//��ʾ����
WORD2,
WORD3,
WORD4,
WORD5,
WORD6,
WORD7,
};


extern struct D1155_STRUCT_USART_Fram	  //����һ��ȫ�ִ�������֡�Ĵ���ṹ��
{
	u8 bData_RX_BUF[cD1155_RX_BUF_MAX_LEN];
	union 
	{
    	__IO u16 wInfAll;
    	struct 
		{
		  	__IO u16 wFramLength       :15;                               // 14:0 
		  	__IO u16 wFramFinishFlag   :1;                                // 15 
	  	}wInfBit;
  	}; 
}D1155_Fram_Record_Struct;

void D1155_printf(char* fmt,...);
void D1155_USART_Send(u8 *data,u16 wLen);

void D1155_Send_Player(u8 mode,u8 str);

u8 *bD1155_Scan(void);
	
void D1155_Usart_Scan(void);							//ʹ�ô��ڽ���ʱ�����ú������� 1ms ɨ��һ�εĺ�����

void D1155_Usart_IRQHandler(void);				//ʹ�ô��ڽ���ʱ������������滻����Ӧ���ڵ��жϷ��������������

#endif
