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
		
		//�˴���Ӵ��������Ĵ������ݵĺ���
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
//			while(USART_GetFlagStatus(cBLE_USART,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
//			USART_SendData(cBLE_USART, bSend_buf[i]);//�򴮿�1��������		
//	}
	BLE_printf("AT+RST\r\n");
}



//////////////////////////////////// �������� ///////////////////////////////


struct BLE_STRUCT_USART_Fram BLE_Fram_Record_Struct = { 0 };		//N58���ڽ��սṹ��

//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
u8 bBLE_USART_RX_CHECK=0;
u8 bBLE_USART_RX_CNT=0;
void BLE_Usart_IRQHandler(void)
{
	u8 res;	    
	if(USART_GetITStatus(cBLE_USART, USART_IT_RXNE) != RESET)//���յ�����
	{
		res =USART_ReceiveData(cBLE_USART);
		if(BLE_Fram_Record_Struct.wInfBit.wFramLength < cBLE_RX_BUF_MAX_LEN-1)		//�����Խ�������
		{
			bBLE_USART_RX_CNT=0;
			if(BLE_Fram_Record_Struct.wInfAll==0)bBLE_USART_RX_CHECK=1;
			BLE_Fram_Record_Struct.bData_RX_BUF[BLE_Fram_Record_Struct.wInfBit.wFramLength++]=res;		//��¼���յ���ֵ
		}else
		{
			BLE_Fram_Record_Struct.wInfBit.wFramFinishFlag=1;					//ǿ�Ʊ�ǽ������
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
			BLE_Fram_Record_Struct.wInfBit.wFramFinishFlag = 1;	//��ǽ������
			bBLE_USART_RX_CNT=0;
			bBLE_USART_RX_CHECK=0;	
		}
	}
}


/////////////////////////////// USART2 ���ڷ��� //////////////////////////////////////////////
u8 bBLE_Data_TX_BUF[cBLE_RX_BUF_MAX_LEN]={0};
void BLE_printf(char* fmt,...)
{
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char *)bBLE_Data_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)bBLE_Data_TX_BUF);		//�˴η������ݵĳ���
	for(j=0;j<i;j++)							//ѭ����������
	{
	  while(USART_GetFlagStatus(cBLE_USART,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
	  USART_SendData(cBLE_USART,bBLE_Data_TX_BUF[j]);
	}
}

void BLE_Usart_Send(u8 *bData,u16 wLen)
{
	u16 i;
	for(i=0;i<wLen;i++)
	{
		USART_SendData(cBLE_USART,bData[i]);
		while(USART_GetFlagStatus(cBLE_USART,USART_FLAG_TC)==RESET); //ѭ������,ֱ��������� 
//		printf("%c",bData[i]);
	}
}
