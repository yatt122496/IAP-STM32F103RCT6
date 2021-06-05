#include "voice.h"


//////////////////////////////////////////////////////////////////////////////////	 
//语音模块初始化
//作者：袁明正
//联系邮箱：mingzheng.yuan@foxmail.com
//修改日期:2020/8/27
//版本：V1.0
//程序,部分参考正点原子例程
//如有问题，欢迎联系我以改进
////////////////////////////////////////////////////////////////////////////////// 	 


#define VOICE_GLOBALS

u8 bSend_buf[16];
struct voice VOICE={
	.bVoice_PlayState=0,
	.bVoice_NowVolume_Level=5
};



//指定序号音乐播放
void VOICE_Play1(u16 wNum)			//总共 24 个音乐文件
{
	VOICE_SendCmd(0xA5,wNum);
}

void VOICE_Play2(u16 wNum)			//总共 24 个音乐文件
{
	VOICE_SendCmd(0xA6,wNum);
}

//上一曲
void VOICE_Play_up(void)
{
	VOICE_SendCmd(0x02,0);
}

//下一曲
void VOICE_Play_down(void)
{
	VOICE_SendCmd(0x01,0);
}

//播放
void VOICE_Run(void)			//不可用
{
	VOICE_SendCmd(0x0D,0);
}

//暂停
void VOICE_Pause(void)		//不可用
{
	VOICE_SendCmd(0x0E,0);
}

//停止
void VOICE_Stop(void)
{
	VOICE_SendCmd(0x16,0);
}

//音量+
void VOICE_Volume_up(void)
{
	if(VOICE.bVoice_NowVolume_Level<5)
	{
		VOICE.bVoice_NowVolume_Level++;
		VOICE_Volume_Set(VOICE.bVoice_NowVolume_Level*6);
	}
}

//音量-
void VOICE_Volume_down(void)
{
	if(VOICE.bVoice_NowVolume_Level>0)
	{
		VOICE.bVoice_NowVolume_Level--;
		VOICE_Volume_Set(VOICE.bVoice_NowVolume_Level*6);
	}
}

//音量设置 0-30
void VOICE_Volume_Set(u16 wNum)
{
	VOICE_SendCmd(0x06,wNum);
}

//查询当前音量
void VOICE_Volume_current(void)				//上电初始音量为 30
{
	VOICE_SendCmd(0x43,0);
}

//设备重启
void VOICE_Restart(void)
{
	VOICE_SendCmd(0x0C,0);
}

//查询开始状态
void VOICE_State_original(void)
{
	VOICE_SendCmd(0x3F,0);
}

//查询当前状态
void VOICE_State_current(void)			//查询当前播放状态，正在播放，停止两个状态
{
	VOICE_SendCmd(0x42,0);
}

//查询文件数量
void VOICE_File_num(void)
{
	VOICE_SendCmd(0x49,0);
}

//查询已使用空间
void VOICE_Used_room(void)
{
	VOICE_SendCmd(0x50,0);
}



/********************************************************************************************
 - 功能描述： 串口向外发送命令[包括控制和查询]
 - 隶属模块： 外部
 - 参数说明： CMD:表示控制指令，请查阅指令表，还包括查询的相关指令
              feedback:是否需要应答[0:不需要应答，1:需要应答]
              data:传送的参数
 - 返回说明：
 - 注：       
********************************************************************************************/
void VOICE_SendCmd(u8 bCMD , u16 wDat)
{
	bSend_buf[0] = 0x7E;    //保留字节 
	bSend_buf[1] = bCMD;     //控制指令
	bSend_buf[2] = 0; 
	bSend_buf[3] = 0x02;    //长度
	bSend_buf[4] = (u8)(wDat >> 8);//datah
	bSend_buf[5] = (u8)(wDat);     //datal
	bSend_buf[6] = 0xEF;
      
	VOICE_Usart_Send(bSend_buf,7);	//发送此帧数据
}

//////////////////////////////////// 串口设置 ///////////////////////////////


struct VOICE_STRUCT_USART_Fram VOICE_Fram_Record_Struct = { 0 };		//N58串口接收结构体

