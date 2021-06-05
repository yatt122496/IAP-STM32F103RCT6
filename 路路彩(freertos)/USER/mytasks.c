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
//LED任务
#define LED0_TASK_PRIO      2
#define LED0_STK_SIZE       128
TaskHandle_t LED0Task_Handler;
void led0_task(void *pvParameters);

//点阵屏任务
//设置任务优先级
#define D1155_TASK_PRIO      3 
#define D1155_STK_SIZE  		 128
//任务堆栈	
TaskHandle_t D1155Task_Handler;
//任务函数
void d1155_task(void *pvParameters);

//语音任务
//设置任务优先级
#define VOICE_TASK_PRIO       4 
//设置任务堆栈大小
#define VOICE_STK_SIZE  		  128
TaskHandle_t VOICETask_Handler;
void voice_task(void *pvParameters);

//机头任务
//设置任务优先级
#define HANDPIECE_TASK_PRIO       5 
//设置任务堆栈大小
#define HANDPIECE_STK_SIZE  		  128
TaskHandle_t HANDPIECETask_Handler;
void handpiece_task(void *pvParameters);

//4G任务
//设置任务优先级
#define L610_TASK_PRIO       7
//设置任务堆栈大小
#define L610_STK_SIZE  		  128
TaskHandle_t L610Task_Handler;
void l610_task(void *pvParameters);

//主任务
//设置任务优先级
#define MAIN_TASK_PRIO       8
//设置任务堆栈大小
#define MAIN_STK_SIZE  		  1024
TaskHandle_t MAINTask_Handler;
void main_task(void *pvParameters);


/////////////////功能函数声明/////////////////////////
void HandPiece_Light_Set(u8 *pDat,u8 bMode,u8 bNum);
void HandPiece_Lottery_Set(u8 *pDat);
u8 GetLightNum(u8 bCsq);
u16 bp4G_Cmd_Create(u8 bMode,u8 *bDat);

MAIN_STATE start_up(void);
MAIN_STATE standby(void);
MAIN_STATE lottery_out(void);
MAIN_STATE lottery_done(void);


/////////////////起始任务函数////////////////////////////
void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();
	//创建LED0任务
	xTaskCreate((TaskFunction_t   )led0_task,
							(const char*      )"led0_task",
							(uint16_t         )LED0_STK_SIZE,
							(void*            )NULL,
							(UBaseType_t      )LED0_TASK_PRIO,
							(TaskHandle_t*    )&LED0Task_Handler);
							
	//创建点阵屏任务
	xTaskCreate((TaskFunction_t   )d1155_task,
							(const char*      )"d1155_task",
							(uint16_t         )D1155_STK_SIZE,
							(void*            )NULL,
							(UBaseType_t      )D1155_TASK_PRIO,
							(TaskHandle_t*    )&D1155Task_Handler);
	
	//创建语音任务
	xTaskCreate((TaskFunction_t   )voice_task,
							(const char*      )"voice_task",
							(uint16_t         )VOICE_STK_SIZE,
							(void*            )NULL,
							(UBaseType_t      )VOICE_TASK_PRIO,
							(TaskHandle_t*    )&VOICETask_Handler);
							
	//创建机头任务
	xTaskCreate((TaskFunction_t   )handpiece_task,
							(const char*      )"handpiece_task",
							(uint16_t         )HANDPIECE_STK_SIZE,
							(void*            )NULL,
							(UBaseType_t      )HANDPIECE_TASK_PRIO,
							(TaskHandle_t*    )&HANDPIECETask_Handler);
							
	//创建4G任务
	xTaskCreate((TaskFunction_t   )l610_task,
							(const char*      )"l610_task",
							(uint16_t         )L610_STK_SIZE,
							(void*            )NULL,
							(UBaseType_t      )L610_TASK_PRIO,
							(TaskHandle_t*    )&L610Task_Handler);
							
	//主任务
	xTaskCreate((TaskFunction_t   )main_task,
							(const char*      )"main_task",
							(uint16_t         )MAIN_STK_SIZE,
							(void*            )NULL,
							(UBaseType_t      )MAIN_TASK_PRIO,
							(TaskHandle_t*    )&MAINTask_Handler);
	
	pL610_Data=pvPortMalloc(100);
	pL610_Send_Data=pvPortMalloc(100);
	vTaskDelete(StartTask_Handler);
	taskEXIT_CRITICAL();
}




