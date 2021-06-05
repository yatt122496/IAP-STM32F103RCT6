#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "iap_app.h"
#include "led.h"
#include "key.h"
#include <string.h>
#include "spi.h"
#include "w25qxx.h"

/************************************************
 ALIENTEK精英STM32开发板实验5
 外部中断 实验 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/



int main(void)
{
	u16 i=0;
	
	SCB->VTOR = FLASH_BASE | 0x10000; /* Vector Table Relocation in Internal FLASH. */
	
	delay_init();	    	 //延时函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级

	uart_init(115200);
	TIM4_Init(9,7199);		//10ms中断
	LED_Init();
	
	W25QXX_Init();
	
	while(1)
	{
		i++;
		bIAP_APP_Scan();
		if(i>50)
		{
			i=0;
			LED2 = ~LED2;
		}
		delay_ms(1);
	}
}

