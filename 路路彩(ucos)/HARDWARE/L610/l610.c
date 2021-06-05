#include "L610.h"

#include "delay.h"
#include "timer.h"
#include "usart.h"


//////////////////////////////////////////////////////////////////////////////////	 
//4G模块初始化
//作者：袁明正
//联系邮箱：mingzheng.yuan@foxmail.com
//修改日期:2020/8/27
//版本：V1.0
//程序,部分参考正点原子例程
//如有问题，欢迎联系我以改进
////////////////////////////////////////////////////////////////////////////////// 	 


#define L610_GLOBALS

u8 bRestart_flag=0,bSending_Flag=0;					//重启标志
u16 wL610_Restart_Check_CNT=0,wSend_Data_Len=0;

static u8 bSend_Data_flag=0;
static void L610_Send_Cmd(u16 wLen);
static void L610_Send_AT_Cmd(u8 *bCmd,u16 wLen);
static u8 *bL610_Check_Data(u8 *bStr);
static void L610_Load_Cmd(u8 bLen,u8 *bL610_Cmd_List,u8 *bL610_Ack_List);

struct lottery LOTTERY={0};
struct L610_STRUCT_USART_Fram L610_Fram_Record_Struct = { 0 };		//L610串口接收结构体
struct L610_STRUCT_USART_Send L610_Send_Struct = {
	.wRestartCheck_Time=cCommonCheck_Time,
};		//L610串口发送结构体
struct platform_send PLATFORM = {
	.bHeadIMEI=0x30,
	.bChallengeCode="199Na95n",
	.bStart_Code=0x5a,
	.bFlag=0,
	.bSer_num=0,
	.bLink_ID=0x06,
	.bID_Code=0x01,
	.bEnd_Code=0x77
};
struct platform_rec PLATFORM_REC = { 
	.bLink_ID=0x06 
};		//L610串口发送结构体



//定义发送的命令
const u8 bL610_AT_Cmd_List1[][80]={
	"",																		////  0
	"AT",																	////  1
	"ATE0",																////  2
	"AT+CPIN?",														////  3
	"AT+CGDCONT=1,\"IP\",\"cmnet\"",			////	4
	"AT+CSQ",															////  5
	"AT+CREG?",														////  6
	"AT+MIPCALL=1",												////	7
	"AT+HTTPSET=\"URL\",\"http://www.baidu.com:80\"",			//// 8
	"AT+HTTPDATA=10",											////	9
	"AT+HTTPACT=1,30",										////	10
	"AT+HTTPREAD",												////	11
	"AT+HTTPACT=0",												////	12
	"AT+CFUN=15",													////	13
	"",																		////	14
	"AT+MIPCALL?",												////	15
	"AT+MIPOPEN?",												////	16
	"AT+MIPOPEN=1,,\"v1-tml.lulucai.cn\",18060,0",			////	17
//	"AT+MIPOPEN=1,,\"www.baidu.com\",80,0",			////	17
	"AT+MIPSEND=1,10",										////	18
	"AT+MIPCLOSE=1",											////	19
	"AT+GTSET=\"IPRFMT\",2",							////	20
	"AT+GSN?",							              ////	21
	"AT+CGATT=1",							            ////	22
	"AT+CEREG?",							            ////	23
	
};

//定义发送的命令
u8 bL610_Send_Data[100]={0};                ////  0
//定义发送的命令
u8 bL610_ATSend_Data[30]="AT+MIPSEND=1,10"	;////	0
u8 bL610_Data[100]={0};

//定义L610模块命令应答
const u8 bL610_AT_Ack_List[][20]={
	"",																		////  0
	"OK",																	////  1
	"READY",															////  2
	"+CSQ:",															////  3
	"+CREG: 0,",													////  4
	"+MIPCALL:",													////  5
	">",																	////  6
	"+HTTP: 1",														////  7
	"+HTTPREAD: ",												////  8
	"+HTTPRES:",													////  9
	"+MIPOPEN:",													////  10
	"+MIPSEND:",													////  11
	"+MIPCLOSE:",													////  12
	"+GSN:",									    				////  13
	"+CEREG: 0,",													////  14
	
};


