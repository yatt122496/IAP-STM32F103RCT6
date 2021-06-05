#ifndef __VOICE_H__
#define __VOICE_H__	
#include "sys.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "usart.h"

#ifdef VOICE_GLOBALS
#define VOICE_EXT
#else
#define VOICE_EXT extern
#endif

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#define cVOICE_RX_BUF_MAX_LEN 256		  //最大接收缓存字节数

#define cVOICE_USART	UART4					//定义语音模块使用的串口,,,,波特率为 115200


VOICE_EXT struct VOICE_STRUCT_USART_Fram	  //定义一个全局串口数据帧的处理结构体
{
	u8 bData_RX_BUF[cVOICE_RX_BUF_MAX_LEN];
	union 
	{
    	__IO u16 wInfAll;
    	struct 
		{
		  	__IO u16 wFramLength       :15;                               // 14:0 
		  	__IO u16 wFramFinishFlag   :1;                                // 15 
	  	}wInfBit;
  	}; 
}VOICE_Fram_Record_Struct;

VOICE_EXT struct voice	  //定义一个全局串口数据帧的处理结构体
{
	u8 bVoice_PlayState;
	u8 bVoice_NowVolume_Level;
}VOICE;

/***********************************************************************************
*																						 对应功能											参数( wDat )
*bCMD						 ：控制命令， bCMD 为  0x01		下一曲
*																			0x02		上一曲
*																			0x03		指定曲目(NUM)							曲目序号 如第一首 ，wDat 为0x0001
*																			0x04		音量+
*																			0x05		音量-
*																			0x06		指定音量									音量范围为 0-30 ，设置音量时 wDat 为音量级数
*																			0x0C		芯片复位
*																			0x0D		播放
*																			0x0E		暂停
*																			0x16		停止
*																			0xA5		指定第一分区播放					wDat 为曲目序号，范围为 1-255
*																			0xA6		指定第二分区播放					wDat 为曲目序号，(第二分区是串口下载进去的音乐，每次下载会删除所有数据)
*																			0x3F		查询初始化状态
*																			0x40		返回错误，请求重发
*																			0x42		查询当前状态
*																			0x43		查询当前音量
*																			0x49		查询 FLASH 的总文件数			返回参数	高字节[第一分区] 低字节[第二分区]
*																			0x50		查询使用的 flash 的容量
*************************************************************************************/


//////////////该函数包含语音模块发送数据的协议，，使用时不用理会
void VOICE_SendCmd(u8 bCMD , u16 wDat);


////////////////////串口打印函数，，，第一个是字符串打印，，第二个是按字节发送数据
void VOICE_printf(char* fmt,...) ;
void VOICE_Usart_Send(u8 *bData,u16 wLen);


///////////使用串口接收时，这两个函数要放在对应位置
void VOICE_Usart_Scan(void);							//使用串口接收时，将该函数放至 1ms 扫描一次的函数里
void VOICE_Usart_IRQHandler(void);				//使用串口接收时，用这个函数替换掉对应串口的中断服务函数里面的内容




u8 bVOICE_Scan(void);										//串口接收扫描函数，返回接收到的数据的起始地址，未接收到数据则返回0


////////////////////////以下函数可供调用
void VOICE_Play1(u16 wNum);
void VOICE_Play2(u16 wNum);
//void VOICE_Run(void);
//void VOICE_Pause(void);
void VOICE_Stop(void);
void VOICE_Play_up(void);
void VOICE_Play_down(void);
void VOICE_Volume_up(void);
void VOICE_Volume_down(void);
void VOICE_Volume_Set(u16 wNum);
void VOICE_Volume_current(void);
void VOICE_Restart(void);
void VOICE_State_original(void);
void VOICE_State_current(void);
void VOICE_File_num(void);
void VOICE_Used_room(void);

#endif