////////////////////////以下是各个任务函数///////////////////////////
void led0_task(void *pvParameters)
{
	u8 b4GDat[100]={0},i=0;
	u16 wLen;
	u32 dwSys_Time=*pSys_Time;
	u32 dw4G_Time=*pSys_Time;
	u32 dwCsq_Time=*pSys_Time;
	for(;;)
	{
		if(LOTTERY.bCheck_Switch==STANDBY)
		{
			if(*pSys_Time-dwSys_Time>=60000)
			{
				VTXprintf("Heart_Sign ");
				LOTTERY.bHeart_Sign=1;
			}
			else if(*pSys_Time-dw4G_Time>=30000)
			{
				dw4G_Time=*pSys_Time;
				VTXprintf("4GOnlineCheck ");
				LOTTERY.b4GOnlineCheck_Flag=1;
			}
			else if(*pSys_Time-dwCsq_Time>=15000)
			{
				dwCsq_Time=*pSys_Time;
				VTXprintf("CsqCheck ");
				LOTTERY.bCsqCheck_Flag=1;
			}
		}
		else if(LOTTERY.bCheck_Switch!=START_UP)
		{
			if(*pSys_Time-dwSys_Time>=60000)
			{
				VTXprintf("Heart_Sign ");
				LOTTERY.bHeart_Sign=1;
			}
		}
			
		if(LOTTERY.bHeart_Sign==1)
		{
			LOTTERY.bHeart_Sign=0;
			dwSys_Time=*pSys_Time;
			
			delay_ms(100);
			while(bSending_Flag)delay_ms(20);
			wLen=bp4G_Cmd_Create(0x05,b4GDat);
			L610_Tcp_Send(b4GDat,wLen);
		}
		
		if(LOTTERY.b4GOnlineCheck_Flag==1)
		{
			LOTTERY.b4GOnlineCheck_Flag=0;
			delay_ms(100);
			while(bSending_Flag)delay_ms(20);
			L610_qTcp();
		}
		
		if(LOTTERY.bCsqCheck_Flag==1)
		{
			LOTTERY.bCsqCheck_Flag=0;
			delay_ms(100);
			while(bSending_Flag)delay_ms(20);
			L610_qSignal();
		}
		i++;
		if(i==30)
		{
			i=0;
			LED2=~LED2;
//			VTXprintf("ROOM=%d\r\n",xPortGetFreeHeapSize());
//			VTXprintf("LED0Task_Handler ROOM is %d \r\n",uxTaskGetStackHighWaterMark(LED0Task_Handler));
//			VTXprintf("D1155Task_Handler ROOM is %d \r\n",uxTaskGetStackHighWaterMark(D1155Task_Handler));
//			VTXprintf("VOICETask_Handler ROOM is %d \r\n",uxTaskGetStackHighWaterMark(VOICETask_Handler));
//			VTXprintf("HANDPIECETask_Handler ROOM is %d \r\n",uxTaskGetStackHighWaterMark(HANDPIECETask_Handler));
//			VTXprintf("L610Task_Handler ROOM is %d \r\n",uxTaskGetStackHighWaterMark(L610Task_Handler));
//			VTXprintf("MAINTask_Handler ROOM is %d \r\n",uxTaskGetStackHighWaterMark(MAINTask_Handler));
//		VTXprintf("LED ");
		}
		
		delay_ms(10);
	}
}

void d1155_task(void * pvParameters)
{
	u8 bRes=0,i=0;
	delay_ms(1000);
	
	for(;;)
	{
		bRes=bD1155_Scan();
		if(bRes)
			LOTTERY.bD1155_Reply=bRes;
		i++;
		if(i==30)
		{
			i=0;
//			VTXprintf("D1155 ");
		}
		delay_ms(10);
	}
}