//定义枚举数组
enum {
	CommandNull=0,
	Command1,
	Command2,
	Command3,
	Command4,
	Command5,
	Command6,
	Command7,
	Command8,
	Command9,
	Command10,
	Command11,
	Command12,
	Command13,
	Command14,
	Command15,
	Command16,
	Command17,
	Command18,
	Command19,
	Command20,
	Command21,
	Command22,
	Command23,
};
	
//向4g模块发送AT指令
//cmd					：发送的指令
//acd					：期待的应答结果，如果为 NULL，则表示不需要等待应答
static void L610_Send_AT_Cmd(u8 *bCmd,u16 wLen)
{
	L610_Fram_Record_Struct.wInfAll = 0;	//从新开始接收新的数据包
	if(bSend_Data_flag)
	{
		bSend_Data_flag=0;
		L610_Usart_Send(bCmd,wSend_Data_Len);
	}
	else
		L610_printf("%s\r\n",bCmd);
}


//查找接收缓冲区字符串
//bStr			:		需要查找的字符串
//返回值			:  			0，没有找到该字符串
//						 其他值，返回该字符串所在位置的指针
static u8 *bL610_Check_Data(u8 *bStr)
{
	char *bStrx=0;
	if((strcmp((const char *)L610_Fram_Record_Struct.bData_RX_BUF,"\r\n")==0)||(strcmp((const char *)L610_Fram_Record_Struct.bData_RX_BUF,"")==0))
		return 0;
	VTXprintf("L610 Rec:%s\r\n", L610_Fram_Record_Struct.bData_RX_BUF);
	bStrx=strstr((const char*)L610_Fram_Record_Struct.bData_RX_BUF,(const char *)bStr);
	return (u8 *)bStrx;
}

static void L610_Load_Cmd(u8 bLen,u8 *bL610_Cmd_List,u8 *bL610_Ack_List)
{
	u8 i=L610_Send_Struct.bNow_Task,bOffset=0,j;
	if(bSending_Flag)
	{
		while(L610_Send_Struct.bTask_Cmd_List[i]!=0)
			i++;
		bOffset=i;
		for(i=0;i<bLen;i++)
		{
			j=i+bOffset;
			if(j>29)
				j=j-30;
			L610_Send_Struct.bTask_Cmd_List[j]=bL610_Cmd_List[i];
			L610_Send_Struct.bTask_Ack_List[j]=bL610_Ack_List[i];
		}
	}
	else
	{
		for(i=0;i<bLen;i++)
		{
			L610_Send_Struct.bTask_Cmd_List[i]=bL610_Cmd_List[i];
			L610_Send_Struct.bTask_Ack_List[i]=bL610_Ack_List[i];
		}
		L610_Send_Struct.bNow_Task=0;
	}
	L610_Send_Struct.bResend_Count=3;
}

//L610初始化程序
void L610_Init(void)
{
	u8 i;
	u8 bL610_Cmd_List[12+1]={ Command1, Command2, Command3, Command22, Command4, 
		Command5, Command23, Command6, Command15, Command7, Command20, Command21, CommandNull};
	u8 bL610_Ack_List[12+1]={ Command1, Command1, Command2, Command1,  Command1, 
		Command3, Command14, Command4, Command5,  Command5, Command1,  Command13, CommandNull};
	
	for(i=0;i<3;i++)
	{
		L610_printf("AT\r\n");
		delay_ms(500);
	}
	L610_printf("AT+CFUN=15\r\n");
	
	delay_ms(3000);
	
	for(i=0;i<3;i++)
	{
		L610_printf("AT\r\n");
		delay_ms(500);
	}
	L610_Send_Struct.bNow_Task=0;
	L610_Load_Cmd(12+1,bL610_Cmd_List,bL610_Ack_List);
}

