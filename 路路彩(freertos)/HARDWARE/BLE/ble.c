#include "ble.h"

u8 *bBLE_Scan(void)
{
	u8 i;
	if(BLE_Fram_Record_Struct.wInfBit.wFramFinishFlag)
	{
		BLE_Fram_Record_Struct.bData_RX_BUF[BLE_Fram_Record_Struct.wInfBit.wFramLength]=0;
//		printf("bData_RX_BUF=%s\r\n",BLE_Fram_Record_Struct.bData_RX_BUF);
//		for(i=0;i<BLE_Fram_Record_Struct.wInfBit.wFramLength;i++)
//			printf("bData_RX_BUF[%d]=%02x\r\n",i,BLE_Fram_Record_Struct.bData_RX_BUF[i]);
		
		//此处添加处理接收完的串口数据的函数
		BLE_Fram_Record_Struct.wInfAll=0;
		return BLE_Fram_Record_Struct.bData_RX_BUF;
	}
	return 0;
}

void BLE_Change_Name(u8 *bStr_data)
{
	BLE_printf("AT+NAME=%s\r\n",bStr_data);
}

void BLE_Restart(void)
{
//	u8 i,bSend_buf[25];

//	bSend_buf[0] = 0xFA;
//	bSend_buf[1] = 0x08;
//	bSend_buf[2] = 0;
//	bSend_buf[3] = 0xAA;
//	for(i=0;i<4;i++)
//	{
//			while(USART_GetFlagStatus(cBLE_USART,USART_FLAG_TC)!=SET);//等待发送结束
//			USART_SendData(cBLE_USART, bSend_buf[i]);//向串口1发送数据		
//	}
	BLE_printf("AT+RST\r\n");
}



//////////////////////////////////// 串口设置 ///////////////////////////////


struct BLE_STRUCT_USART_Fram BLE_Fram_Record_Struct = { 0 };		//N58串口接收结构体

//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
u8 bBLE_USART_RX_CHECK=0;
u8 bBLE_USART_RX_CNT=0;
void BLE_Usart_IRQHandler(void)
{
	u8 res;	    
	if(USART_GetITStatus(cBLE_USART, USART_IT_RXNE) != RESET)//接收到数据
	{
		res =USART_ReceiveData(cBLE_USART);
		if(BLE_Fram_Record_Struct.wInfBit.wFramLength < cBLE_RX_BUF_MAX_LEN-1)		//还可以接收数据
		{
			bBLE_USART_RX_CNT=0;
			if(BLE_Fram_Record_Struct.wInfAll==0)bBLE_USART_RX_CHECK=1;
			BLE_Fram_Record_Struct.bData_RX_BUF[BLE_Fram_Record_Struct.wInfBit.wFramLength++]=res;		//记录接收到的值
		}else
		{
			BLE_Fram_Record_Struct.wInfBit.wFramFinishFlag=1;					//强制标记接收完成
		}
	}
}

void BLE_Usart_Scan(void)
{
	if(bBLE_USART_RX_CHECK)
	{
		bBLE_USART_RX_CNT++;
		if(bBLE_USART_RX_CNT==10)
		{
			BLE_Fram_Record_Struct.wInfBit.wFramFinishFlag = 1;	//标记接收完成
			bBLE_USART_RX_CNT=0;
			bBLE_USART_RX_CHECK=0;	
		}
	}
}


/////////////////////////////// USART2 串口发送 //////////////////////////////////////////////
u8 bBLE_Data_TX_BUF[cBLE_RX_BUF_MAX_LEN]={0};
void BLE_printf(char* fmt,...)
{
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char *)bBLE_Data_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)bBLE_Data_TX_BUF);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
	  while(USART_GetFlagStatus(cBLE_USART,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
	  USART_SendData(cBLE_USART,bBLE_Data_TX_BUF[j]);
	}
}

void BLE_Usart_Send(u8 *bData,u16 wLen)
{
	u16 i;
	for(i=0;i<wLen;i++)
	{
		USART_SendData(cBLE_USART,bData[i]);
		while(USART_GetFlagStatus(cBLE_USART,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕 
//		printf("%c",bData[i]);
	}
}
