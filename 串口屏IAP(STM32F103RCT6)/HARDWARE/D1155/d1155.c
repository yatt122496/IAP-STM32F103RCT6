#include "d1155.h"

//������ͨѶ����
//data				���ַ������飬�� mode=0 ʱ��Ч��Ϊ��Ҫ��ʾ������
//													mode ��Ϊ 0 ʱ����������Ч
//len					���ַ��������ֽ������� mode = 0 ʱ��Ч
//str					��������ģʽ��Ϊ 0 ʱ��ʾ�ַ���
//												 Ϊ 1 ʱ�����ٶȣ�mode Ϊ�ٶȵ�ֵ
//												 Ϊ 2 ʱ�������ȣ�mode Ϊ����ֵ
//												 Ϊ 3 ʱ��Ϊ��ʾ�ڲ����֣�mode Ϊ���ֵı�� �� 0-17 
//												 Ϊ 4 ʱ��ʾ�ڲ���ͨ���֣�mode Ϊ���ֵı�� �� 0-3
//mode				���� str Ϊ 0 ʱ��Ч��
//								 str ��Ϊ 0 ʱ��mode Ϊ��ͬ��ģʽ��ȡֵ


//��ʾ������Ϣʱ��mode ֵ��Ӧ������Ϣ���£�
// 0 �հ�
// 1 ��ӭ�����й�������Ʊ���۳���΢��ɨ���Ʊ���ϵĶ�ά�뼴�ɹ����й�������Ʊ
// 2 ɨ����Ը��ֻ����
// 3 ɨ�빺��
// 4 ɨ����
// 5 ����ɨ�豾��Ʊ���ϵĶ�ά����г��
// 6 ���ã�����ǰ�����й�������Ʊ�������ۻ�������������ѡ���й�������Ʊ��������ɺ���������
// 7 ͼ��
// 8 �ܱ�Ǹ����Ʊ������
// 9 ����֧����ɣ������ĵȴ�
// 10 �������ӳ�ʱ�����˿������1���Ӻ��ٴγ��ԡ�������������С����ҳ����ϵ�ۺ����
// 11 �뽫��ɫˮ��������β������������
// 12 ͼ��
// 13 �ܱ�Ǹ����治�㣬ʣ��Ʊ�����20�������˻�������֧���˻���������������С����ҳ����ϵ�ۺ����
// 14 �ο��н��������ĺ��뱣���㣬���ϱ��й�Ƭ����ʹ�ù�Ƭ�ο����н���������ĺ���һ�¼ȵ��·��н����
// 15 ͼ��
// 16 ��ϲ���н���������������й���ʣ�··����
// 17 ͼ��


// ������ͨ�������£�
// 0 �հ�
// 1 �й�������Ʊָ�����пռ���������     ··��
// 2 ɨ����Ը��ֻ����ܿ��ٳ��
// 3 ����ɨ�뱾��Ʊ����ά��������ܿ��ٳ��
void D1155_Send_Player(u8 mode,u8 str)
{
	u8 Data[][150] ={
		"�й�������Ʊָ�����пռ���������     ··��     ���ز�Ʊ���ۻ�",
		"���н�����ο����½ǵı����㣬ʹ�ù���ʱ��С����ɨ�豣�����µĶ�ά�룬����ʱ���ˣ�������������С����ҳ����ϵ�ۺ�������ѯ˾��ʦ��",
		"���ڳ�Ʊ��",
		"�뼰ʱȡƱ",
	};         //����ʵʱ��ʾ�ַ������飬����Ҫ����������������ȱ��
	u8 send_data1[150]={0},temp=0,bCmd[2],len,bCmd_flag=0,bData_num;
	u32 i;
	send_data1[0]='w';
	send_data1[1]='a';
	send_data1[2]='n';
	send_data1[3]='g';
	if(mode>2)
		mode=0;
	if(mode==0)//ʵʱ��Ϣ
	{
		if(str<1||str>7)
			str=1;
		switch(str)
		{
			case 1:
				send_data1[4]=0x70;
				bData_num=0;
				len=strlen((const char *)Data[bData_num]);
				bCmd_flag=0;
				break;
			case 2:
				send_data1[4]=0x73;
				bCmd[0]=0x73;
				bCmd[1]=6|0x80;
				len=2;
				bCmd_flag=1;
				break;
			case 3:
				send_data1[4]=0x73;
				bCmd[0]=0x73;
				bCmd[1]=13|0x80;
				len=2;
				bCmd_flag=1;
				break;
			case 4:
				send_data1[4]=0x73;
				bCmd[0]=0x73;
				bCmd[1]=14|0x80;
				len=2;
				bCmd_flag=1;
				break;
			case 5:
				send_data1[4]=0x70;
				bData_num=1;
				len=strlen((const char *)Data[bData_num]);
				bCmd_flag=0;
				break;
			case 6:
				send_data1[4]=0x70;
				bData_num=2;
				len=strlen((const char *)Data[bData_num]);
				bCmd_flag=0;
				break;
			case 7:
				send_data1[4]=0x70;
				bData_num=3;
				len=strlen((const char *)Data[bData_num]);
				bCmd_flag=0;
				break;
			default:
				break;
		}
	}
	else	if(mode==1)//�ٶ� 0-7 0����
	{
		send_data1[4]=0x71;
		bCmd[0]=0x71;
		if(str>7)
			str=3;
		bCmd[1]=str;
		len=2;
		bCmd_flag=1;
	}
	else	if(mode==2)//�л����� 0-3 0����
	{
		send_data1[4]=0x72;
		bCmd[0]=0x72;
		if(str>3)
			str=2;
		bCmd[1]=str;
		len=2;
		bCmd_flag=1;
	}
	send_data1[5]=7+len;
	for(i=0;i<len;i++)
	{
		if(bCmd_flag)
		{
			send_data1[7+i]=bCmd[i];
			temp=bCmd[i]+temp;
		}
		else
		{
			send_data1[7+i]=Data[bData_num][i];
			temp=Data[bData_num][i]+temp;
		}
	}
	send_data1[6]=temp+0x55;

	D1155_USART_Send(send_data1,7+len);
	
}

