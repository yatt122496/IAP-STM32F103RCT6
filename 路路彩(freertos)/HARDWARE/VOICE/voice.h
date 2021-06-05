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

#define cVOICE_RX_BUF_MAX_LEN 256		  //�����ջ����ֽ���

#define cVOICE_USART	UART4					//��������ģ��ʹ�õĴ���,,,,������Ϊ 115200


VOICE_EXT struct VOICE_STRUCT_USART_Fram	  //����һ��ȫ�ִ�������֡�Ĵ���ṹ��
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

VOICE_EXT struct voice	  //����һ��ȫ�ִ�������֡�Ĵ���ṹ��
{
	u8 bVoice_PlayState;
	u8 bVoice_NowVolume_Level;
}VOICE;

/***********************************************************************************
*																						 ��Ӧ����											����( wDat )
*bCMD						 ��������� bCMD Ϊ  0x01		��һ��
*																			0x02		��һ��
*																			0x03		ָ����Ŀ(NUM)							��Ŀ��� ���һ�� ��wDat Ϊ0x0001
*																			0x04		����+
*																			0x05		����-
*																			0x06		ָ������									������ΧΪ 0-30 ����������ʱ wDat Ϊ��������
*																			0x0C		оƬ��λ
*																			0x0D		����
*																			0x0E		��ͣ
*																			0x16		ֹͣ
*																			0xA5		ָ����һ��������					wDat Ϊ��Ŀ��ţ���ΧΪ 1-255
*																			0xA6		ָ���ڶ���������					wDat Ϊ��Ŀ��ţ�(�ڶ������Ǵ������ؽ�ȥ�����֣�ÿ�����ػ�ɾ����������)
*																			0x3F		��ѯ��ʼ��״̬
*																			0x40		���ش��������ط�
*																			0x42		��ѯ��ǰ״̬
*																			0x43		��ѯ��ǰ����
*																			0x49		��ѯ FLASH �����ļ���			���ز���	���ֽ�[��һ����] ���ֽ�[�ڶ�����]
*																			0x50		��ѯʹ�õ� flash ������
*************************************************************************************/


//////////////�ú�����������ģ�鷢�����ݵ�Э�飬��ʹ��ʱ�������
void VOICE_SendCmd(u8 bCMD , u16 wDat);


////////////////////���ڴ�ӡ������������һ�����ַ�����ӡ�����ڶ����ǰ��ֽڷ�������
void VOICE_printf(char* fmt,...) ;
void VOICE_Usart_Send(u8 *bData,u16 wLen);


///////////ʹ�ô��ڽ���ʱ������������Ҫ���ڶ�Ӧλ��
void VOICE_Usart_Scan(void);							//ʹ�ô��ڽ���ʱ�����ú������� 1ms ɨ��һ�εĺ�����
void VOICE_Usart_IRQHandler(void);				//ʹ�ô��ڽ���ʱ������������滻����Ӧ���ڵ��жϷ��������������




u8 bVOICE_Scan(void);										//���ڽ���ɨ�躯�������ؽ��յ������ݵ���ʼ��ַ��δ���յ������򷵻�0


////////////////////////���º����ɹ�����
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
