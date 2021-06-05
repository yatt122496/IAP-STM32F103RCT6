#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "iap_app.h"
#include "led.h"
#include "key.h"
#include <string.h>
#include "spi.h"
#include "w25qxx.h"
#include "d1155.h"

/************************************************
 ALIENTEK��ӢSTM32������ʵ��5
 �ⲿ�ж� ʵ�� 
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/



int main(void)
{
	u8 j=0,l=1;
	u16 i=0;
	
	SCB->VTOR = FLASH_BASE | 0x10000; /* Vector Table Relocation in Internal FLASH. */
	
	delay_init();	    	 //��ʱ������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

	uart_init(38400);
	TIM4_Init(9,7199);		//10ms�ж�
	LED_Init();
	
	W25QXX_Init();
	
	while(1)
	{
		i++;
		bIAP_APP_Scan();
		if(i>500)
		{
			j++;
			if(j>20)
			{
				j=0;
				D1155_Send_Player(MODE0,l);
				l++;
				if(l>7)
					l=0;
			}
			i=0;
			LED2 = ~LED2;
		}
		delay_ms(1);
	}
}

