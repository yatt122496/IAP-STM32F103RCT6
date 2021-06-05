#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "stmflash.h"
#include "iap.h"
#include "w25qxx.h"
#include <string.h>
#include "key.h"

#if CheckBound==1
	#include "usmart.h"
#endif

#define UPDATAINFOADDR	1024*1024*6
#define FIRMWAREADDR		(UPDATAINFOADDR+4096)

u8 buf[4096]={0};

int main(void)
{
	u16 i,t=0,wLen;				//接收到的app代码长度
	u8 bAPPSectors=0,key=0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(38400);	//串口初始化为115200
	delay_init();	   	 	//延时初始化
	W25QXX_Init();
 	LED_Init();		  			//初始化与LED连接的硬件接口
	KEY_Init();
#if CheckBound==1
	usmart_dev.init(SystemCoreClock/1000000);	//初始化USMART
#endif
	VTXprintf("Runing Boot Program.\r\n");
	VTXprintf("Wait 3 second ...");
	VTXprintf("Jump to user App...\r\n");

	while(1)
	{
		key=KEY_Scan(0);
		t++;
		delay_ms(10);
		if(t%30==0)
		{
			VTXprintf("second:%d\r\n",t/100);
			LED2=!LED2;
			VTXprintf("Wait 3s Jump User Program.\r\n");
		}
		if(t==300)
		{
			W25QXX_Read(buf,UPDATAINFOADDR,2);//读出整个扇区的内容
			bAPPSectors=buf[1];
			if(buf[0]==0)
			{
				W25QXX_Read(buf,FIRMWAREADDR,8);
				if(((*(vu32*)(buf+4))&0xFF000000)==0x08000000)
				{
					VTXprintf("Start Writing Internal Flash ...\r\n");
					for(i=0;i<bAPPSectors;i++)
					{
						W25QXX_Read(buf,FIRMWAREADDR+i*4096,4096);		//从0开始写入4096个数据
						iap_write_appbin(FLASH_APP1_ADDR+i*4096,buf,4096);//更新FLASH代码
						VTXprintf("i = %d,updata = %d%%\r\n",i,(i+1)*100/bAPPSectors);
					}
					buf[0]=0xff;buf[1]=bAPPSectors;
					W25QXX_Write(buf,UPDATAINFOADDR,2);
					VTXprintf("Writing Internal Flash Success...\r\n");
				}
				else
				{
					VTXprintf("Write Internal Flash False Because External Flash Program Error .\r\n");
				}
			}
			VTXprintf("Start Runing User Program ...\r\n");
			if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
			{
				iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
			}else
			{
				VTXprintf("Runing False Because Program Illegal .\r\n");
			}
			t=0;
		}
		if(key==KEY0_PRES)
		{
			W25QXX_Read(buf,FIRMWAREADDR,8);
			if(((*(vu32*)(buf+4))&0xFF000000)==0x08000000)
			{
				VTXprintf("Start Writing Internal Flash ...\r\n");
				for(i=0;i<bAPPSectors;i++)
				{
					W25QXX_Read(buf,FIRMWAREADDR+i*4096,4096);		//从0开始写入4096个数据
					iap_write_appbin(FLASH_APP1_ADDR+i*4096,buf,4096);//更新FLASH代码
					VTXprintf("i = %d,updata = %d%%\r\n",i,(i+1)*100/bAPPSectors);
				}
				buf[0]=0xff;buf[1]=bAPPSectors;
				W25QXX_Write(buf,UPDATAINFOADDR,2);
				VTXprintf("Writing Internal Flash Success...\r\n");
			}
			else
			{
				VTXprintf("Write Internal Flash False Because External Flash Program Error .\r\n");
			}
			VTXprintf("Start Runing User Program ...\r\n");
			if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
			{
				iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
			}else
			{
				VTXprintf("Runing False Because Program Illegal .\r\n");
			}
			t=0;
		}
#if CheckBound==0
		if(USART_RX_STA&0x8000)
		{
			LED2 = ~LED2;
			wLen=USART_RX_STA&0x3fff;
			USART_RX_BUF[wLen]=0;
			for(i=0;i<wLen;i++)
			{
				USART_SendData(USART1,USART_RX_BUF[i]);
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕 
			}
			
			if(strcmp((const char *)USART_RX_BUF,"K0")==0)
			{
				W25QXX_Read(buf,FIRMWAREADDR,8);
				if(((*(vu32*)(buf+4))&0xFF000000)==0x08000000)
				{
					VTXprintf("Start Writing Internal Flash ...\r\n");
					for(i=0;i<bAPPSectors;i++)
					{
						W25QXX_Read(buf,FIRMWAREADDR+i*4096,4096);		//从0开始写入4096个数据
						iap_write_appbin(FLASH_APP1_ADDR+i*4096,buf,4096);//更新FLASH代码
						VTXprintf("i = %d,updata = %d%%\r\n",i,(i+1)*100/bAPPSectors);
					}
					buf[0]=0xff;buf[1]=bAPPSectors;
					W25QXX_Write(buf,UPDATAINFOADDR,2);
					VTXprintf("Writing Internal Flash Success...\r\n");
				}
				else
				{
					VTXprintf("Write Internal Flash False Because External Flash Program Error .\r\n");
				}
				VTXprintf("Start Runing User Program ...\r\n");
				if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
				{
					iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
				}else
				{
					VTXprintf("Runing False Because Program Illegal .\r\n");
				}
				t=0;
			}
			//此处添加处理接收完的串口数据的函数
			USART_RX_STA=0;
		}
#endif
	}
}


