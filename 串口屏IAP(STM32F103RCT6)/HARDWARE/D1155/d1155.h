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

#define cD1155_RX_BUF_MAX_LEN 1024		  //最大接收缓存字节数

#define cD1155_USART	USART1

enum {
MODE0=0,					//显示文字
MODE1,						//切换速度
MODE2,						//切换亮度
};
	
enum{
SPEED0=0,					//显示速度，0最慢
SPEED1,
SPEED2,
SPEED3,
SPEED4,
SPEED5,
SPEED6,
SPEED7,
};
	
enum{
BRIGHT0=0,				//显示亮度，0最亮
BRIGHT1,
BRIGHT2,
BRIGHT3,
};

enum{
WORD1=1,					//显示文字
WORD2,
WORD3,
WORD4,
WORD5,
WORD6,
WORD7,
};


extern struct D1155_STRUCT_USART_Fram	  //定义一个全局串口数据帧的处理结构体
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
	
void D1155_Usart_Scan(void);							//使用串口接收时，将该函数放至 1ms 扫描一次的函数里

void D1155_Usart_IRQHandler(void);				//使用串口接收时，用这个函数替换掉对应串口的中断服务函数里面的内容

#endif