void voice_task(void * pvParameters)
{
	u8 bRes=0,i=0;
	delay_ms(1000);
	for(;;)
	{
		bRes=bVOICE_Scan();
		if(bRes)
			LOTTERY.bVoice_Reply=bRes;
		i++;
		if(i==30)
		{
			i=0;
//			VTXprintf("VOICE ");
		}
		delay_ms(10);
	}
}

void handpiece_task(void * pvParameters)
{
	u8 bRes=0,i=0;
	
	delay_ms(1000);
	for(;;)
	{
		bRes=bHandPiece_Scan();
		if(bRes)
			LOTTERY.bHandPiece_Reply=bRes;
		i++;
		if(i==30)
		{
			i=0;
//			VTXprintf("HANDPIECE ");
		}
		delay_ms(10);
	}
}

void l610_task(void * pvParameters)
{
	u8 bRes=0,i=0;
	delay_ms(1000);
	for(;;)
	{
		L610_Scan_Send();
		bRes=L610_Scan();
		if(bRes)
		{
			LOTTERY.bL610_Reply=bRes;
		}
		L610_Scan_Restart();
		i++;
		if(i==30)
		{
			i=0;
//			VTXprintf("L610 ");
		}
		delay_ms(10);
	}
}

//主任务
void main_task(void *pdata)
{
	MAIN_STATE MainState=START_UP;
	delay_ms(2000);
	W25QXX_Read(PLATFORM.bOrder_ID_Bkp,cFLASH_ORDER_ADDRESS,9);
	for(;;)
	{
		switch((int)MainState)
		{
			case START_UP:
			{
				MainState=start_up();
				break;
			}
			case STANDBY:
			{
				MainState=standby();
				break;
			}
			case LOTTERY_OUT:
			{
				MainState=lottery_out();
				break;
			}
			case LOTTERY_DONE:
			{
				MainState=lottery_done();
				break;
			}
			case UPDATE:
			{
				
				break;
			}
			default:
				break;
		}
		delay_ms(10);
	}
}

//////////////以下是主功能函数////////////////////////

MAIN_STATE start_up(void)
{
	MAIN_STATE state=START_UP;
	u8 bDat[20],b4GDat[100]={0},i=0;
	u16 wLen;
	
	LOTTERY.bCheck_Switch=state;
	D1155_Send_player(MODE_WORD,0);
	HandPiece_Light_Set(bDat,3,5);
	HandPiece_Send_Cmd(HANDPIECE_LIGHTING,bDat);
	VOICE_Play1(21);
	
	L610_Init();
	
	delay_ms(100);
	while(bSending_Flag)delay_ms(20);
	L610_Tcp_Set(NULL,NULL);
	
	delay_ms(100);
	while(bSending_Flag)delay_ms(20);
	wLen=bp4G_Cmd_Create(0x01,b4GDat);
	L610_Tcp_Send(b4GDat,wLen);
	
	for(;;)
	{
		if(LOTTERY.bL610_Reply==0x02)
		{
			LOTTERY.bL610_Reply=0;
			
			delay_ms(100);
			while(bSending_Flag)delay_ms(50);
			wLen=bp4G_Cmd_Create(0x03,b4GDat);
			L610_Tcp_Send(b4GDat,wLen);
			
			while(1)
			{
				if(LOTTERY.bVoice_Reply==0x3e)
				{
					LOTTERY.bVoice_Reply=0;
					break;
				}
				delay_ms(10);
			}
			if(PLATFORM.bActice_State)
			{
				state = STANDBY;
				break;
			}
		}
		if(LOTTERY.bRestart_flag)
		{
			LOTTERY.bRestart_flag=0;
			state = START_UP;
			break;
		}
		i++;
		if(i==30)
		{
			i=0;
			VTXprintf("START_UP ");
		}
		delay_ms(10);
	}
	return state;
}

