#include "mytasks.h"
 

/************************************************
 ALIENTEK��ӢSTM32������ʵ��45
 UCOSIIʵ��2-2-���񴴽���ɾ�������𣬻ָ�
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/


 
/////////////////////////UCOSII�����ջ����///////////////////////////////////
//START ����
//�����������ȼ�
#define START_TASK_PRIO      			10 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				64
//���������ջ�ռ�	
OS_STK START_TASK_STK[START_STK_SIZE];
//�������ӿ�
void start_task(void *pdata);
			
int main(void)
{
	SCB->VTOR = FLASH_BASE | cUSERAPPADDR; /* Vector Table Relocation in Internal FLASH. */
	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�   
	uart_init(38400);
	
	uart_init3(115200);
	uart_init4(115200);
	uart_init5(115200);
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();
	TIM3_Init(9,7199);
	W25QXX_Init();
	Adc_Init();		  		//ADC��ʼ��
	IWDG_Init(5,625);
	OSInit();  	 			//��ʼ��UCOSII		 			  
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	OSStart();	    
}

