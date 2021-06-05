#ifndef __L610_H__
#define __L610_H__

#include "sys.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include "spi.h"
#include "w25qxx.h"

#ifdef L610_GLOBALS
#define L610_EXT
#else
#define L610_EXT extern
#endif

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif


#define cRX_BUF_MAX_LEN 1024		  //最大接收缓存字节数
#define cTX_LIST_MAX_LEN	20			//发送命令列表最大命令个数

#define cL610_USART UART5				//L610 通讯串口，，波特率为115200
#define cFLASH_ORDER_ADDRESS 12*1024*1024     //12M地址
#define cCommonCheck_Time    2999
#define cSpecialChenk_Time   5999

L610_EXT struct L610_STRUCT_USART_Fram	  //定义一个全局串口数据帧的处理结构体
{
	u8 bData_RX_BUF[cRX_BUF_MAX_LEN];
	union 
	{
    	__IO u16 wInfAll;
    	struct 
		{
		  	__IO u16 wFramLength       :15;                               // 14:0 
		  	__IO u16 wFramFinishFlag   :1;                                // 15 
		}wInfBit;
	}; 
}L610_Fram_Record_Struct;

L610_EXT struct L610_STRUCT_USART_Send	  //定义一个全局串口发送的处理结构体
{
	u8 bTask_Cmd_List[cTX_LIST_MAX_LEN];			//命令发送任务数组
	u8 bTask_Ack_List[cTX_LIST_MAX_LEN];			//应答任务数组
	u8 bResend_Count,bData_Mode,bNow_Task;						//失败重发次数，，数据发送模式标志
	u16 wRestartCheck_Time;
	u8 bSending_Flag;   		     // 0		正在发送标志
	u8 bSend_Finish;				 		// 1		发送完成标志
	u8 bFinish_state;							// 2		命令完成状态标志，0为发送失败
	u8 bRestart_Check;						// 3		重启检测标志，命令发送则开始重启检测
	u8 bResend_Flag;							// 4		重新发送标志
	u8 bResponse_Flag;						// 5-7	应答类别，0为不需要应答，
																					//							 	1为普通应答
																					//								大于1为特殊应答
}L610_Send_Struct;

L610_EXT struct platform_send	  //平台信息发送和存储结构体
{
	u8 bHeadIMEI;
	u8 bIMEI[15];
	u8 bActice_Code[8];
	u8 bChallengeCode[8];
	u8 bActice_State;
	u8 bCsq;
	u8 bCheckTime[4];
	u8 bBlue[18];
	u8 bOrder_time[4];
	u8 bOrder_ID[9];
	u8 bOrder_ID_Bkp[9];
	u8 bDevice_ID[16];
	u8 bStart_Code;
	u8 bMode;
	u8 bFlag;
	u16 wLength;
	u16 wChecksum;
	u8 bSer_num;
	u8 bLink_ID;
	u8 bID_Code;
	u8 bEnd_Code;
	u32 dwOrder_num;
	u32 dwUnix_Time;
}PLATFORM;

L610_EXT struct platform_rec	  //平台接收信息结构体
{
	u8 bMode;
	u8 bFlag;
	u16 wLength;
	u16 wChecksum;
	u8 bSer_num;
	u8 bLink_ID;
	u8 bBody[100];
}PLATFORM_REC;

L610_EXT struct lottery	  //主程序标志结构体
{
	u8 bD1155_Reply;
	u8 bVoice_Reply;
	u8 bHandPiece_Reply;
	u8 bL610_Reply;
	u8 bLotteryOut_Flag;
	u8 bCollectionLottery_Voice_Flag;
	u8 bHeart_Sign;
	u8 bPowerDown;
	u8 bL610_Online;
	u8 bRestart_flag;
	u8 bCsqCheck_Flag;
	u8 b4GOnlineCheck_Flag;
	u8 bCheck_Switch;
	
}LOTTERY;

L610_EXT u8 bRestart_flag,bSending_Flag;					//重启标志
L610_EXT u16 bL610_Restart_Check_CNT,wSend_Data_Len;
u16 CalcCrc16 ( u8 *pdata, int len);
void SoftReset(void);         //软件重启

void L610_printf(char* fmt,...);							//printf函数
void L610_Usart_Send(u8 *bData,u16 bLen);			//L610发送字节函数


//发送具体命令的函数
void L610_Init(void);															//L610模块通讯初始化
void L610_qSignal(void);
void L610_Restart(void);
//void L610_Http_Set(u8 *bUrl,u8 *bPort);						//L610 http 设置网址和端口号
//void L610_Http_Get(u16 bOffset,u16 bSize);					//L610 http get 请求方式
//void L610_Http_Post(u8 *bCode,u8 bLen);						//L610 http post 发送数据
void L610_qTcp(void);
void L610_Tcp_Set(u8 *bUrl,u8 *bPort);
void L610_Tcp_Send(u8 *bCode,u16 wLen);




//////////////////////////使用时只需将这三个函数放入主循环中扫描
void L610_Scan_Send(void);											//发送任务扫描函数
void L610_Scan_Restart(void);									//L610重启扫描函数
u8 L610_Scan(void);								//L610串口接收扫描函数



///////////使用时需要放入其他文件中扫描
void L610_Restart_Check_Scan(void);					//该函数检测发送命令后无响应的时间，超时则重启，使用串口接收时，将该函数放至 1ms 扫描一次的函数里
void L610_USART_SCAN(void);									//该函数检测串口接收完成，使用串口接收时，将该函数放至 1ms 扫描一次的函数里
void L610_USART_IRQHandler(void);						//该函数为串口接收中断服务函数，使用串口接收时，用这个函数替换掉对应串口的中断服务函数里面的内容



#endif