u8 *bD1155_Scan(void)
{
	u8 i;
	static u8 l=1,j=0,k=0;
	if(D1155_Fram_Record_Struct.wInfBit.wFramFinishFlag)
	{
		D1155_Fram_Record_Struct.bData_RX_BUF[D1155_Fram_Record_Struct.wInfBit.wFramLength]=0;
		printf("bData_RX_BUF=%s\r\n",D1155_Fram_Record_Struct.bData_RX_BUF);
		for(i=0;i<D1155_Fram_Record_Struct.wInfBit.wFramLength;i++)
			printf("bData_RX_BUF[%d]=%02x\r\n",i,D1155_Fram_Record_Struct.bData_RX_BUF[i]);
		
		if(strcmp((const char *)D1155_Fram_Record_Struct.bData_RX_BUF,"K0")==0)
		{
			D1155_Send_Player(MODE0,l);
			l++;
			if(l>7)
				l=1;
		}
		else if(strcmp((const char *)D1155_Fram_Record_Struct.bData_RX_BUF,"K1")==0)
		{
			D1155_Send_Player(MODE1,j);
			j++;
			if(j>7)
				j=0;
		}
		else if(strcmp((const char *)D1155_Fram_Record_Struct.bData_RX_BUF,"K2")==0)
		{
			D1155_Send_Player(MODE2,k);
			k++;
			if(k>3)
				k=0;
		}
		//�˴���Ӵ��������Ĵ������ݵĺ���
		D1155_Fram_Record_Struct.wInfAll=0;
		return D1155_Fram_Record_Struct.bData_RX_BUF;
	}
	return 0;
}
//////////////////////////////////// �������� ///////////////////////////////


struct D1155_STRUCT_USART_Fram D1155_Fram_Record_Struct = { 0 };		//N58���ڽ��սṹ��

//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
u8 bD1155_Usart_Rx_Check=0;
u8 bD1155_Usart_Rx_Cnt=0;
void D1155_Usart_IRQHandler(void)
{
	u8 res;	    
	if(USART_GetITStatus(cD1155_USART, USART_IT_RXNE) != RESET)//���յ�����
	{
		res =USART_ReceiveData(cD1155_USART);
		if(D1155_Fram_Record_Struct.wInfBit.wFramLength < cD1155_RX_BUF_MAX_LEN-1)		//�����Խ�������
		{
			bD1155_Usart_Rx_Cnt=0;
			if(D1155_Fram_Record_Struct.wInfAll==0)bD1155_Usart_Rx_Check=1;
			D1155_Fram_Record_Struct.bData_RX_BUF[D1155_Fram_Record_Struct.wInfBit.wFramLength++]=res;		//��¼���յ���ֵ
		}else
		{
			D1155_Fram_Record_Struct.wInfBit.wFramFinishFlag=1;					//ǿ�Ʊ�ǽ������
		}
	}
}

void D1155_Usart_Scan(void)
{
	if(bD1155_Usart_Rx_Check)
	{
		bD1155_Usart_Rx_Cnt++;
		if(bD1155_Usart_Rx_Cnt==10)
		{
			D1155_Fram_Record_Struct.wInfBit.wFramFinishFlag = 1;	//��ǽ������
			bD1155_Usart_Rx_Cnt=0;
			bD1155_Usart_Rx_Check=0;	
		}
	}
}


/////////////////////////////// USART2 ���ڷ��� //////////////////////////////////////////////

//����,printf ����
//ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
u8 bD1155_Data_TX_BUF[cD1155_RX_BUF_MAX_LEN];
void D1155_printf(char* fmt,...)
{
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char *)bD1155_Data_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)bD1155_Data_TX_BUF);		//�˴η������ݵĳ���
	for(j=0;j<i;j++)							//ѭ����������
	{
		USART_SendData(cD1155_USART,bD1155_Data_TX_BUF[j]);
	  while(USART_GetFlagStatus(cD1155_USART,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
	}
}

void D1155_USART_Send(u8 *bData,u16 wLen)
{
	u16 i;
	for(i=0;i<wLen;i++)
	{
		USART_SendData(cD1155_USART,bData[i]);
		while(USART_GetFlagStatus(cD1155_USART,USART_FLAG_TC)==RESET); //ѭ������,ֱ��������� 
	}
}
