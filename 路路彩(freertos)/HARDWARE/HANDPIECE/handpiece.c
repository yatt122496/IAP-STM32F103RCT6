#include "handpiece.h"

#include <string.h>
#include <stdio.h>
#include "timer.h"
#include "delay.h"
#include "voice.h"

//////////////////////////////////////////////////////////////////////////////////	 
//机头程序
//作者：袁明正
//联系邮箱：mingzheng.yuan@foxmail.com
//修改日期:2020/8/27
//版本：V1.0
//程序,部分参考正点原子例程
//如有问题，欢迎联系我以改进
////////////////////////////////////////////////////////////////////////////////// 	 

#define HandPiece_GLOBALS

struct HandPiece HANDPIECE={0};
struct HandPiece_STRUCT_USART_Fram HandPiece_Fram_Record_Struct = { 0 };		//串口接收结构体


void HandPiece_Send_Cmd(HANDPIECE_MODE mode,u8 *pDat)
{
	u8 bCmd[20]={0},bSum=0;
	u16 i;
	
	bCmd[0]=0xAA;
	bCmd[1]=0x55;
	bCmd[3]=mode;
	if(mode==HANDPIECE_LIGHTING)
	{
		bCmd[2]=3;
		bCmd[4]=*pDat;
		bCmd[5]=*(pDat+1);
	}
	else if(mode==HANDPIECE_LOTTERY_OUT)
	{
		bCmd[2]=0x0b;
		for(i=0;i<9;i++)
			bCmd[i+4]=*(pDat+i);
		bCmd[i+4]=*(pDat+i);
	}
	else if(mode==HANDPIECE_GET_VERSION)
	{
		bCmd[2]=0x02;
		bCmd[4]=0;
	}
	else if(mode==HANDPIECE_READLY)
	{
		bCmd[2]=0x02;
		bCmd[4]=0;
	}
	
	for(i=0;i<bCmd[2];i++)
		bSum+=bCmd[i+3];
	bCmd[i+3]=bSum&0xFF;
	
	HandPiece_Usart_Send(bCmd,i+4);
	
}

void HandPiece_Rec_Cmd(u8 bCode)
{
	u8 bCmd[7]={0},bSum=0;
	u16 i;
	
	bCmd[0]=0xAA;
	bCmd[1]=0x55;
	bCmd[2]=0x03;
	bCmd[3]=0x84;
	bCmd[4]=bCode;
	bCmd[5]=0;
	for(i=0;i<bCmd[2];i++)
		bSum+=bCmd[i+3];
	bCmd[6]=bSum&0xFF;
	
	HandPiece_Usart_Send(bCmd,7);
	
}
//////////////////////////////////// 串口设置 ///////////////////////////////


//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
static u8 bHandPiece_Usart_Rx_Check=0;
static u32 dwHandPiece_Usart_Rx_Cnt=0;
void HandPiece_Usart_IRQHandler(void)
{
	u8 res;

	if(USART_GetITStatus(cHandPiece_USART, USART_IT_RXNE) != RESET)//接收到数据
	{
		res =USART_ReceiveData(cHandPiece_USART);
		if(HandPiece_Fram_Record_Struct.wInfBit.wFramLength < cHandPiece_RX_BUF_MAX_LEN-1)		//还可以接收数据
		{
			dwHandPiece_Usart_Rx_Cnt=0;
			if(HandPiece_Fram_Record_Struct.wInfAll==0)bHandPiece_Usart_Rx_Check=1;
			HandPiece_Fram_Record_Struct.bData_RX_BUF[HandPiece_Fram_Record_Struct.wInfBit.wFramLength++]=res;		//记录接收到的值
		}else
		{
			HandPiece_Fram_Record_Struct.wInfBit.wFramFinishFlag=1;					//强制标记接收完成
		}
	}

}

void HandPiece_Usart_Scan(void)
{
	if(bHandPiece_Usart_Rx_Check)
	{
		if(!dwHandPiece_Usart_Rx_Cnt)
			dwHandPiece_Usart_Rx_Cnt=*pSys_Time;
		if((*pSys_Time-dwHandPiece_Usart_Rx_Cnt)==20)
		{
			HandPiece_Fram_Record_Struct.wInfBit.wFramFinishFlag = 1;	//标记接收完成
			bHandPiece_Usart_Rx_Check=0;
		}
	}
	else
		dwHandPiece_Usart_Rx_Cnt=0;
}