void L610_qSignal(void)
{
	u8 bL610_Cmd_List[1+1]={ Command5, CommandNull};
	u8 bL610_Ack_List[1+1]={ Command3,  CommandNull};
	L610_Load_Cmd(2,bL610_Cmd_List,bL610_Ack_List);
}

void L610_Restart(void)
{
	u8 bL610_Cmd_List[1+1]={ Command13, CommandNull};
	u8 bL610_Ack_List[1+1]={ Command1,  CommandNull};
	L610_Load_Cmd(2,bL610_Cmd_List,bL610_Ack_List);
}

///HTTP SET
//url			：目标网址
//port		：目标端口
//void L610_Http_Set(u8 *bUrl,u8 *bPort)
//{
//	u8 bL610_Cmd_List[1+1]={ Command8, CommandNull};
//	u8 bL610_Ack_List[1+1]={ Command1, CommandNull};
//	sprintf((char *)bL610_AT_Cmd_List[Command8],"AT+HTTPSET=\"URL\",\"%s:%s\"",bUrl,bPort);
//	L610_Load_Cmd(2,bL610_Cmd_List,bL610_Ack_List);
//}


///HTTP GET 请求方法
//offset	：分段下载时的偏移地址
//size		：分段下载时每段下载大小
//	不使用分段下载时，offset ,size 都设置为 NULL 
//	注意接收的内容不要超过接收缓冲区大小
//void L610_Http_Get(u16 bOffset,u16 bSize)
//{
//	u8 bL610_Cmd_List[2+1]={ Command12, Command11, CommandNull};
//	u8 bL610_Ack_List[2+1]={ Command9,  Command8,  CommandNull};
//	if(bSize)
//	{
//		bL610_Cmd_List[1]=Command11;
//		sprintf((char *)bL610_AT_Cmd_List[Command11],"AT+HTTPREAD=%d,%d",bOffset,bSize);
//	}
//	L610_Load_Cmd(3,bL610_Cmd_List,bL610_Ack_List);
//}

//HTTP POST 请求方法
//bCode				：发送的数据
//bLen				：发送的数据长度
//void L610_Http_Post(u8 *bCode,u8 bLen)
//{
//	u8 bL610_Cmd_List[4+1]={ Command9, Command14, Command10, Command11, CommandNull};
//	u8 bL610_Ack_List[4+1]={ Command6, Command1,  Command9,  Command8,  CommandNull};
//	sprintf((char *)bL610_AT_Cmd_List[Command9],"AT+HTTPDATA=%d",bLen);
//	strcpy((char *)bL610_AT_Cmd_List[Command14],(const char *)bCode);
//	L610_Load_Cmd(5,bL610_Cmd_List,bL610_Ack_List);
//}

void L610_qTcp(void)
{
	u8 bL610_Cmd_List[2+1]={ Command16, CommandNull ,CommandNull};
	u8 bL610_Ack_List[2+1]={ Command1,  CommandNull ,CommandNull};
	L610_Load_Cmd(1+1,bL610_Cmd_List,bL610_Ack_List);
}

void L610_Tcp_Set(u8 *bUrl,u8 *bPort)
{
	u8 bL610_Cmd_List[2+1]={ Command16, Command17, CommandNull};
	u8 bL610_Ack_List[2+1]={ Command1,  Command10, CommandNull};
//	if(bUrl!=NULL)
//		sprintf((char *)bL610_AT_Cmd_List[Command17],"AT+MIPOPEN=1,,\"%s\",%s,0",bUrl,bPort);
	L610_Load_Cmd(2+1,bL610_Cmd_List,bL610_Ack_List);
}


