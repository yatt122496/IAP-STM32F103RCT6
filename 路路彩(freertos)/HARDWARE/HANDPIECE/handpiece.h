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

#define cHandPiece_RX_BUF_MAX_LEN 1024		  //最大接收缓存字节数
#define cHandPiece_USART	USART3
#define HandPiece_Printf 0

typedef enum{
	HANDPIECE_READLY             =0xC4,
	HANDPIECE_LIGHTING           =0x90,
	HANDPIECE_LOTTERY_OUT        =0x91,
	HANDPIECE_GET_VERSION        =0x92,
	HANDPIECE_UPDATE_SOFT        =0x93,
	
}HANDPIECE_MODE;

HandPiece_EXT struct HandPiece_STRUCT_USART_Fram	  //定义一个全局串口数据帧的处理结构体
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

HandPiece_EXT struct HandPiece	  //定义一个全局串口数据帧的处理结构体
{
	u8 bHandPiece_State;
	u8 bHandReply_Code;
	u8 bLottery_State;
	u8 bLottery_Order[10];
}HANDPIECE;

#if HandPiece_Printf==1

void HandPiece_printf(char* fmt,...);                            //串口 printf 函数
#define cHandPiece_TX_BUF_MAX_LEN  1024

#endif

void HandPiece_Usart_Send(u8 *bData,u16 wLen);                   //串口发送字节函数

void HandPiece_Rec_Cmd(u8 bCode);
void HandPiece_Send_Cmd(HANDPIECE_MODE mode,u8 *pDat);



u8 bHandPiece_Scan(void);			//串口数据处理函数，接收到的串口数据存在串口接收结构体中。


void HandPiece_Usart_Scan(void);							//使用串口接收时，将该函数放至 1ms 扫描一次的函数里
void HandPiece_Usart_IRQHandler(void);				//使用串口接收时，用这个函数替换掉对应串口的中断服务函数里面的内容

#endif