u8 bHandPiece_Scan(void)
{
	u8 bRes=0,bSum=0;
	u16 i;
	if(HandPiece_Fram_Record_Struct.wInfBit.wFramFinishFlag)
	{
		HandPiece_Fram_Record_Struct.bData_RX_BUF[HandPiece_Fram_Record_Struct.wInfBit.wFramLength]=0;
		VTXprintf("HandPiece REV:\r\n");
		for(i=0;i<HandPiece_Fram_Record_Struct.wInfBit.wFramLength;i++)
			VTXprintf("%02X",HandPiece_Fram_Record_Struct.bData_RX_BUF[i]);
		VTXprintf("\r\n");
		
		if(HandPiece_Fram_Record_Struct.bData_RX_BUF[0]==0xaa&&HandPiece_Fram_Record_Struct.bData_RX_BUF[1]==0x55)
		{
			for(i=0;i<HandPiece_Fram_Record_Struct.bData_RX_BUF[2];i++)
				bSum+=HandPiece_Fram_Record_Struct.bData_RX_BUF[i+3];
			if((bSum&0xFF)==HandPiece_Fram_Record_Struct.bData_RX_BUF[i+3])
			{
				if(HandPiece_Fram_Record_Struct.bData_RX_BUF[3]!=0x81&&HandPiece_Fram_Record_Struct.bData_RX_BUF[3]!=0xC4)
				{
					HandPiece_Rec_Cmd(HandPiece_Fram_Record_Struct.bData_RX_BUF[3]);
					delay_ms(20);
				}
				bRes=HandPiece_Fram_Record_Struct.bData_RX_BUF[3];
				switch(bRes)
				{
					case 0x81:
					{
						HANDPIECE.bHandReply_Code=HandPiece_Fram_Record_Struct.bData_RX_BUF[4];
						VTXprintf("REC HandPiece Respond %02X\r\n",HANDPIECE.bHandReply_Code);
						break;
					}
					case 0xC0:
					{
						VTXprintf("HandPiece Error %02X\r\n",HandPiece_Fram_Record_Struct.bData_RX_BUF[4]);
						break;
					}
					case 0xC1:
					{
						HANDPIECE.bLottery_State=HandPiece_Fram_Record_Struct.bData_RX_BUF[4]&0x08;
						VTXprintf("Lottery State %02X\r\n",HANDPIECE.bLottery_State);
						break;
					}
					case 0xC2:
					{
						VTXprintf("\r\nLottery Order: ");
						for(i=0;i<10;i++)
						{
							HANDPIECE.bLottery_Order[i]=HandPiece_Fram_Record_Struct.bData_RX_BUF[4+i];
							VTXprintf("%02X ",HANDPIECE.bLottery_Order[i]);
						}
						VTXprintf("\r\n");
						break;
					}
					case 0xC3:
					{
						if(HandPiece_Fram_Record_Struct.bData_RX_BUF[4]==0x02)
						{
							VOICE_Volume_up();
							VTXprintf("VolumeLevel + 1\r\n");
						}
						else
						{
							VOICE_Volume_down();
							VTXprintf("VolumeLevel - 1\r\n");
						}
						break;
					}
					case 0xC4:
					{
						HANDPIECE.bHandPiece_State=1;
						VTXprintf("HandPiece Ready OK\r\n");
						break;
					}
					default:
						break;
				}
			}
		}
		//此处添加处理接收完的串口数据的函数
		HandPiece_Fram_Record_Struct.wInfAll=0;
	}
	return bRes;
}

/////////////////////////////// USART2 串口发送 //////////////////////////////////////////////

#if HandPiece_Printf
//串口,VTXprintf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
u8 bHandPiece_Data_TX_BUF[cHandPiece_TX_BUF_MAX_LEN];
void HandPiece_printf(char* fmt,...)
{
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char *)bHandPiece_Data_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)bHandPiece_Data_TX_BUF);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
		USART_SendData(cHandPiece_USART,bHandPiece_Data_TX_BUF[j]);
	  while(USART_GetFlagStatus(cHandPiece_USART,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
	}
}
#endif

void HandPiece_Usart_Send(u8 *bData,u16 wLen)
{
	u16 i;
	VTXprintf("HandPiece Send:");
	for(i=0;i<wLen;i++)
	{
		USART_SendData(cHandPiece_USART,bData[i]);
		while(USART_GetFlagStatus(cHandPiece_USART,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕
		VTXprintf("%02x ",bData[i]);
	}
	VTXprintf("\r\n");
}
