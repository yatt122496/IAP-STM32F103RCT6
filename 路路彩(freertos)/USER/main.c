#include "mytasks.h"

//////////////////////////////////////////////////////////////////////////////////	 
//FreeRTOS �������
//���ߣ�Ԭ����
//��ϵ���䣺mingzheng.yuan@foxmail.com
//�޸�����:2020/8/29
//�汾��V1.0
//����,���ֲο�����ԭ������
//�������⣬��ӭ��ϵ���ԸĽ�
//////////////////////////////////////////////////////////////////////////////////

#define START_TASK_PRIO      1
#define START_STK_SIZE       128
TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);


int main(void)
{
	SCB->VTOR = FLASH_BASE | cUSERAPPADDR; /* Vector Table Relocation in Internal FLASH. */
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	delay_init();	    //��ʱ������ʼ��	  
	LED_Init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ�
	uart_init(38400);
	
	uart_init3(115200);
	uart_init4(115200);
	uart_init5(115200);
#if CheckBound==1
	usmart_dev.init(SystemCoreClock/1000000);	//��ʼ��USMART
#endif
	KEY_Init();
	TIM3_Init(9,7199);
	W25QXX_Init();
	Adc_Init();		  		//ADC��ʼ��
	
	xTaskCreate((TaskFunction_t   )start_task,
							(const char*      )"start_task",
							(uint16_t         )START_STK_SIZE,
							(void*            )NULL,
							(UBaseType_t      )START_TASK_PRIO,
							(TaskHandle_t*    )&StartTask_Handler);
							
	vTaskStartScheduler();
}


 /**
 *****************����ע�ӵĴ�����ͨ�����ÿ⺯����ʵ��IO���Ƶķ���*****************************************
int main(void)
{ 
 
	delay_init();		  //��ʼ����ʱ����
	LED_Init();		        //��ʼ��LED�˿�
	while(1)
	{
			GPIO_ResetBits(GPIOB,GPIO_Pin_5);  //LED0��Ӧ����GPIOB.5���ͣ���  ��ͬLED0=0;
			GPIO_SetBits(GPIOE,GPIO_Pin_5);   //LED1��Ӧ����GPIOE.5���ߣ��� ��ͬLED1=1;
			delay_ms(300);  		   //��ʱ300ms
			GPIO_SetBits(GPIOB,GPIO_Pin_5);	   //LED0��Ӧ����GPIOB.5���ߣ���  ��ͬLED0=1;
			GPIO_ResetBits(GPIOE,GPIO_Pin_5); //LED1��Ӧ����GPIOE.5���ͣ��� ��ͬLED1=0;
			delay_ms(300);                     //��ʱ300ms
	}
} 
 
 ****************************************************************************************************
 ***/
 

	
/**
*******************����ע�͵��Ĵ�����ͨ�� ֱ�Ӳ����Ĵ��� ��ʽʵ��IO�ڿ���**************************************
int main(void)
{ 
 
	delay_init();		  //��ʼ����ʱ����
	LED_Init();		        //��ʼ��LED�˿�
	while(1)
	{
     GPIOB->BRR=GPIO_Pin_5;//LED0��
	   GPIOE->BSRR=GPIO_Pin_5;//LED1��
		 delay_ms(300);
     GPIOB->BSRR=GPIO_Pin_5;//LED0��
	   GPIOE->BRR=GPIO_Pin_5;//LED1��
		 delay_ms(300);

	 }
 }
**************************************************************************************************
**/

