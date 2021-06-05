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
	u16 i,t=0,wLen;				//���յ���app���볤��
	u8 bAPPSectors=0,key=0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(38400);	//���ڳ�ʼ��Ϊ115200
	delay_init();	   	 	//��ʱ��ʼ��
	W25QXX_Init();
 	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();
#if CheckBound==1
	usmart_dev.init(SystemCoreClock/1000000);	//��ʼ��USMART
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
			W25QXX_Read(buf,UPDATAINFOADDR,2);//������������������
			bAPPSectors=buf[1];
			if(buf[0]==0)
			{
				W25QXX_Read(buf,FIRMWAREADDR,8);
				if(((*(vu32*)(buf+4))&0xFF000000)==0x08000000)
				{
					VTXprintf("Start Writing Internal Flash ...\r\n");
					for(i=0;i<bAPPSectors;i++)
					{
						W25QXX_Read(buf,FIRMWAREADDR+i*4096,4096);		//��0��ʼд��4096������
						iap_write_appbin(FLASH_APP1_ADDR+i*4096,buf,4096);//����FLASH����
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
			if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
			{
				iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
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
					W25QXX_Read(buf,FIRMWAREADDR+i*4096,4096);		//��0��ʼд��4096������
					iap_write_appbin(FLASH_APP1_ADDR+i*4096,buf,4096);//����FLASH����
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
			if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
			{
				iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
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
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //ѭ������,ֱ��������� 
			}
			
			if(strcmp((const char *)USART_RX_BUF,"K0")==0)
			{
				W25QXX_Read(buf,FIRMWAREADDR,8);
				if(((*(vu32*)(buf+4))&0xFF000000)==0x08000000)
				{
					VTXprintf("Start Writing Internal Flash ...\r\n");
					for(i=0;i<bAPPSectors;i++)
					{
						W25QXX_Read(buf,FIRMWAREADDR+i*4096,4096);		//��0��ʼд��4096������
						iap_write_appbin(FLASH_APP1_ADDR+i*4096,buf,4096);//����FLASH����
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
				if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
				{
					iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
				}else
				{
					VTXprintf("Runing False Because Program Illegal .\r\n");
				}
				t=0;
			}
			//�˴���Ӵ��������Ĵ������ݵĺ���
			USART_RX_STA=0;
		}
#endif
	}
}