//TCP 发送方法
//bCode				：发送的数据
//bLen				：发送的数据长度
void L610_Tcp_Send(u8 *bCode,u16 wLen)
{
	u16 i;
	u8 bL610_Cmd_List[2+1]={ 41,        40,        CommandNull};
	u8 bL610_Ack_List[2+1]={ Command6,  Command11, CommandNull};

	sprintf((char *)bL610_ATSend_Data,"AT+MIPSEND=1,%d",wLen);
	for(i=0;i<wLen;i++)
		bL610_Send_Data[i]=bCode[i];
	wSend_Data_Len=wLen;
	L610_Load_Cmd(2+1,bL610_Cmd_List,bL610_Ack_List);
}


/////////////////////////////// L610 扫描函数 //////////////////////////////////////////////


void L610_Scan_Send(void)
{
	if(!L610_Send_Struct.bSending_Flag)
	{
		if(L610_Send_Struct.bSend_Finish)
		{
			if(L610_Send_Struct.bFinish_state)
			{
				L610_Send_Struct.bNow_Task++;
				L610_Send_Struct.bResend_Count=3;
			}
			else
			{
				if(L610_Send_Struct.bTask_Cmd_List[L610_Send_Struct.bNow_Task]==40)
					L610_Send_Struct.bNow_Task--;
				//从这里返回上一步
				L610_Send_Struct.bResend_Count--;
				delay_ms(1000);
			}
			L610_Send_Struct.bResponse_Flag=0;
			L610_Send_Struct.bSend_Finish=0;
			L610_Send_Struct.bRestart_Check=0;
			L610_Fram_Record_Struct.wInfAll=0;
		}
		else if(L610_Send_Struct.bTask_Cmd_List[L610_Send_Struct.bNow_Task]!=0)
		{
			if(L610_Send_Struct.bResend_Count==0)
			{
				bRestart_flag=1;
			}
			else
			{
				L610_Send_Cmd(0);
				bSending_Flag=1;
			}
		}
		else if(L610_Send_Struct.bTask_Cmd_List[L610_Send_Struct.bNow_Task]==0)
		{
			bSending_Flag=0;
		}
	}
}

