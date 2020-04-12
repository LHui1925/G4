#include "board.h"
#include "rtthread.h"
#include "key.h"
#include "bsp_usart.h"
#include "beep.h"
#include "sys.h"
#include "delay.h"  
#include "usart2.h"
#include "AS608.h"
#include "timer.h"
#include "Handle_as608.h"
#include "stdio.h"
#include "string.h"

SysPara AS608Para;//ָ��ģ��AS608����
u16 ValidN;//ģ������Чָ�Ƹ���
//u8** kbd_tbl;
int num=100;
int verify_sign=0;

//��ȡ������ֵ
u16 GET_NUM(void)
{
	u8  key_num=0;
	u16 num=0;
	while(1)
	{
		key_num=GetKeyVal();	
		if(key_num!=100)
		{
			if(key_num==13)return 0xFFFF; //�����ء���
			if(key_num==14)return 0xFF00;  //	
//      if(key_num>0&&key_num<=9&&num<99)			
			if(key_num>0&&key_num<=9&&num<9999)//��1-9����(��������3λ��)
				num =num*10+key_num;		
			if(key_num==21)num =num/10;//��Del����		
//      if(key_num==0&&num<99)num =num*10;			
			if(key_num==0&&num<9999)num =num*10;//��0����
			if(key_num==20)return num;  //��Enter����
			rt_kprintf("num=%d\n\n",num);
		}
	}	
}


u16 GET_str(unsigned char *str_num)
{
	char  key_num=0;
//	u16 num=0;
	unsigned char count=0;
	while(1)
	{
		key_num=GetKeyVal();	
		if(key_num!=100)
		{
			
			if(key_num==13)return 0xFFFF; //�����ء���
			if(key_num==14)return 0xFF00;  //			
			if(key_num>=0&&key_num<=9)//��1-9����(��������3λ��)
			{
				str_num[count++] = key_num+'0';
				str_num[count] = '\0';
			}		
			if(key_num==21)str_num[--count]='\0';//��Del����					
			if(key_num==20)return 0xF000;  //��Enter����
			rt_kprintf("str_num=%s\n\n",str_num);
		}
	}	
	
	
}




void del_str(unsigned char *str_num)
{
	unsigned char count;
	u16 length=0;
	length = strlen((const char *)str_num);
	for(count=0;count<length;count++)
		str_num[count] = '\0';
}



//¼ָ��
void Add_FR(void)
{
	u8 i,ensure ,processnum=0;
	u16 ID;
	u16 count=0;
	
	rt_kprintf("��ʼ¼ָ�ƣ�����\n\n");
	
	while(1)
	{
		count++;
		switch (processnum)
		{
			case 0:
				i++;
				rt_kprintf("�밴ָ��\n\n");
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					BEEP=1;
					ensure=PS_GenChar(CharBuffer1);//��������
					BEEP=0;
					if(ensure==0x00)
					{
						rt_kprintf("ָ������\n\n");
						i=0;
						processnum=1;//�����ڶ���						
					}else rt_kprintf("ָ�ƴ���\n\n");//ShowErrMessage(ensure);				
				}else rt_kprintf("������������\n\n");//ShowErrMessage(ensure);						
				break;
			
			case 1:
				i++;
				rt_kprintf("�밴�ٰ�һ��ָ��\n\n");
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					BEEP=1;
					ensure=PS_GenChar(CharBuffer2);//��������
					BEEP=0;
					if(ensure==0x00)
					{
						rt_kprintf("ָ������\n\n");
						i=0;
						processnum=2;//����������
					}else rt_kprintf("ָ�ƴ���\n\n");// ShowErrMessage(ensure);	
				}else rt_kprintf("������������\n\n");//ShowErrMessage(ensure);		
				break;

			case 2:
				rt_kprintf("�Ա�����ָ��\n\n");
				ensure=PS_Match();
				if(ensure==0x00) 
				{
					rt_kprintf("�Աȳɹ�,����ָ��һ��\n\n");
					processnum=3;//�������Ĳ�
				}
				else 
				{
					rt_kprintf("�Ա�ʧ�ܣ�������¼��ָ��\n\n");
					i=0;
					processnum=0;//���ص�һ��		
				}
				delay_ms(1200);
				break;

			case 3:
				rt_kprintf("����ָ��ģ��\n\n");
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{
				rt_kprintf("����ָ��ģ��ɹ�\n\n");
					processnum=4;//�������岽
				}
				else 
				{
					processnum=0;
				rt_kprintf("����ָ��ģ�����\n\n");
				}
				delay_ms(1200);
				break;
				
			case 4:	
				rt_kprintf("�����봢��ID,��Enter����\n\n");
				rt_kprintf("0=< ID <=299\n\n");
			
				do
					ID=GET_NUM();
				while(!(ID<AS608Para.PS_max));//����ID����С��ָ�����������ֵ
				ensure=PS_StoreChar(CharBuffer2,ID);//����ģ��
				if(ensure==0x00) 
				{			
					rt_kprintf("¼��ָ�Ƴɹ�\n\n");		
					PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
					rt_kprintf("AS608Para.PS_max-ValidN=%d\n\n",AS608Para.PS_max-ValidN);
					delay_ms(1500);
					return ;
				}else {processnum=0;rt_kprintf("¼��ָ��ʧ��\n\n");}					
				break;				
		}
		delay_ms(1000);
		if(i==5)//����5��û�а���ָ���˳�
		{
			rt_kprintf("����5��û�а���ָ���˳�������\n\n");
			break;	
		}	
		if(count==10)//����5��û�а���ָ���˳�
		{
			rt_kprintf("����5��û�а���ָ���˳�������\n\n");
			break;	
		}			
	}
}

