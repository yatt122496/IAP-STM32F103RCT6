#include "delay.h"
#include "usart.h"
#include "sys.h"
#include "led.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timer.h"
#include "w25qxx.h"
#include "adc.h"

#if CheckBound==1
	#include "usmart.h"
#endif

#include "d1155.h"
#include "voice.h"
#include "L610.h"
#include "handpiece.h"
#define cUSERAPPADDR     (0x0800A000)

extern void start_task(void *pvParameters);
extern TaskHandle_t StartTask_Handler;


typedef enum{
	START_UP                =1,
	STANDBY                 =2,
	LOTTERY_OUT             =3,
	LOTTERY_DONE            =4,
	UPDATE                  =5,
}MAIN_STATE;