//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
static u8 bVOICE_USART_RX_CHECK=0;
static u32 dwVOICE_USART_RX_CNT=0;
void VOICE_Usart_IRQHandler(void)
{
	u8 res;

	if(USART_GetITStatus(cVOICE_USART, USART_IT_RXNE) != RESET)//接收到数据
	{
		res =USART_ReceiveData(cVOICE_USART);
		if(VOICE_Fram_Record_Struct.wInfBit.wFramLength < cVOICE_RX_BUF_MAX_LEN-1)		//还可以接收数据
		{
			dwVOICE_USART_RX_CNT=0;
			if(VOICE_Fram_Record_Struct.wInfAll==0)bVOICE_USART_RX_CHECK=1;
			VOICE_Fram_Record_Struct.bData_RX_BUF[VOICE_Fram_Record_Struct.wInfBit.wFramLength++]=res;		//记录接收到的值
		}else
		{
			VOICE_Fram_Record_Struct.wInfBit.wFramFinishFlag=1;					//强制标记接收完成
		}
	}

}

void VOICE_Usart_Scan(void)
{
	if(bVOICE_USART_RX_CHECK)
	{
		if(!dwVOICE_USART_RX_CNT)
			dwVOICE_USART_RX_CNT=*pSys_Time;
		if((*pSys_Time-dwVOICE_USART_RX_CNT)==20)
		{
			VOICE_Fram_Record_Struct.wInfBit.wFramFinishFlag = 1;	//标记接收完成
			bVOICE_USART_RX_CHECK=0;
		}
	}
	else
		dwVOICE_USART_RX_CNT=0;
}