//ˢָ��
void press_FR(void)
{
	SearchResult seach;
	u8 ensure;
	char *str;
	ensure=PS_GetImage();
	if(ensure==0x00)//��ȡͼ��ɹ� 
	{	
		BEEP=1;//�򿪷�����	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //���������ɹ�
		{		
			BEEP=0;//�رշ�����	
			ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
			if(ensure==0x00)//�����ɹ�
			{				
				rt_kprintf("ˢָ�Ƴɹ�\n\n");				
				str=rt_malloc(50);
				sprintf(str,"ȷ�д���,ID:%d  ƥ��÷�:%d\n\n",seach.pageID,seach.mathscore);
				rt_kprintf("str=%s\n\n",str);	
//				rt_free(str);
			}
			else 
				rt_kprintf("����ʧ�ܣ�\n\n");					
	  }
		else
     rt_kprintf("��������ʧ��\n\n");
	 BEEP=0;//�رշ�����
	 delay_ms(600);

	}
}

//��ָ֤��
void verify_FR(void)
{
	SearchResult seach;
	u8 ensure;
	char *str;
	int i=0;
	while(1)
	{
		i++;
	 if(PS_Sta)	 //���PS_Sta״̬���������ָ����
	 {
	ensure=PS_GetImage();
	if(ensure==0x00)//��ȡͼ��ɹ� 
	{	
		BEEP=1;//�򿪷�����	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //���������ɹ�
		{		
			BEEP=0;//�رշ�����	
			ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
			if(ensure==0x00)//�����ɹ�
			{				
				rt_kprintf("ˢָ�Ƴɹ�\n\n");				
				str=rt_malloc(50);
				sprintf(str,"����ָ��,ID:%d  ƥ��÷�:%d\n\n�뻻��ָ��!!!\n\n",seach.pageID,seach.mathscore);
				rt_kprintf("str=%s\n\n",str);	
				rt_free(str);
				verify_sign=1;
				if(i==6)
				{
				break;	
				}
			}
			else 
			{
				verify_sign=0;
				rt_kprintf("��ָ��δ¼�룡����\n\n");		
				Add_FR();	//¼ָ��
				break;	
			}				
	  }
		else
		{
     rt_kprintf("����ָ������ʧ�ܣ��˳�������\n\n");
			break;	
		}
	 BEEP=0;//�رշ�����
	 delay_ms(600);
	}
	}
	 
	   rt_kprintf("�밴ָ�ƣ�����\n\n");
			if(i==10)//����10��û�а���ָ���˳�
		{
			verify_sign=2;
			rt_kprintf("����10��û�а���ָ���˳�������\n\n");
			break;	
		}	
	delay_ms(600);
 }
}

//ɾ��ָ��
void Del_FR(void)
{
	u8  ensure;
	u16 num;
	rt_kprintf("ɾ��ָ��\n\n");
	rt_kprintf("������ָ��ID��Enter����\n\n");
	rt_kprintf("0=< ID <=299\n\n");
	delay_ms(50);
//	AS608_load_keyboard(0,170,(u8**)kbd_delFR);
	num=GET_NUM();//��ȡ���ص���ֵ
//	if(num==0xFFFF)
//	{
//		rt_kprintf("����������\n\n");
//		goto MENU ; //������ҳ��
//	}
		if(num==0xFFFF)
	{
		rt_kprintf("����������\n\n");
		
	}
//		goto MENU ; //������ҳ��
	if(num==0xFF00)
		ensure=PS_Empty();//���ָ�ƿ�
	else 
		ensure=PS_DeletChar(num,1);//ɾ������ָ��
	if(ensure==0)
	{
		rt_kprintf("ɾ��ָ�Ƴɹ�!!!\n\n");
		
	}
  else
		rt_kprintf("ɾ��ָ�Ʋ��ɹ�!!!\n\n");
	delay_ms(1200);
	PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	rt_kprintf("AS608Para.PS_max-ValidN=%d\n\n",AS608Para.PS_max-ValidN);
	
//MENU:	
  	delay_ms(50);
}

void Handle_AS608(void)
{
	  u8 ensure;
	  char *str;	
	/*����ָ��ʶ��ʵ�����*/

  rt_kprintf("AS608ָ��ʶ��ģ����Գ���\n\n");
  rt_kprintf("��AS608ģ������....\n\n");	
	
	while(PS_HandShake(&AS608Addr))//��AS608ģ������
	{
		delay_ms(400);
		rt_kprintf("δ��⵽ģ�飡����\n\n");
		delay_ms(800);
		rt_kprintf("��������ģ��...\n\n");	  
	}

	rt_kprintf("ͨѶ�ɹ�!!!\n\n");
	str=rt_malloc(30);
	sprintf(str,"������:%d   ��ַ:%x",usart2_baund,AS608Addr);
	rt_kprintf("%s\n\n",str);
	ensure=PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	if(ensure!=0x00)
		rt_kprintf("OK\n\n");//ShowErrMessage(ensure);//��ʾȷ���������Ϣ	
	ensure=PS_ReadSysPara(&AS608Para);  //������ 
	if(ensure==0x00)
	{
		rt_memset(str,0,50);
		sprintf(str,"������:%d     �Աȵȼ�: %d",AS608Para.PS_max-ValidN,AS608Para.PS_level);
		rt_kprintf("%s\n\n",str);
	}
	else
		rt_kprintf("ERROR\n\n");//ShowErrMessage(ensure);	
	rt_free(str);
//	AS608_load_keyboard(0,170,(u8**)kbd_menu);//�����������
	

}