MAIN_STATE standby(void)
{
	MAIN_STATE state=STANDBY;
	u8 bDat[20],i=0,bPeople_Flag=0,bPeopleVoiceStart_Flag=1;
	u16 wAdcx=0;
	u32 dwPeopleVoice_Time=*pSys_Time;
	
	LOTTERY.bCheck_Switch=state;
	delay_ms(100);
	HandPiece_Light_Set(bDat,1,GetLightNum(PLATFORM.bCsq));
	HandPiece_Send_Cmd(HANDPIECE_LIGHTING,bDat);
	for(;;)
	{
		if(LOTTERY.bD1155_Reply==0x02)
		{
			LOTTERY.bD1155_Reply=0;
			D1155_Send_player(MODE_WORD,0);
		}
		
		if(LOTTERY.bHandPiece_Reply==0xC4)
		{
			LOTTERY.bHandPiece_Reply=0;
			if(HANDPIECE.bHandPiece_State)
				HandPiece_Rec_Cmd(HANDPIECE_READLY);
		}
		else if(LOTTERY.bHandPiece_Reply==0xC2)
		{
			LOTTERY.bHandPiece_Reply=0;
			state=LOTTERY_DONE;
			VOICE_Stop();
			break;
		}
		else if(LOTTERY.bHandPiece_Reply==0xC1)
		{
			LOTTERY.bHandPiece_Reply=0;
			LOTTERY.bPowerDown=1;
			state=LOTTERY_OUT;
			VOICE_Stop();
			break;
		}
		
		if(LOTTERY.bL610_Reply==0x11)
		{
			LOTTERY.bL610_Reply=0;
			LOTTERY.bLotteryOut_Flag=1;
			state=LOTTERY_OUT;
			VOICE_Stop();
			break;
		}
		else if(LOTTERY.bL610_Reply==0xA1)
		{
			LOTTERY.bL610_Reply=0;
			delay_ms(100);
			HandPiece_Light_Set(bDat,1,GetLightNum(PLATFORM.bCsq));
			HandPiece_Send_Cmd(HANDPIECE_LIGHTING,bDat);
		}
		else if(LOTTERY.bL610_Reply==0xA2)
		{
			LOTTERY.bL610_Reply=0;
			if(LOTTERY.bL610_Online==0)
			{
				state=START_UP;
				VOICE_Stop();
				break;
			}
		}
		
		if(LOTTERY.bMasterControlUpdate_Flag==1)
		{
			LOTTERY.bMasterControlUpdate_Flag=0;
			state=UPDATE;
			VOICE_Stop();
			break;
		}
		
		if(LOTTERY.bRestart_flag==1)
		{
			LOTTERY.bRestart_flag=0;
			state = START_UP;
			VOICE_Stop();
			break;
		}
		
		if(bPeople_Flag==1)
		{
			if(*pSys_Time-dwPeopleVoice_Time >= 30000||bPeopleVoiceStart_Flag==1)
			{
				bPeopleVoiceStart_Flag=0;
				dwPeopleVoice_Time=*pSys_Time;
				VOICE_Play1(1);
			}
		}
		
		i++;
		if(i==30)
		{
			i=0;
			wAdcx=(u32)Get_Adc_Average(ADC_Channel_10,10)*3300/4096;
//			VTXprintf("Adcx=%d ",wAdcx);
			if(wAdcx>3200)
			{
				bPeople_Flag=1;
				VTXprintf("There Are People.\r\n");
			}
			else
				bPeople_Flag=0;
			VTXprintf("STANDBY ");
		}
		delay_ms(10);
	}
	return state;
}

