#ifndef __D1155_H__
#define __D1155_H__	

#include "sys.h"
#include <stdarg.h>

#ifdef D1155_GLOBALS
#define D1155_EXT
#else
#define D1155_EXT extern
#endif

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#define cD1155_RX_BUF_MAX_LEN 1024		  //最大接收缓存字节数
#define cD1155_USART	USART1
#define D1155_Printf 0

typedef enum{
	MODE_WORD					=0,						//显示文字
	MODE_SPEED				=1,						//切换速度
	MODE_BRIGHT				=2,						//切换亮度
}MODE;
	
typedef enum{
	SPEED0             =0,					//显示速度，0最慢
	SPEED1             =1,
	SPEED2             =2,
	SPEED3             =3,
	SPEED4             =4,
	SPEED5             =5,
	SPEED6             =6,
	SPEED7             =7,
}SPEED;
	
typedef enum{
	BRIGHT0           =0,				//显示亮度，0最亮
	BRIGHT1           =1,
	BRIGHT2           =2,
	BRIGHT3           =3,
}BRIGHT;

typedef enum{
	WORD1             =1,					//显示文字1
	WORD2             =2,
	WORD3             =3,
	WORD4             =4,
	WORD5             =5,
	WORD6             =6,
	WORD7             =7,
}WORD;


D1155_EXT struct D1155_STRUCT_USART_Fram	  //定义一个全局串口数据帧的处理结构体
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


#if D1155_Printf

void D1155_printf(char* fmt,...);                            //串口 printf 函数
#define cD1155_TX_BUF_MAX_LEN  1024

#endif

void D1155_Usart_Send(u8 *bData,u16 wLen);                   //串口发送字节函数


//串口屏显示函数
//bMode	：点阵屏模式		bMode = 0 为显示文字；	bStr = 1 时， 文字为：中国体育彩票指定出行空间销售渠道     路路彩     车载彩票销售机
//bStr	：为传递的数据														 2 	  			 ：您好：您面前的是中国体育彩票自主销售机，现在您可以选购中国体育彩票，购买完成后立即开奖
//																								 3			 		 ：很抱歉，库存不足，剩余票款会在20分钟内退还到您的支付账户，如有问题请在小程序页面联系售后服务
//																								 4					 ：刮开中奖号码和你的号码保护层，车上备有刮片，可使用刮片刮开。中奖号码与你的号码一致既得下方中奖金额
//																								 5					 ：如中奖，请刮开左下角的保护层，使用购买时的小程序扫描保护层下的二维码，奖金即时到账，如有问题请在小程序页面联系售后服务或咨询司机师傅
//																								 6					 ：正在出票中
//																								 7					 ：请及时取票
//										bMode = 1 为切换显示速度	；bStr 取值为 0-7 ，0 最慢
//														2 为切换显示亮度 ；bStr 取值为 0-3 ，0 最亮
//
void D1155_Send_player(MODE mode,u8 bStr);


u8 bD1155_Scan(void);			//串口数据处理函数，接收到的串口数据存在串口接收结构体中。


void D1155_Usart_Scan(void);							//使用串口接收时，将该函数放至 1ms 扫描一次的函数里
void D1155_Usart_IRQHandler(void);				//使用串口接收时，用这个函数替换掉对应串口的中断服务函数里面的内容

#endif
