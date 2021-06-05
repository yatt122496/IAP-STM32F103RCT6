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


#define cRX_BUF_MAX_LEN 1024		  //�����ջ����ֽ���
#define cTX_LIST_MAX_LEN	20			//���������б�����������

#define cL610_USART UART5				//L610 ͨѶ���ڣ���������Ϊ115200
#define cFLASH_ORDER_ADDRESS 12*1024*1024     //12M��ַ
#define cCommonCheck_Time    2999
#define cSpecialChenk_Time   5999

L610_EXT struct L610_STRUCT_USART_Fram	  //����һ��ȫ�ִ�������֡�Ĵ���ṹ��
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

L610_EXT struct L610_STRUCT_USART_Send	  //����һ��ȫ�ִ��ڷ��͵Ĵ���ṹ��
{
	u8 bTask_Cmd_List[cTX_LIST_MAX_LEN];			//�������������
	u8 bTask_Ack_List[cTX_LIST_MAX_LEN];			//Ӧ����������
	u8 bResend_Count,bData_Mode,bNow_Task;						//ʧ���ط������������ݷ���ģʽ��־
	u16 wRestartCheck_Time;
	u8 bSending_Flag;   		     // 0		���ڷ��ͱ�־
	u8 bSend_Finish;				 		// 1		������ɱ�־
	u8 bFinish_state;							// 2		�������״̬��־��0Ϊ����ʧ��
	u8 bRestart_Check;						// 3		��������־���������ʼ�������
	u8 bResend_Flag;							// 4		���·��ͱ�־
	u8 bResponse_Flag;						// 5-7	Ӧ�����0Ϊ����ҪӦ��
																					//							 	1Ϊ��ͨӦ��
																					//								����1Ϊ����Ӧ��
}L610_Send_Struct;

L610_EXT struct platform_send	  //ƽ̨��Ϣ���ͺʹ洢�ṹ��
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

L610_EXT struct platform_rec	  //ƽ̨������Ϣ�ṹ��
{
	u8 bMode;
	u8 bFlag;
	u16 wLength;
	u16 wChecksum;
	u8 bSer_num;
	u8 bLink_ID;
	u8 bBody[100];
}PLATFORM_REC;

L610_EXT struct lottery	  //�������־�ṹ��
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

L610_EXT u8 bRestart_flag,bSending_Flag;					//������־
L610_EXT u16 bL610_Restart_Check_CNT,wSend_Data_Len;
u16 CalcCrc16 ( u8 *pdata, int len);
void SoftReset(void);         //�������

void L610_printf(char* fmt,...);							//printf����
void L610_Usart_Send(u8 *bData,u16 bLen);			//L610�����ֽں���


//���;�������ĺ���
void L610_Init(void);															//L610ģ��ͨѶ��ʼ��
void L610_qSignal(void);
void L610_Restart(void);
//void L610_Http_Set(u8 *bUrl,u8 *bPort);						//L610 http ������ַ�Ͷ˿ں�
//void L610_Http_Get(u16 bOffset,u16 bSize);					//L610 http get ����ʽ
//void L610_Http_Post(u8 *bCode,u8 bLen);						//L610 http post ��������
void L610_qTcp(void);
void L610_Tcp_Set(u8 *bUrl,u8 *bPort);
void L610_Tcp_Send(u8 *bCode,u16 wLen);




//////////////////////////ʹ��ʱֻ�轫����������������ѭ����ɨ��
void L610_Scan_Send(void);											//��������ɨ�躯��
void L610_Scan_Restart(void);									//L610����ɨ�躯��
u8 L610_Scan(void);								//L610���ڽ���ɨ�躯��



///////////ʹ��ʱ��Ҫ���������ļ���ɨ��
void L610_Restart_Check_Scan(void);					//�ú�����ⷢ�����������Ӧ��ʱ�䣬��ʱ��������ʹ�ô��ڽ���ʱ�����ú������� 1ms ɨ��һ�εĺ�����
void L610_USART_SCAN(void);									//�ú�����⴮�ڽ�����ɣ�ʹ�ô��ڽ���ʱ�����ú������� 1ms ɨ��һ�εĺ�����
void L610_USART_IRQHandler(void);						//�ú���Ϊ���ڽ����жϷ�������ʹ�ô��ڽ���ʱ������������滻����Ӧ���ڵ��жϷ��������������



#endif
