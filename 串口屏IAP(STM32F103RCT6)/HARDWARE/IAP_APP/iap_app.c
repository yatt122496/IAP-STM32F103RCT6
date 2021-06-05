#include "iap_app.h"

void SoftReset(void)
{
	__set_FAULTMASK(ENABLE);
	NVIC_SystemReset();
}

static u8 buf[4096]={0};

u8 *bIAP_APP_Scan(void)
{
	u8 bSum=0,bNowPack=0,bAllPacks=0,bAPPSectors=0;
	u16 i;
	if(IAP_APP_Fram_Record_Struct.wInfBit.wFramFinishFlag)
	{
		IAP_APP_Fram_Record_Struct.bData_RX_BUF[IAP_APP_Fram_Record_Struct.wInfBit.wFramLength]=0;
//		IAP_APP_printf("%d\r\n",IAP_APP_Fram_Record_Struct.wInfBit.wFramLength);
//			IAP_APP_USART_Send(IAP_APP_Fram_Record_Struct.bData_RX_BUF,IAP_APP_Fram_Record_Struct.wInfBit.wFramLength);
		if(IAP_APP_Fram_Record_Struct.bData_RX_BUF[3]==0x93)
		{
			for(i=0;i<IAP_APP_Fram_Record_Struct.bData_RX_BUF[4]*256+IAP_APP_Fram_Record_Struct.bData_RX_BUF[5];i++)
			{
				bSum+=IAP_APP_Fram_Record_Struct.bData_RX_BUF[i+3];
			}
			if((bSum&0xFF)==IAP_APP_Fram_Record_Struct.bData_RX_BUF[i+3])
			{
				bNowPack=IAP_APP_Fram_Record_Struct.bData_RX_BUF[7];
				bAllPacks=IAP_APP_Fram_Record_Struct.bData_RX_BUF[6];
				bAPPSectors=bAllPacks/2+bAllPacks%2;
				for(i=0;i<2048;i++)
				{
					if(bNowPack%2!=0)
						buf[i]=IAP_APP_Fram_Record_Struct.bData_RX_BUF[8+i];
					else
						buf[2048+i]=IAP_APP_Fram_Record_Struct.bData_RX_BUF[8+i];
				}
				if(bNowPack%2==0)
				{
					W25QXX_Write(buf,cFIRMWAREADDR+(bNowPack/2-1)*4096,4096);
				}
				else if(bNowPack==bAllPacks)
				{
					W25QXX_Write(buf,cFIRMWAREADDR+(bNowPack/2)*4096,4096);
				}
				if(bNowPack==bAllPacks)
				{
					buf[0]=0;buf[1]=bAPPSectors;
					W25QXX_Write(buf,cUPDATAINFOADDR,2);		//从0开始写入4096个数据
					
					SoftReset();
				}
				IAP_APP_Reply();
			}
		}
		//此处添加处理接收完的串口数据的函数
		IAP_APP_Fram_Record_Struct.wInfAll=0;
		return IAP_APP_Fram_Record_Struct.bData_RX_BUF;
	}
	return 0;
}

void IAP_APP_Reply(void)
{
	u8 bCmd[7]={0},bSum=0,i;
	bCmd[0]=0xAA;
	bCmd[1]=0x55;
	bCmd[2]=0x03;
	bCmd[3]=0x81;
	bCmd[4]=0x93;
	bCmd[5]=0x01;
	for(i=0;i<bCmd[2];i++)
		bSum+=bCmd[3+i];
	bCmd[6]=bSum&0xFF;
	IAP_APP_USART_Send(bCmd,7);
}

//////////////////////////////////// 串口设置 ///////////////////////////////


struct IAP_APP_STRUCT_USART_Fram IAP_APP_Fram_Record_Struct = { 0 };		//N58串口接收结构体

//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
u8 bIAP_APP_Usart_Rx_Check=0;
u8 bIAP_APP_Usart_Rx_Cnt=0;
void IAP_APP_Usart_IRQHandler(void)
{
	u8 res;	    
	if(USART_GetITStatus(cIAP_APP_USART, USART_IT_RXNE) != RESET)//接收到数据
	{
		res =USART_ReceiveData(cIAP_APP_USART);
		if(IAP_APP_Fram_Record_Struct.wInfBit.wFramLength < cIAP_APP_RX_BUF_MAX_LEN-1)		//还可以接收数据
		{
			bIAP_APP_Usart_Rx_Cnt=0;
			if(IAP_APP_Fram_Record_Struct.wInfAll==0)bIAP_APP_Usart_Rx_Check=1;
			IAP_APP_Fram_Record_Struct.bData_RX_BUF[IAP_APP_Fram_Record_Struct.wInfBit.wFramLength++]=res;		//记录接收到的值
		}else
		{
			IAP_APP_Fram_Record_Struct.wInfBit.wFramFinishFlag=1;					//强制标记接收完成
		}
	}
}

void IAP_APP_Usart_Scan(void)
{
	if(bIAP_APP_Usart_Rx_Check)
	{
		bIAP_APP_Usart_Rx_Cnt++;
		if(bIAP_APP_Usart_Rx_Cnt==10)
		{
			IAP_APP_Fram_Record_Struct.wInfBit.wFramFinishFlag = 1;	//标记接收完成
			bIAP_APP_Usart_Rx_Cnt=0;
			bIAP_APP_Usart_Rx_Check=0;	
		}
	}
}


/////////////////////////////// USART2 串口发送 //////////////////////////////////////////////

//串口,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
u8 bIAP_APP_Data_TX_BUF[cIAP_APP_RX_BUF_MAX_LEN];
void IAP_APP_printf(char* fmt,...)
{
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char *)bIAP_APP_Data_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)bIAP_APP_Data_TX_BUF);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
		USART_SendData(cIAP_APP_USART,bIAP_APP_Data_TX_BUF[j]);
	  while(USART_GetFlagStatus(cIAP_APP_USART,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
	}
}

void IAP_APP_USART_Send(u8 *bData,u16 wLen)
{
	u16 i;
	for(i=0;i<wLen;i++)
	{
		USART_SendData(cIAP_APP_USART,bData[i]);
		while(USART_GetFlagStatus(cIAP_APP_USART,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕 
	}
}