u8 L610_Scan(void)
{
	u8 *bData=0,bGetIP_Flag=0,bSocketID=0,bRes=0,bCsq=0;
	u16 i,wLen=0;
	if(L610_Fram_Record_Struct.wInfBit.wFramFinishFlag)
	{
		if(L610_Send_Struct.bRestart_Check)
			L610_Send_Struct.bRestart_Check=0;
		L610_Fram_Record_Struct.bData_RX_BUF[L610_Fram_Record_Struct.wInfBit.wFramLength ] = '\0';
		if(L610_Send_Struct.bResponse_Flag==1)
		{
			if(strstr((const char *)bL610_AT_Ack_List[L610_Send_Struct.bTask_Ack_List[L610_Send_Struct.bNow_Task]],">"))
				bSend_Data_flag=1;
			else if(strstr((const char *)bL610_AT_Ack_List[L610_Send_Struct.bTask_Ack_List[L610_Send_Struct.bNow_Task]],"+MIPCALL:"))
				L610_Send_Struct.wRestartCheck_Time=cCommonCheck_Time;
			bData=bL610_Check_Data((u8 *)bL610_AT_Ack_List[L610_Send_Struct.bTask_Ack_List[L610_Send_Struct.bNow_Task]]);
			if(bData)
			{
				L610_Send_Struct.bFinish_state=1;
				L610_Send_Struct.bResponse_Flag=0;
			}
			else
			{
				L610_Send_Struct.bFinish_state=0;
			}
			L610_Send_Struct.bSend_Finish=1;
			L610_Send_Struct.bSending_Flag=0;
		}
		else if(L610_Send_Struct.bResponse_Flag==2)
		{
			bCsq=atoi(strstr(strstr((const char*)L610_Fram_Record_Struct.bData_RX_BUF,"+CSQ:"),":")+1);
			VTXprintf("L610 Rec:%s\r\n", L610_Fram_Record_Struct.bData_RX_BUF);
			if(bCsq<=31)
			{
				if(PLATFORM.bCsq!=bCsq)
				{
					PLATFORM.bCsq=bCsq;
					bRes=0xA1;
				}
				L610_Send_Struct.bFinish_state=1;
			}
			else
			{
				L610_Send_Struct.bFinish_state=0;
			}
			L610_Send_Struct.bSend_Finish=1;
			L610_Send_Struct.bSending_Flag=0;
		}
		else if(L610_Send_Struct.bResponse_Flag==3)
		{
			bData=bL610_Check_Data((u8 *)bL610_AT_Ack_List[8]);
			if(bData)
			{
				strcpy((char *)bL610_Data,(const char *)L610_Fram_Record_Struct.bData_RX_BUF);
				VTXprintf("Data=\r\n%s\r\n",bL610_Data);
			}
			L610_Send_Struct.bFinish_state=1;
			L610_Send_Struct.bSend_Finish=1;
			L610_Send_Struct.bSending_Flag=0;
		}
		else if(L610_Send_Struct.bResponse_Flag==4)
		{
			bData=bL610_Check_Data((u8 *)bL610_AT_Ack_List[1]);
			if(bData)
			{
				L610_Send_Struct.bResponse_Flag=1;
			}
		}
		else if(L610_Send_Struct.bResponse_Flag==5)
		{
			bGetIP_Flag=atoi(strstr(strstr((const char*)L610_Fram_Record_Struct.bData_RX_BUF,"+MIPCALL:"),":")+1);
			VTXprintf("L610 Rec:%s\r\n", L610_Fram_Record_Struct.bData_RX_BUF);
			if(bGetIP_Flag)
			{
				L610_Send_Struct.bNow_Task++;
			}
			L610_Send_Struct.wRestartCheck_Time=cSpecialChenk_Time;
			L610_Send_Struct.bFinish_state=1;
			L610_Send_Struct.bSend_Finish=1;
			L610_Send_Struct.bSending_Flag=0;
		}
		else if(L610_Send_Struct.bResponse_Flag==6)
		{
			bSocketID=atoi(strstr(strstr((const char*)L610_Fram_Record_Struct.bData_RX_BUF,"+MIPOPEN:"),":")+1);
			VTXprintf("L610 Rec:%s\r\n", L610_Fram_Record_Struct.bData_RX_BUF);
			if(bSocketID!=1)
			{
				LOTTERY.bL610_Online=1;
				L610_Send_Struct.bNow_Task++;
			}
			else
			{
				LOTTERY.bL610_Online=0;
			}
			bRes=0xA2;
			L610_Send_Struct.bFinish_state=1;
			L610_Send_Struct.bSend_Finish=1;
			L610_Send_Struct.bSending_Flag=0;
		}
		else if(L610_Send_Struct.bResponse_Flag==7)
		{
			u8 i;
			bData=bL610_Check_Data((u8 *)bL610_AT_Ack_List[13]);
			if(bData)
			{
				for(i=0;i<15;i++)
					PLATFORM.bIMEI[i]=L610_Fram_Record_Struct.bData_RX_BUF[i+9];
				VTXprintf("IMEI : %s\r\n",PLATFORM.bIMEI);
				bRes=0xA3;
			}
			L610_Send_Struct.bFinish_state=1;
			L610_Send_Struct.bSend_Finish=1;
			L610_Send_Struct.bSending_Flag=0;
		}
		else
		{
//			VTXprintf("L610 Rec:");
//			for(i=0;i<L610_Fram_Record_Struct.wInfBit.wFramLength;i++)
//				VTXprintf("%02x ",L610_Fram_Record_Struct.bData_RX_BUF[i]);
//			VTXprintf("\r\n");
			bData=bL610_Check_Data((u8 *)"+MIPRTCP:");
			if(bData)
			{
				wLen=atoi(strstr(strstr((const char*)L610_Fram_Record_Struct.bData_RX_BUF,"+MIPRTCP: 1,"),",")+1);
				for(i=0;i<wLen;i++)
				{
					bL610_Data[i]=L610_Fram_Record_Struct.bData_RX_BUF[i+17];
				}
				if(bL610_Data[0]==PLATFORM.bStart_Code&&bL610_Data[wLen-1]==PLATFORM.bEnd_Code)
				{
					PLATFORM_REC.wLength=bL610_Data[2]*256+bL610_Data[3];
					PLATFORM_REC.wChecksum=bL610_Data[5]*256+bL610_Data[6];
					bL610_Data[5]=0;bL610_Data[6]=0;
					////校验
					if(CalcCrc16(bL610_Data,PLATFORM_REC.wLength+10)==PLATFORM_REC.wChecksum)
					{
						PLATFORM_REC.bMode=bL610_Data[1];
						PLATFORM_REC.bFlag=bL610_Data[4];
						PLATFORM_REC.bSer_num=bL610_Data[7];
						for(i=0;i<PLATFORM_REC.wLength;i++)
							PLATFORM_REC.bBody[i]=bL610_Data[i+9];
						switch(PLATFORM_REC.bMode)
						{
							case 0x02:
							{
								VTXprintf("L610 Rec 0x02 ,Login Reply.\r\n");
								if(PLATFORM_REC.bBody[0]==0)
								{
									PLATFORM.bActice_State=1;
									for(i=0;i<8;i++)
										PLATFORM.bActice_Code[i]=PLATFORM_REC.bBody[i+1];
									for(i= 0 ;i<4 ;i++)
										PLATFORM.bCheckTime[i]=PLATFORM_REC.bBody[i+9];
									for(i= 0 ;i<18 ;i++)
										PLATFORM.bBlue[i]=PLATFORM_REC.bBody[i+13];
									VTXprintf("Login Success.\r\n");
									bRes=0x02;
								}
								break;
							}
							case 0x04:
							{
								VTXprintf("L610 Rec 0x04 ,Login Check.\r\n");
								LOTTERY.bHeart_Sign=1;
								break;
							}
							case 0x11:
							{
								u8 bCopare=0;
								for(i=0;i<4;i++)
									PLATFORM.bOrder_time[i]=PLATFORM_REC.bBody[i];
								PLATFORM.dwUnix_Time=(PLATFORM.bOrder_time[0]<<24) + (PLATFORM.bOrder_time[1]<<16) + (PLATFORM.bOrder_time[2]<<8) + PLATFORM.bOrder_time[3];
								for(i=0;i<16;i++)
									PLATFORM.bDevice_ID[i]=PLATFORM_REC.bBody[i+4];
								for(i=0;i<9;i++)
									PLATFORM.bOrder_ID[i]=PLATFORM_REC.bBody[i+20];
								PLATFORM.dwOrder_num=(PLATFORM_REC.bBody[29]<<24) + (PLATFORM_REC.bBody[30]<<16) + (PLATFORM_REC.bBody[31]<<8) + PLATFORM_REC.bBody[32];
								VTXprintf("L610 Rec 0x11 ,Order Num = %d\r\n",PLATFORM.dwOrder_num);
								for(i=0;i<9;i++)
								{
									if(PLATFORM.bOrder_ID[i]==PLATFORM.bOrder_ID_Bkp[i])
										bCopare++;
								}
								if(bCopare<9)    //订单号不相同
								{
									for(i=0;i<9;i++)
										PLATFORM.bOrder_ID_Bkp[i]=PLATFORM.bOrder_ID[i];
									//写入 Flash
									W25QXX_Write(PLATFORM.bOrder_ID_Bkp,cFLASH_ORDER_ADDRESS,9);
									VTXprintf("Rec One New Order. \r\n");
									bRes=0x11;
								}
								else
								{
									VTXprintf("Rec Duplicate Order. \r\n");
								}
								break;
							}
							case 0x13:
							{
								VTXprintf("L610 Rec 0x13 ,Order State = %02X\r\n",PLATFORM_REC.bBody[9]);
								bRes=0x13;
								break;
							}
							case 0x50:
							{
								VTXprintf("L610 Rec 0x50 .\r\n");
								bRes=0x50;
								break;
							}
							case 0x65:
							{
								VTXprintf("L610 Rec 0x65 .\r\n");
								bRes=0x65;
								break;
							}
							case 0x77:
							{
								VTXprintf("L610 Rec 0x77 .\r\n");
								bRes=0x77;
								break;
							}
							case 0x81:
							{
								VTXprintf("L610 Rec 0x81 .\r\n");
								bRes=0x81;
								break;
							}
							case 0x83:
							{
								VTXprintf("L610 Rec 0x83 .\r\n");
								bRes=0x83;
								break;
							}
							case 0x90:
							{
								VTXprintf("L610 Rec 0x90 .\r\n");
								bRes=0x90;
								break;
							}
							default:
								break;
						}
					}
				}
			}
			else
				bRes=0;
		}
		L610_Fram_Record_Struct.wInfAll=0;
	}
	else if(L610_Send_Struct.bSending_Flag)
	{
		if(!L610_Send_Struct.bRestart_Check&&!bRestart_flag)
		{
			L610_Send_Struct.bRestart_Check=1;
			wL610_Restart_Check_CNT=0;
		}
	}
	return bRes;
}


