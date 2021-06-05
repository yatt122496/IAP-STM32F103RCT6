#include "mytasks.h"
 

/************************************************
 ALIENTEK精英STM32开发板实验45
 UCOSII实验2-2-任务创建，删除，挂起，恢复
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/


 
/////////////////////////UCOSII任务堆栈设置///////////////////////////////////
//START 任务
//设置任务优先级
#define START_TASK_PRIO      			10 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				64
//创建任务堆栈空间	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数接口
void start_task(void *pdata);
			
int main(void)
{
	SCB->VTOR = FLASH_BASE | cUSERAPPADDR; /* Vector Table Relocation in Internal FLASH. */
	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级   
	uart_init(38400);
	
	uart_init3(115200);
	uart_init4(115200);
	uart_init5(115200);
	LED_Init();		  		//初始化与LED连接的硬件接口
	KEY_Init();
	TIM3_Init(9,7199);
	W25QXX_Init();
	Adc_Init();		  		//ADC初始化
	IWDG_Init(5,625);
	OSInit();  	 			//初始化UCOSII		 			  
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();	    
}

