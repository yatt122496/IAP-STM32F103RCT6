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

#define cD1155_RX_BUF_MAX_LEN 1024		  //�����ջ����ֽ���
#define cD1155_USART	USART1
#define D1155_Printf 0

typedef enum{
	MODE_WORD					=0,						//��ʾ����
	MODE_SPEED				=1,						//�л��ٶ�
	MODE_BRIGHT				=2,						//�л�����
}MODE;
	
typedef enum{
	SPEED0             =0,					//��ʾ�ٶȣ�0����
	SPEED1             =1,
	SPEED2             =2,
	SPEED3             =3,
	SPEED4             =4,
	SPEED5             =5,
	SPEED6             =6,
	SPEED7             =7,
}SPEED;
	
typedef enum{
	BRIGHT0           =0,				//��ʾ���ȣ�0����
	BRIGHT1           =1,
	BRIGHT2           =2,
	BRIGHT3           =3,
}BRIGHT;

typedef enum{
	WORD1             =1,					//��ʾ����1
	WORD2             =2,
	WORD3             =3,
	WORD4             =4,
	WORD5             =5,
	WORD6             =6,
	WORD7             =7,
}WORD;


D1155_EXT struct D1155_STRUCT_USART_Fram	  //����һ��ȫ�ִ�������֡�Ĵ���ṹ��
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

void D1155_printf(char* fmt,...);                            //���� printf ����
#define cD1155_TX_BUF_MAX_LEN  1024

#endif

void D1155_Usart_Send(u8 *bData,u16 wLen);                   //���ڷ����ֽں���


//��������ʾ����
//bMode	��������ģʽ		bMode = 0 Ϊ��ʾ���֣�	bStr = 1 ʱ�� ����Ϊ���й�������Ʊָ�����пռ���������     ··��     ���ز�Ʊ���ۻ�
//bStr	��Ϊ���ݵ�����														 2 	  			 �����ã�����ǰ�����й�������Ʊ�������ۻ�������������ѡ���й�������Ʊ��������ɺ���������
//																								 3			 		 ���ܱ�Ǹ����治�㣬ʣ��Ʊ�����20�������˻�������֧���˻���������������С����ҳ����ϵ�ۺ����
//																								 4					 ���ο��н��������ĺ��뱣���㣬���ϱ��й�Ƭ����ʹ�ù�Ƭ�ο����н���������ĺ���һ�¼ȵ��·��н����
//																								 5					 �����н�����ο����½ǵı����㣬ʹ�ù���ʱ��С����ɨ�豣�����µĶ�ά�룬����ʱ���ˣ�������������С����ҳ����ϵ�ۺ�������ѯ˾��ʦ��
//																								 6					 �����ڳ�Ʊ��
//																								 7					 ���뼰ʱȡƱ
//										bMode = 1 Ϊ�л���ʾ�ٶ�	��bStr ȡֵΪ 0-7 ��0 ����
//														2 Ϊ�л���ʾ���� ��bStr ȡֵΪ 0-3 ��0 ����
//
void D1155_Send_player(MODE mode,u8 bStr);


u8 bD1155_Scan(void);			//�������ݴ����������յ��Ĵ������ݴ��ڴ��ڽ��սṹ���С�


void D1155_Usart_Scan(void);							//ʹ�ô��ڽ���ʱ�����ú������� 1ms ɨ��һ�εĺ�����
void D1155_Usart_IRQHandler(void);				//ʹ�ô��ڽ���ʱ������������滻����Ӧ���ڵ��жϷ��������������

#endif
