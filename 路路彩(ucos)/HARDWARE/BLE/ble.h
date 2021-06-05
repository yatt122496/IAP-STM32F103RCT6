#ifndef __BLE_H__
#define __BLE_H__	
#include "sys.h"
#include <stdarg.h>
#include <string.h>
#include "timer.h"
#include "led.h"
#include "usart.h"

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#define cBLE_RX_BUF_MAX_LEN 1024		  //�����ջ����ֽ���

#define cBLE_USART	USART2					//����ģ��ʹ�õĴ��ڣ���������Ϊ9600




extern struct BLE_STRUCT_USART_Fram	  //����һ��ȫ�ִ�������֡�Ĵ���ṹ��
{
	u8 bData_RX_BUF[cBLE_RX_BUF_MAX_LEN];
	union 
	{
    	__IO u16 wInfAll;
    	struct 
		{
		  	__IO u16 wFramLength       :15;                               // 14:0 
		  	__IO u16 wFramFinishFlag   :1;                                // 15 
	  	}wInfBit;
  	}; 
}BLE_Fram_Record_Struct;

u8 *bBLE_Scan(void);										//ɨ���ȡ���ڽ����������

void BLE_Change_Name(u8 *bStr_data);
void BLE_Restart(void);

void BLE_printf(char* fmt,...);
void BLE_Usart_Send(u8 *bData,u16 wLen);

void BLE_Usart_Scan(void);							//ʹ�ô��ڽ���ʱ�����ú������� 1ms ɨ��һ�εĺ�����

void BLE_Usart_IRQHandler(void);				//ʹ�ô��ڽ���ʱ������������滻����Ӧ���ڵ��жϷ��������������

#endif