MAIN_STATE lottery_out(void)
{
	MAIN_STATE state=LOTTERY_OUT;
	u8 bDat[20],i=0;
	u32 dwCollectionLottery_Time=0;
	
	LOTTERY.bCheck_Switch=state;
	delay_ms(100);
	HandPiece_Light_Set(bDat,5,GetLightNum(PLATFORM.bCsq));
	HandPiece_Send_Cmd(HANDPIECE_LIGHTING,bDat);
	
	LOTTERY.bHandPiece_Reply=0;
	if(LOTTERY.bPowerDown)
	{
		LOTTERY.bPowerDown=0;
		LOTTERY.bHandPiece_Reply=0xC1;
		HANDPIECE.bLottery_State=0x08;
	}
	else
	{
		delay_ms(100);
		HandPiece_Lottery_Set(bDat);
		HandPiece_Send_Cmd(HANDPIECE_LOTTERY_OUT,bDat);
		while(1)
		{
			if(LOTTERY.bHandPiece_Reply==0x81)
			{
				LOTTERY.bHandPiece_Reply=0;
				if(HANDPIECE.bHandReply_Code==0x91)
				{
					D1155_Send_player(MODE_WORD,5);
					VOICE_Play1(6);
					break;
				}
			}
			delay_ms(10);
		}
	}
	
	for(;;)
	{
		if(LOTTERY.bHandPiece_Reply==0xC1)
		{
			LOTTERY.bHandPiece_Reply=0;
			if(HANDPIECE.bLottery_State==0x08)
			{
				dwCollectionLottery_Time=*pSys_Time;
				D1155_Send_player(MODE_WORD,6);
				VOICE_Play1(8);
				LOTTERY.bCollectionLottery_Voice_Flag=1;
			}
			else
			{
				D1155_Send_player(MODE_WORD,5);
				VOICE_Stop();
				LOTTERY.bCollectionLottery_Voice_Flag=0;
			}
		}
		else if(LOTTERY.bHandPiece_Reply==0xC2)
		{
			LOTTERY.bHandPiece_Reply=0;
			VOICE_Stop();
			LOTTERY.bCollectionLottery_Voice_Flag=0;
			LOTTERY.bLotteryOut_Flag=0;
			state=LOTTERY_DONE;
			break;
		}
		if(LOTTERY.bCollectionLottery_Voice_Flag)
		{
			if(*pSys_Time-dwCollectionLottery_Time >= 15000)
			{
				dwCollectionLottery_Time=*pSys_Time;
				VOICE_Play1(8);
			}
		}
		if(LOTTERY.bRestart_flag)
		{
			LOTTERY.bRestart_flag=0;
			LOTTERY.bCollectionLottery_Voice_Flag=0;
			state = START_UP;
			break;
		}
		i++;
		if(i==30)
		{
			i=0;
			VTXprintf("LOTTERY_OUT ");
		}
		delay_ms(10);
	}
	return state;
}

MAIN_STATE lottery_done(void)
{
	MAIN_STATE state=LOTTERY_DONE;
	u8 bDat[20],b4GDat[100]={0},i=0,bD1155_ShowWord_Flag=0;
	u16 wLen;
	
	LOTTERY.bCheck_Switch=state;
	delay_ms(100);
	HandPiece_Light_Set(bDat,1,GetLightNum(PLATFORM.bCsq));
	HandPiece_Send_Cmd(HANDPIECE_LIGHTING,bDat);
	
	LOTTERY.bD1155_Reply=0;
	D1155_Send_player(MODE_WORD,7);
	delay_ms(20);
	D1155_Send_player(MODE_SPEED,6);
	LOTTERY.bVoice_Reply=0;
	VOICE_Play1(10);
	
	delay_ms(100);
	while(bSending_Flag)delay_ms(20);
	wLen=bp4G_Cmd_Create(0x12,b4GDat);
	L610_Tcp_Send(b4GDat,wLen);
	
	for(;;)
	{
		if(!bD1155_ShowWord_Flag)
		{
			if(LOTTERY.bD1155_Reply==2)
			{
				D1155_Send_player(MODE_WORD,4);
				delay_ms(100);
				D1155_Send_player(MODE_SPEED,6);
				if(LOTTERY.bVoice_Reply==0x3E)
				{
					VOICE_Play1(11);
					LOTTERY.bD1155_Reply=0;
					LOTTERY.bVoice_Reply=0;
					bD1155_ShowWord_Flag=1;
				}
			}
		}
		if(LOTTERY.bD1155_Reply==2&&LOTTERY.bVoice_Reply==0x3E)
		{
			LOTTERY.bD1155_Reply=0;
			LOTTERY.bVoice_Reply=0;
			D1155_Send_player(MODE_WORD,0);
			state=STANDBY;
			break;
		}
		if(LOTTERY.bL610_Reply==0x11)
		{
			LOTTERY.bL610_Reply=0;
			LOTTERY.bD1155_Reply=0;
			LOTTERY.bVoice_Reply=0;
			LOTTERY.bLotteryOut_Flag=1;
			state=LOTTERY_OUT;
			break;
		}
		if(LOTTERY.bRestart_flag)
		{
			LOTTERY.bRestart_flag=0;
			state = START_UP;
			break;
		}
		
		i++;
		if(i==30)
		{
			i=0;
			VTXprintf("LOTTERY_DONE ");
		}
		delay_ms(10);
	}
	return state;
}