static void L610_Send_Cmd(u16 wLen)
{
	u8 i;
	i=L610_Send_Struct.bTask_Cmd_List[L610_Send_Struct.bNow_Task];
	if(i<40)
	{
		L610_Send_AT_Cmd((u8 *)bL610_AT_Cmd_List1[i],wLen);
	}
	else if(i==40)
	{
		i=14;
		L610_Send_AT_Cmd(bL610_Send_Data,wLen);
	}
	else if(i==41)
	{
		i=18;
		L610_Send_AT_Cmd(bL610_ATSend_Data,wLen);
	}
	L610_Send_Struct.bSending_Flag=1;
	if(strstr((const char *)bL610_AT_Cmd_List1[i],"AT+CSQ"))
		L610_Send_Struct.bResponse_Flag=2;
	else if(strstr((const char *)bL610_AT_Cmd_List1[i],"AT+HTTPREAD"))
		L610_Send_Struct.bResponse_Flag=3;
	else if(strstr((const char *)bL610_AT_Cmd_List1[i],"AT+HTTPACT=")||strstr((const char *)bL610_AT_Cmd_List1[i],"AT+MIPOPEN=1"))
		L610_Send_Struct.bResponse_Flag=4;
	else if(strstr((const char *)bL610_AT_Cmd_List1[i],"AT+MIPCALL?"))
		L610_Send_Struct.bResponse_Flag=5;
	else if(strstr((const char *)bL610_AT_Cmd_List1[i],"AT+MIPOPEN?"))
		L610_Send_Struct.bResponse_Flag=6;
	else if(strstr((const char *)bL610_AT_Cmd_List1[i],"AT+GSN?"))
		L610_Send_Struct.bResponse_Flag=7;
	else
		L610_Send_Struct.bResponse_Flag=1;
}

