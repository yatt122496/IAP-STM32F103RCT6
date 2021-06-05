#ifndef __USART2_H
#define __USART2_H	 
#include "sys.h"
#include <stdarg.h>
#include <string.h>

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#define D1155_USART(fmt, ...)	 USART_printf (USART2, fmt, ##__VA_ARGS__)	//��N58���ڷ�������
#define PC_USART(fmt, ...)	 printf (fmt, ##__VA_ARGS__)								//��ʾN58���ص�����

#define RX_BUF_MAX_LEN 1024		  //�����ջ����ֽ���

extern struct STRUCT_USART_Fram	  //����һ��ȫ�ִ�������֡�Ĵ���ṹ��
{
	u8 bData_RX_BUF[RX_BUF_MAX_LEN];
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

void USART_printf( USART_TypeDef * USARTx, char * Data, ... );		//printf����

void USART2_Init(u32 bound);				//����2��ʼ�� 
void TIM4_Set(u8 sta);
void TIM4_Init(u16 arr,u16 psc);
#endif