u8 bVOICE_Scan(void)
{
	u8 bLen,bRes=0;
	u16 i,wEndPoint;
	if(VOICE_Fram_Record_Struct.wInfBit.wFramFinishFlag)
	{
		VOICE_Fram_Record_Struct.bData_RX_BUF[VOICE_Fram_Record_Struct.wInfBit.wFramLength]=0;
		VTXprintf("Voice Rec:\r\n");
		for(i=0;i<VOICE_Fram_Record_Struct.wInfBit.wFramLength;i++)
				VTXprintf("%02X ",VOICE_Fram_Record_Struct.bData_RX_BUF[i]);
		VTXprintf("\r\n");
		
		if(VOICE_Fram_Record_Struct.bData_RX_BUF[VOICE_Fram_Record_Struct.wInfBit.wFramLength-1]==0xef)
		{
			i=VOICE_Fram_Record_Struct.wInfBit.wFramLength-6;
			bLen=0;
			while(VOICE_Fram_Record_Struct.bData_RX_BUF[i]!=0xef)
			{
				if(i==0)
				{
					wEndPoint=0;
					break;
				}
				bLen++;
				if(bLen>8)
				{
					VOICE_Fram_Record_Struct.wInfAll=0;
					memset((char*)VOICE_Fram_Record_Struct.bData_RX_BUF,0x00,VOICE_Fram_Record_Struct.wInfBit.wFramLength);//串口接收缓冲区清0
					return 0;
				}
				i--;
				wEndPoint=i+1;
			}
			
			if(VOICE_Fram_Record_Struct.bData_RX_BUF[wEndPoint]==0x7e)
			{
				wEndPoint++;
			}
//			VTXprintf("wEndPoint=%d,wEndPoint=%02x\r\n",wEndPoint,VOICE_Fram_Record_Struct.bData_RX_BUF[wEndPoint]);
			bRes=VOICE_Fram_Record_Struct.bData_RX_BUF[wEndPoint];
			switch(bRes)
			{
				case 0x01:
				{
					VTXprintf("Play Next Voice.\r\n");
					VOICE.bVoice_PlayState=1;
					break;
				}
				case 0x02:
				{
					VTXprintf("Play Before Voice.\r\n");
					VOICE.bVoice_PlayState=1;
					break;
				}
				case 0x06:
				{
					u8 bSetVolume=VOICE_Fram_Record_Struct.bData_RX_BUF[wEndPoint+3]*256+VOICE_Fram_Record_Struct.bData_RX_BUF[wEndPoint+4];
					VTXprintf("Set Volume is %d.\r\n",bSetVolume);
					VOICE.bVoice_NowVolume_Level=bSetVolume/6;
					break;
				}
				case 0x0C:
				{
					VTXprintf("Restart This Device.\r\n");
					break;
				}
				case 0x16:
				{
					VTXprintf("Ending Play Now Voice.\r\n");
					VOICE.bVoice_PlayState=0;
					break;
				}
				case 0x39:
				{
					u8 bNowVoice=VOICE_Fram_Record_Struct.bData_RX_BUF[wEndPoint+3]*256+VOICE_Fram_Record_Struct.bData_RX_BUF[wEndPoint+4];
					VTXprintf("Ace2 Voice %d Play Finish.\r\n",bNowVoice);
					VOICE.bVoice_PlayState=0;
					break;
				}
				case 0x3E:
				{
					u8 bNowVoice=VOICE_Fram_Record_Struct.bData_RX_BUF[wEndPoint+3]*256+VOICE_Fram_Record_Struct.bData_RX_BUF[wEndPoint+4];
					VTXprintf("Ace1 Voice %d Play Finish.\r\n",bNowVoice);
					VOICE.bVoice_PlayState=0;
					break;
				}
				case 0x3F:
				{
					u8 bVoiceNum_Area1=VOICE_Fram_Record_Struct.bData_RX_BUF[wEndPoint+3];
					u8 bVoiceNum_Area2=VOICE_Fram_Record_Struct.bData_RX_BUF[wEndPoint+4];
					VTXprintf("Ace1 Voice Num is %d,\r\nAce2 Voice Num is %d.\r\n",bVoiceNum_Area1,bVoiceNum_Area2);
					break;
				}
				case 0x42:
				{
					u8 bNowState=VOICE_Fram_Record_Struct.bData_RX_BUF[wEndPoint+4];
					if(bNowState)
						VTXprintf("Now State is Playing Voice.\r\n");
					else
						VTXprintf("Now Play State is Not Playing Voice.\r\n");
					VOICE.bVoice_PlayState=bNowState;
					break;
				}
				case 0x43:
				{
					u8 bNowVolume=VOICE_Fram_Record_Struct.bData_RX_BUF[wEndPoint+3]*256+VOICE_Fram_Record_Struct.bData_RX_BUF[wEndPoint+4];
					VTXprintf("Now Volume is %d.\r\n",bNowVolume);
					VOICE.bVoice_NowVolume_Level=bNowVolume/6;
					break;
				}
				case 0x49:
				{
					u8 bVoiceNum_Area1=VOICE_Fram_Record_Struct.bData_RX_BUF[wEndPoint+3];
					u8 bVoiceNum_Area2=VOICE_Fram_Record_Struct.bData_RX_BUF[wEndPoint+4];
					VTXprintf("Ace1 Voice Num is %d,\r\nAce2 Voice Num is %d.\r\n",bVoiceNum_Area1,bVoiceNum_Area2);
					break;
				}
				case 0x50:
				{
					u8 bFlashRoom=VOICE_Fram_Record_Struct.bData_RX_BUF[wEndPoint+3]*256+VOICE_Fram_Record_Struct.bData_RX_BUF[wEndPoint+4];
					VTXprintf("Flash Room is %d MBytes.\r\n",bFlashRoom);
					break;
				}
				case 0xA5:
				{
					u8 bPlayVoice=VOICE_Fram_Record_Struct.bData_RX_BUF[wEndPoint+3]*256+VOICE_Fram_Record_Struct.bData_RX_BUF[wEndPoint+4];
					VTXprintf("Playing Area1 Voice %d.\r\n",bPlayVoice);
					VOICE.bVoice_PlayState=1;
					break;
				}
				case 0xA6:
				{
					u8 bPlayVoice=VOICE_Fram_Record_Struct.bData_RX_BUF[wEndPoint+3]*256+VOICE_Fram_Record_Struct.bData_RX_BUF[wEndPoint+4];
					VTXprintf("Playing Area2 Voice %d.\r\n",bPlayVoice);
					break;
				}
				default:
					break;
			}
		}
		//此处添加处理接收完的串口数据的函数
		VOICE_Fram_Record_Struct.wInfAll=0;
	}
	return bRes;
}

/////////////////////////////// USART2 串口发送 //////////////////////////////////////////////



//串口,VTXprintf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
u8 bVOICE_Data_TX_BUF[cVOICE_RX_BUF_MAX_LEN]={0};
void VOICE_printf(char* fmt,...)  
{
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char *)bVOICE_Data_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)bVOICE_Data_TX_BUF);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
	  while(USART_GetFlagStatus(cVOICE_USART,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
	  USART_SendData(cVOICE_USART,bVOICE_Data_TX_BUF[j]); 
	} 
}

void VOICE_Usart_Send(u8 *bData,u16 wLen)
{
	u16 i;
	VTXprintf("Voice Send:");
	for(i=0;i<wLen;i++)
	{
		USART_SendData(cVOICE_USART,bData[i]);
		while(USART_GetFlagStatus(cVOICE_USART,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕 
		VTXprintf("%02x",bData[i]);
	}
	VTXprintf("\r\n");
}
