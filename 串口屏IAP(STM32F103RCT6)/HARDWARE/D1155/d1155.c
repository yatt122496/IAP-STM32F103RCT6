#include "d1155.h"

//串口屏通讯函数
//data				：字符串数组，当 mode=0 时有效，为需要显示的内容
//													mode 不为 0 时，该数据无效
//len					：字符串数组字节数，当 mode = 0 时有效
//str					：串口屏模式，为 0 时显示字符串
//												 为 1 时调整速度，mode 为速度的值
//												 为 2 时调整亮度，mode 为亮度值
//												 为 3 时改为显示内部文字，mode 为文字的标号 从 0-17 
//												 为 4 时显示内部普通文字，mode 为文字的标号 从 0-3
//mode				：当 str 为 0 时无效，
//								 str 不为 0 时，mode 为不同的模式的取值


//显示内置信息时，mode 值对应文字信息如下：
// 0 空白
// 1 欢迎乘坐中国体育彩票销售车，微信扫描彩票机上的二维码即可购买中国体育彩票
// 2 扫码可以给手机充电
// 3 扫码购彩
// 4 扫码充电
// 5 请您扫描本彩票机上的二维码进行充电
// 6 您好：您面前的是中国体育彩票自主销售机，现在您可以选购中国体育彩票，购买完成后立即开奖
// 7 图像
// 8 很抱歉，彩票已售完
// 9 您已支付完成，请耐心等待
// 10 网络链接超时，已退款，您可在1分钟后再次尝试。如有问题请在小程序页面联系售后服务
// 11 请将红色水晶拉杆向车尾方向用力拉出
// 12 图像
// 13 很抱歉，库存不足，剩余票款会在20分钟内退还到您的支付账户，如有问题请在小程序页面联系售后服务
// 14 刮开中奖号码和你的号码保护层，车上备有刮片，可使用刮片刮开。中奖号码与你的号码一致既得下方中奖金额
// 15 图像
// 16 恭喜您中奖，今天好运气。中国体彩，路路精彩
// 17 图像


// 内置普通文字如下：
// 0 空白
// 1 中国体育彩票指定出行空间销售渠道     路路彩
// 2 扫码可以给手机智能快速充电
// 3 请您扫码本彩票机二维码进行智能快速充电
void D1155_Send_Player(u8 mode,u8 str)
{
	u8 Data[][150] ={
		"中国体育彩票指定出行空间销售渠道     路路彩     车载彩票销售机",
		"如中奖，请刮开左下角的保护层，使用购买时的小程序扫描保护层下的二维码，奖金即时到账，如有问题请在小程序页面联系售后服务或咨询司机师傅",
		"正在出票中",
		"请及时取票",
	};         //定义实时显示字符串数组，，主要用来补充内置文字缺少
	u8 send_data1[150]={0},temp=0,bCmd[2],len,bCmd_flag=0,bData_num;
	u32 i;
	send_data1[0]='w';
	send_data1[1]='a';
	send_data1[2]='n';
	send_data1[3]='g';
	if(mode>2)
		mode=0;
	if(mode==0)//实时信息
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
	else	if(mode==1)//速度 0-7 0最慢
	{
		send_data1[4]=0x71;
		bCmd[0]=0x71;
		if(str>7)
			str=3;
		bCmd[1]=str;
		len=2;
		bCmd_flag=1;
	}
	else	if(mode==2)//切换亮度 0-3 0最亮
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
		//此处添加处理接收完的串口数据的函数
		D1155_Fram_Record_Struct.wInfAll=0;
		return D1155_Fram_Record_Struct.bData_RX_BUF;
	}
	return 0;
}
//////////////////////////////////// 串口设置 ///////////////////////////////


struct D1155_STRUCT_USART_Fram D1155_Fram_Record_Struct = { 0 };		//N58串口接收结构体

//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
u8 bD1155_Usart_Rx_Check=0;
u8 bD1155_Usart_Rx_Cnt=0;
void D1155_Usart_IRQHandler(void)
{
	u8 res;	    
	if(USART_GetITStatus(cD1155_USART, USART_IT_RXNE) != RESET)//接收到数据
	{
		res =USART_ReceiveData(cD1155_USART);
		if(D1155_Fram_Record_Struct.wInfBit.wFramLength < cD1155_RX_BUF_MAX_LEN-1)		//还可以接收数据
		{
			bD1155_Usart_Rx_Cnt=0;
			if(D1155_Fram_Record_Struct.wInfAll==0)bD1155_Usart_Rx_Check=1;
			D1155_Fram_Record_Struct.bData_RX_BUF[D1155_Fram_Record_Struct.wInfBit.wFramLength++]=res;		//记录接收到的值
		}else
		{
			D1155_Fram_Record_Struct.wInfBit.wFramFinishFlag=1;					//强制标记接收完成
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
			D1155_Fram_Record_Struct.wInfBit.wFramFinishFlag = 1;	//标记接收完成
			bD1155_Usart_Rx_Cnt=0;
			bD1155_Usart_Rx_Check=0;	
		}
	}
}


/////////////////////////////// USART2 串口发送 //////////////////////////////////////////////

//串口,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
u8 bD1155_Data_TX_BUF[cD1155_RX_BUF_MAX_LEN];
void D1155_printf(char* fmt,...)
{
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char *)bD1155_Data_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)bD1155_Data_TX_BUF);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
		USART_SendData(cD1155_USART,bD1155_Data_TX_BUF[j]);
	  while(USART_GetFlagStatus(cD1155_USART,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
	}
}

void D1155_USART_Send(u8 *bData,u16 wLen)
{
	u16 i;
	for(i=0;i<wLen;i++)
	{
		USART_SendData(cD1155_USART,bData[i]);
		while(USART_GetFlagStatus(cD1155_USART,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕 
	}
}