void L610_Scan_Restart(void)
{
	u8 i;
	if(bRestart_flag)
	{
		VTXprintf("\r\n正在重启\r\n");
		L610_Send_Struct.bFinish_state=0;
		L610_Send_Struct.bResend_Count=3;
		L610_Send_Struct.bResend_Flag=0;
		L610_Send_Struct.bResponse_Flag=0;
		L610_Send_Struct.bRestart_Check=0;
		L610_Send_Struct.bSending_Flag=0;
		L610_Send_Struct.bSend_Finish=0;
		for(i=0;i<20;i++)
		{
			L610_Send_Struct.bTask_Cmd_List[i]=0;
			L610_Send_Struct.bTask_Ack_List[i]=0;
		}
		bRestart_flag=0;
		LOTTERY.bRestart_flag=1;
	}
}


/////////////////////////////// USART2 串口初始化 //////////////////////////////////////////////

//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
u8 bL610_USART_RX_CHECK=0;
u8 bL610_USART_RX_CNT=0;
void L610_USART_IRQHandler(void)
{
	u8 res;

	if(USART_GetITStatus(cL610_USART, USART_IT_RXNE) != RESET)//接收到数据
	{
		res =USART_ReceiveData(cL610_USART);		
		if(L610_Fram_Record_Struct.wInfBit.wFramLength < cRX_BUF_MAX_LEN - 1 )		//还可以接收数据
		{
			bL610_USART_RX_CNT=0;
			if(L610_Fram_Record_Struct.wInfAll==0)bL610_USART_RX_CHECK=1;
			L610_Fram_Record_Struct.bData_RX_BUF[L610_Fram_Record_Struct.wInfBit.wFramLength++]=res;		//记录接收到的值	 
		}else 
		{
			L610_Fram_Record_Struct.wInfBit.wFramFinishFlag=1;					//强制标记接收完成
		}
	}

}

