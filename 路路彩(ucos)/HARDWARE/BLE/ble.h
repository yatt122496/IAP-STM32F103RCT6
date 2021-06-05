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

#define cBLE_RX_BUF_MAX_LEN 1024		  //最大接收缓存字节数

#define cBLE_USART	USART2					//蓝牙模块使用的串口，，波特率为9600




extern struct BLE_STRUCT_USART_Fram	  //定义一个全局串口数据帧的处理结构体
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

u8 *bBLE_Scan(void);										//扫描读取串口接收完的数据

void BLE_Change_Name(u8 *bStr_data);
void BLE_Restart(void);

void BLE_printf(char* fmt,...);
void BLE_Usart_Send(u8 *bData,u16 wLen);

void BLE_Usart_Scan(void);							//使用串口接收时，将该函数放至 1ms 扫描一次的函数里

void BLE_Usart_IRQHandler(void);				//使用串口接收时，用这个函数替换掉对应串口的中断服务函数里面的内容

#endif
