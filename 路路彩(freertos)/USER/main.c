#include "mytasks.h"

//////////////////////////////////////////////////////////////////////////////////	 
//FreeRTOS 任务程序
//作者：袁明正
//联系邮箱：mingzheng.yuan@foxmail.com
//修改日期:2020/8/29
//版本：V1.0
//程序,部分参考正点原子例程
//如有问题，欢迎联系我以改进
//////////////////////////////////////////////////////////////////////////////////

#define START_TASK_PRIO      1
#define START_STK_SIZE       128
TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);


int main(void)
{
	SCB->VTOR = FLASH_BASE | cUSERAPPADDR; /* Vector Table Relocation in Internal FLASH. */
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	delay_init();	    //延时函数初始化	  
	LED_Init();		  	//初始化与LED连接的硬件接口
	uart_init(38400);
	
	uart_init3(115200);
	uart_init4(115200);
	uart_init5(115200);
#if CheckBound==1
	usmart_dev.init(SystemCoreClock/1000000);	//初始化USMART
#endif
	KEY_Init();
	TIM3_Init(9,7199);
	W25QXX_Init();
	Adc_Init();		  		//ADC初始化
	
	xTaskCreate((TaskFunction_t   )start_task,
							(const char*      )"start_task",
							(uint16_t         )START_STK_SIZE,
							(void*            )NULL,
							(UBaseType_t      )START_TASK_PRIO,
							(TaskHandle_t*    )&StartTask_Handler);
							
	vTaskStartScheduler();
}


 /**
 *****************下面注视的代码是通过调用库函数来实现IO控制的方法*****************************************
int main(void)
{ 
 
	delay_init();		  //初始化延时函数
	LED_Init();		        //初始化LED端口
	while(1)
	{
			GPIO_ResetBits(GPIOB,GPIO_Pin_5);  //LED0对应引脚GPIOB.5拉低，亮  等同LED0=0;
			GPIO_SetBits(GPIOE,GPIO_Pin_5);   //LED1对应引脚GPIOE.5拉高，灭 等同LED1=1;
			delay_ms(300);  		   //延时300ms
			GPIO_SetBits(GPIOB,GPIO_Pin_5);	   //LED0对应引脚GPIOB.5拉高，灭  等同LED0=1;
			GPIO_ResetBits(GPIOE,GPIO_Pin_5); //LED1对应引脚GPIOE.5拉低，亮 等同LED1=0;
			delay_ms(300);                     //延时300ms
	}
} 
 
 ****************************************************************************************************
 ***/
 

	
/**
*******************下面注释掉的代码是通过 直接操作寄存器 方式实现IO口控制**************************************
int main(void)
{ 
 
	delay_init();		  //初始化延时函数
	LED_Init();		        //初始化LED端口
	while(1)
	{
     GPIOB->BRR=GPIO_Pin_5;//LED0亮
	   GPIOE->BSRR=GPIO_Pin_5;//LED1灭
		 delay_ms(300);
     GPIOB->BSRR=GPIO_Pin_5;//LED0灭
	   GPIOE->BRR=GPIO_Pin_5;//LED1亮
		 delay_ms(300);

	 }
 }
**************************************************************************************************
**/