void L610_USART_SCAN(void)
{
	if(bL610_USART_RX_CHECK)
	{
		bL610_USART_RX_CNT++;
		if(bL610_USART_RX_CNT==10)
		{
			L610_Fram_Record_Struct.wInfBit.wFramFinishFlag = 1;	//标记接收完成
			bL610_USART_RX_CNT=0;
			bL610_USART_RX_CHECK=0;	
		}
	}
}

void L610_Restart_Check_Scan(void)
{
	if(L610_Send_Struct.bRestart_Check)
	{
		wL610_Restart_Check_CNT++;
		if(wL610_Restart_Check_CNT==L610_Send_Struct.wRestartCheck_Time)
		{
			bRestart_flag=1;
			wL610_Restart_Check_CNT=0;
			L610_Send_Struct.bRestart_Check=0;
		}
	}
}


/////////////////////////////// USART2 串口发送 //////////////////////////////////////////////

//串口,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
u8 bL610_Data_TX_BUF[200]={0};
void L610_printf(char* fmt,...)
{
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char *)bL610_Data_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)bL610_Data_TX_BUF);		//此次发送数据的长度
	VTXprintf("L610 Send:%s\r\n",bL610_Data_TX_BUF);
	for(j=0;j<i;j++)							//循环发送数据
	{
	  while(USART_GetFlagStatus(cL610_USART,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
	  USART_SendData(cL610_USART,bL610_Data_TX_BUF[j]); 
	}
}

void L610_Usart_Send(u8 *bData,u16 bLen)
{
	u16 i;
	VTXprintf("L610 Send:\r\n");
	for(i=0;i<bLen;i++)
	{
		USART_SendData(cL610_USART,bData[i]);
		while(USART_GetFlagStatus(cL610_USART,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕
		VTXprintf("%02x",bData[i]);
	}
	VTXprintf("\r\n");
}

u16 CalcCrc16 ( u8 *pdata, int len)
{
	u16 crc=0xFFFF;
	int i, j;
	for ( j=0; j<len;j++)
	{
		crc=crc^pdata[j];
		for ( i=0; i<8; i++)
		{
			if( ( crc&0x0001) >0)
			{
				crc=crc>>1;
				crc=crc^ 0xa001;
			}
			else
			crc=crc>>1;
		}
	}
	return crc;
}

void SoftReset(void)
{
	__set_FAULTMASK(ENABLE);
	NVIC_SystemReset();
}
