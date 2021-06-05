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
					W25QXX_Write(buf,cUPDATAINFOADDR,2);		//��0��ʼд��4096������
					
					SoftReset();
				}
				IAP_APP_Reply();
			}
		}
		//�˴���Ӵ��������Ĵ������ݵĺ���
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

//////////////////////////////////// �������� ///////////////////////////////


struct IAP_APP_STRUCT_USART_Fram IAP_APP_Fram_Record_Struct = { 0 };		//N58���ڽ��սṹ��

//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
u8 bIAP_APP_Usart_Rx_Check=0;
u8 bIAP_APP_Usart_Rx_Cnt=0;
void IAP_APP_Usart_IRQHandler(void)
{
	u8 res;	    
	if(USART_GetITStatus(cIAP_APP_USART, USART_IT_RXNE) != RESET)//���յ�����
	{
		res =USART_ReceiveData(cIAP_APP_USART);
		if(IAP_APP_Fram_Record_Struct.wInfBit.wFramLength < cIAP_APP_RX_BUF_MAX_LEN-1)		//�����Խ�������
		{
			bIAP_APP_Usart_Rx_Cnt=0;
			if(IAP_APP_Fram_Record_Struct.wInfAll==0)bIAP_APP_Usart_Rx_Check=1;
			IAP_APP_Fram_Record_Struct.bData_RX_BUF[IAP_APP_Fram_Record_Struct.wInfBit.wFramLength++]=res;		//��¼���յ���ֵ
		}else
		{
			IAP_APP_Fram_Record_Struct.wInfBit.wFramFinishFlag=1;					//ǿ�Ʊ�ǽ������
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
			IAP_APP_Fram_Record_Struct.wInfBit.wFramFinishFlag = 1;	//��ǽ������
			bIAP_APP_Usart_Rx_Cnt=0;
			bIAP_APP_Usart_Rx_Check=0;	
		}
	}
}


/////////////////////////////// USART2 ���ڷ��� //////////////////////////////////////////////

//����,printf ����
//ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
u8 bIAP_APP_Data_TX_BUF[cIAP_APP_RX_BUF_MAX_LEN];
void IAP_APP_printf(char* fmt,...)
{
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char *)bIAP_APP_Data_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)bIAP_APP_Data_TX_BUF);		//�˴η������ݵĳ���
	for(j=0;j<i;j++)							//ѭ����������
	{
		USART_SendData(cIAP_APP_USART,bIAP_APP_Data_TX_BUF[j]);
	  while(USART_GetFlagStatus(cIAP_APP_USART,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
	}
}

void IAP_APP_USART_Send(u8 *bData,u16 wLen)
{
	u16 i;
	for(i=0;i<wLen;i++)
	{
		USART_SendData(cIAP_APP_USART,bData[i]);
		while(USART_GetFlagStatus(cIAP_APP_USART,USART_FLAG_TC)==RESET); //ѭ������,ֱ��������� 
	}
}
