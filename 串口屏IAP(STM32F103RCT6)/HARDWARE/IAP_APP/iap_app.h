#ifndef __IAP_APP_H__
#define __IAP_APP_H__	
#include "sys.h"
#include <stdarg.h>
#include <string.h>
#include "timer.h"
#include "usart.h"
#include "w25qxx.h"

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#define cIAP_APP_RX_BUF_MAX_LEN 4096		  //最大接收缓存字节数

#define cIAP_APP_USART	USART1

#define cUPDATAINFOADDR	1024*1024*6
#define cFIRMWAREADDR		(cUPDATAINFOADDR+4096)

extern struct IAP_APP_STRUCT_USART_Fram	  //定义一个全局串口数据帧的处理结构体
{
	u8 bData_RX_BUF[cIAP_APP_RX_BUF_MAX_LEN];
	union 
	{
    	__IO u16 wInfAll;
    	struct 
		{
		  	__IO u16 wFramLength       :15;                               // 14:0 
		  	__IO u16 wFramFinishFlag   :1;                                // 15 
	  	}wInfBit;
  	}; 
}IAP_APP_Fram_Record_Struct;

void IAP_APP_Reply(void);

void IAP_APP_printf(char* fmt,...);
void IAP_APP_USART_Send(u8 *data,u16 wLen);


u8 *bIAP_APP_Scan(void);
	
void IAP_APP_Usart_Scan(void);							//使用串口接收时，将该函数放至 1ms 扫描一次的函数里

void IAP_APP_Usart_IRQHandler(void);				//使用串口接收时，用这个函数替换掉对应串口的中断服务函数里面的内容

#endif