MAIN_STATE update(void)
{
	MAIN_STATE state=UPDATE;
	u8 bDat[20],b4GDat[100]={0},i=0;
	u16 wLen;
	
	for(;;)
	{
		i++;
		if(i==30)
		{
			i=0;
			VTXprintf("UPDATE ");
		}
		delay_ms(10);
	}
	return state;
}

//////////////以下是功能函数//////////////////////////
void HandPiece_Light_Set(u8 *pDat,u8 bMode,u8 bNum)
{
	if(bMode==0||bMode>0x04)
	{
		pDat[1]=0;
	}
	else
		pDat[1]=bNum;
	pDat[0]=bMode;
}

void HandPiece_Lottery_Set(u8 *pDat)
{
	u8 i;
	for(i=0;i<9;i++)
		pDat[i]=PLATFORM.bOrder_ID_Bkp[i];
	pDat[i]=PLATFORM.dwOrder_num;
}

u8 GetLightNum(u8 bCsq)
{
	u8 bRes=1;
	if(bCsq>24)
	{
		bRes=5;
	}
	else if(bCsq>18)
	{
		bRes=4;
	}
	else if(bCsq>11)
	{
		bRes=3;
	}
	else if(bCsq>5)
	{
		bRes=2;
	}
	else
	{
		bRes=1;
	}
	return bRes;
}

u16 bp4G_Cmd_Create(u8 bMode,u8 *bDat)
{
	u16 i,wLen=0;
	PLATFORM.bMode=bMode;
	switch(bMode)     //case 只需设置发送的 BODY 即可
	{
		case 0x01:
		{
			PLATFORM.wLength=25;
			bDat[9]=PLATFORM.bID_Code;
			bDat[10]=PLATFORM.bHeadIMEI;
			for(i=0;i<15;i++)
			{
				bDat[i+11]=PLATFORM.bIMEI[i];
			}
			for(i=0;i<8;i++)
			{
				bDat[i+26]=PLATFORM.bChallengeCode[i];
			}
			break;
		}
		case 0x03:
		{
			PLATFORM.wLength=0;
			break;
		}
		case 0x05:
		{
			PLATFORM.wLength=26;
			bDat[9]=PLATFORM.bHeadIMEI;
			for(i=0;i<15;i++)
				bDat[i+10]=PLATFORM.bIMEI[i];
			bDat[25]=0;
			for(i=0;i<9;i++)
				bDat[i+26]=0;
			break;
		}
		case 0x12:
		{
			PLATFORM.wLength=34;
			bDat[9]=0;
			for(i=0;i<4;i++)
				bDat[i+10]=PLATFORM.bOrder_time[i];
			for(i=0;i<16;i++)
				bDat[i+14]=PLATFORM.bDevice_ID[i];
			for(i=0;i<9;i++)
				bDat[i+30]=HANDPIECE.bLottery_Order[i];
			for(i=0;i<3;i++)
				bDat[i+39]=0;
			bDat[42]=HANDPIECE.bLottery_Order[9];
			break;
		}
		default:
			break;
	}
	bDat[0]=PLATFORM.bStart_Code;
	bDat[1]=PLATFORM.bMode;
	bDat[2]=wLen/256;bDat[3]=wLen%256;
	bDat[4]=PLATFORM.bFlag;
	bDat[5]=0;bDat[6]=0;
	bDat[7]=PLATFORM.bSer_num++;
	bDat[8]=PLATFORM.bLink_ID;
	bDat[PLATFORM.wLength+9]=PLATFORM.bEnd_Code;
	
	PLATFORM.wChecksum=CalcCrc16(bDat,PLATFORM.wLength+10);
	
	bDat[5]=PLATFORM.wChecksum/256;bDat[6]=PLATFORM.wChecksum%256;
	return PLATFORM.wLength+10;
}
