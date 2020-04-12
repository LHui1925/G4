#include "as60.h"
#include "bsp_led.h" 
#include "board.h"
#include "rtthread.h"
#include "delay.h"

rt_sem_t AS_Exit=RT_NULL;//as608���·��ص�

uint32_t AS608_addr=0xffffffff;//as608��ַ
uint16_t capacity;//ָ�ƿ�����
uint16_t PackSize;//���ݰ���С
uint16_t baudRate;//������


uint8_t AS_responseBuff[1024];//Ӧ�����ݻ�����
uint8_t AS_responseFlag=0;//����ָ����Ƿ���Ӧ��
//uint8_t *temporaryData;
//uint8_t temporaryFlag=0;
//uint16_t temporaryByteCount=0;


/**
  * @brief  AS608_USART������ָ��ģ�鴫������ 
  * @param  data;���������
  */
void AS608_SendData(uint8_t data)
{
  USART_SendData(AS608_USART,data);
  while(USART_GetFlagStatus(AS608_USART,USART_FLAG_TXE) == RESET);
}

/**
  * @brief  ���Ͱ�ͷ
  */
void AS608_SendPackHead()
{
	AS608_SendData(0xef);
	AS608_SendData(0x01);
}

/**
  * @brief  ���͵�ַ
  */
void AS608_SendAddr()
{
	AS608_SendData(AS608_addr>>24);
	AS608_SendData(AS608_addr>>16);
	AS608_SendData(AS608_addr>>8);
	AS608_SendData(AS608_addr);
}

/**
  * @brief  ���Ͱ���ʶ
  */
void AS608_SendFlag(uint8_t flag)
{
	AS608_SendData(flag);
}

/**
  * @brief  ���Ͱ�����
  */
void AS608_SendLength(uint16_t length)
{
	AS608_SendData(length>>8);
	AS608_SendData(length);
}

/**
  * @brief  ���Ͱ�ָ����
  */
void AS608_SendCom(uint8_t com)
{
	AS608_SendData(com);
}

/**
  * @brief  ����У���
  * @param  check:���λ
  * @retval ��
  */
void SendCheck(uint16_t check)
{
	AS608_SendData(check>>8);
	AS608_SendData(check);
}




/**
  * @brief  �Զ�ע�ᡣ
	* @param  PageID ע��õ���ID 
  * @retval ȷ����=00H ע��ɹ���
						ȷ����=01H ��ʾ�հ��д�
						ȷ����=1eH ע��ʧ�ܣ�
						ȷ����=FFH ��ʱ
  */
uint8_t PS_Enroll(uint16_t *PageID)
{
	uint16_t temp;//�м����
  uint8_t sure;//���ص�ȷ����
	uint16_t count=0;//�������ǵ�50��ʱ
	AS_responseFlag=0;//Ӧ���־����
	AS608_SendPackHead();
	AS608_SendAddr();
	AS608_SendFlag(0x01);
	AS608_SendLength(0x03);
	AS608_SendCom(0x10);  //PS_Enroll    	
	temp = 0x14;
	SendCheck(temp);
  while(AS_responseFlag==0&&count<500)
	{
		count++;
		AS_Delay(10);
	}
	if(count>=500)
	{
		//rt_kprintf("\r\n�豸��Ӧ��\r\n");
		return 0xff;
	}
	//rt_kprintf("\r\n�豸Ӧ��\r\n");
	sure=AS_responseBuff[9];
	*PageID=(AS_responseBuff[10]<<8)+AS_responseBuff[11];
  return  sure;
}


/**
  * @brief  �Զ���֤��
	* @param  PageID �����õ���ID 
  * @retval ȷ����=00H �����ɹ���
						ȷ����=01H ��ʾ�հ��д�
						ȷ����=09H ����ʧ�ܣ�û����������
						ȷ����=FFH ��ʱ
  */
uint8_t PS_Identify(uint16_t *PageID)
{
	uint16_t temp;//�м����
  uint8_t sure;//���ص�ȷ����
	uint16_t count=0;//�������ǵ�50��ʱ
	AS_responseFlag=0;//Ӧ���־����
	AS608_SendPackHead();
	AS608_SendAddr();
	AS608_SendFlag(0x01);
	AS608_SendLength(0x03);
	AS608_SendCom(0x11);  //PS_Identify    	
	temp = 0x15;
	SendCheck(temp);
  while(AS_responseFlag==0&&count<200)
	{
		count++;
		AS_Delay(10);
	}
	if(count>=200)
	{
		//rt_kprintf("\r\n�豸��Ӧ��\r\n");
		return 0xff;
	}
	//rt_kprintf("\r\n�豸Ӧ��\r\n");
	sure=AS_responseBuff[9];
	*PageID=(AS_responseBuff[10]<<8)+AS_responseBuff[11];
  return  sure;
}


/**
   *@Brief ��֤password
   *@Param ����
   *@RetVal ȷ����=00H �ɹ���
						ȷ����=ffH�� ��ʱ��
   */
uint8_t PS_VfyPwd(uint32_t password)
{
	uint16_t temp;//�м����
  uint8_t sure;//���ص�ȷ����
	uint16_t count=0;//�������ǵ�50��ʱ
	AS_responseFlag=0;//Ӧ���־����
	AS608_SendPackHead();
	AS608_SendAddr();
	AS608_SendFlag(0x01);
	AS608_SendLength(0x07);
	AS608_SendCom(0x13);  //PS_Identify    	
	AS608_SendData(password>>24);	
	AS608_SendData(password>>16);	
	AS608_SendData(password>>8);	
	AS608_SendData(password);	
	temp = 0x1b;
	SendCheck(temp);
  while(AS_responseFlag==0&&count<500)
	{
		count++;
		AS_Delay(10);
	}
	if(count>=500)
	{
		//rt_kprintf("\r\n�豸��Ӧ��\r\n");
		return 0xff;
	}
	//rt_kprintf("\r\n�豸Ӧ��\r\n");
	sure=AS_responseBuff[9];
  return  sure;
}

/**
  * @brief   ��flash���ݿ���ָ��ID�ŵ�ָ��ģ���������ģ�建����CharBuffer1��CharBuffer2
	* @param  BufferID ģ�建������
	* @param  PageID ID��
  * @retval ȷ����=00H �ɹ���
						ȷ����=01H ��ʾ�հ��д�
						ȷ����=0cH �����д��ģ����Ч
						ȷ����=0bH pageID������Χ
						ȷ����=FFH ��ʱ
  */
uint8_t PS_LoadChar (uint8_t BufferID,uint16_t PageID)
{
	uint16_t temp;//�м����
  uint8_t sure;//���ص�ȷ����
	uint16_t count=0;//�������ǵ�50��ʱ
	AS_responseFlag=0;//Ӧ���־����
	rt_kprintf("\r\nload char *********\r\n");
	AS608_SendPackHead();
	AS608_SendAddr();
	AS608_SendFlag(0x01);
	AS608_SendLength(0x06);
	AS608_SendCom(0x07);  //PS_LoadChar
	AS608_SendData(BufferID);
	AS608_SendData(PageID>>8);
	AS608_SendData(PageID);	
	temp = 0x01+0x06+0x07+BufferID+((PageID>>8)&0x00ff)+((PageID)&0x00ff);
	SendCheck(temp);
  while(AS_responseFlag==0&&count<500)
	{
		count++;
		AS_Delay(10);
	}
	if(count>=500)
	{
		//rt_kprintf("\r\n�豸��Ӧ��\r\n");
		rt_kprintf("\r\nload char fail*********\r\n");
		return 0xff;
	}
	//rt_kprintf("\r\n�豸Ӧ��\r\n");
	sure=AS_responseBuff[9];
	rt_kprintf("\r\nload char end*********\r\n");
  return  sure;
}

/**
  * @brief  ��ָ�������ļ���������ָ�������ļ��ϴ�����λ��
	* @param  BufferID ģ�建������
  * @retval 0 ʧ��
						1 �ɹ�
  */
uint8_t PS_UpChar(uint8_t BufferID,uint8_t *data,uint16_t *byteCount)
{
	uint16_t temp;//�м����
  uint8_t sure;//���ص�ȷ����
	uint8_t tag;//��¼����ʶ
	uint8_t *buffer;//ָ�����ݻ�����
	uint16_t count=0,i;//����
	uint16_t length;//һ�����ݰ������ݳ���
	AS_responseFlag=0;//Ӧ���־����
	AS608_SendPackHead();
	AS608_SendAddr();
	AS608_SendFlag(0x01);
	AS608_SendLength(0x04);
	AS608_SendCom(0x08);  //PS_UpChar
	AS608_SendData(BufferID);
	temp = 0x01+0x04+0x08+BufferID;
	SendCheck(temp);
  while(AS_responseFlag==0&&count<500)
	{
		count++;
		AS_Delay(10);
	}
	if(count>=500)
	{
		return 0;
	}
	sure=AS_responseBuff[9];
	if(sure!=0x00)
	{
		return 0;
	}
	count=0;
	buffer=AS_responseBuff+12;
	do
	{
		buffer=buffer+6;
		tag=*buffer;
		buffer++;
		length=(*buffer)<<8;
		buffer++;
		length=length+*buffer;
		buffer++;
		rt_kprintf("\r\n length: %d\r\n",length);
		rt_kprintf("\r\n data:\r\n");
		for(i=0;i<length-2;i++)
		{
			data[count]=*buffer;
			count++;
			//rt_kprintf("%x ",*buffer);
			buffer++;
		}
		buffer++;
		buffer++;
	}while(tag!=0x08);
	*byteCount=count;
	return 1;
}

/**
  * @brief  ��λ����ָ�������ļ�������ָ��ģ��ָ���������ļ�������
	* @param  BufferID ģ�建������
  * @retval ȷ����=00H �ɹ�����λ���������Է������ݰ�
						ȷ����=01H ��ʾ�հ��д�
						ȷ����=0fH ���ܷ������ݰ�
						ȷ����=FFH ��ʱ
  */
uint8_t PS_DownChar(uint8_t BufferID)
{
	uint16_t temp;//�м����
  uint8_t sure;//���ص�ȷ����
	uint16_t count=0;//�������ǵ�50000��ʱ
	AS_responseFlag=0;//Ӧ���־����
	temp = 0x01+0x04+0x09+BufferID;
	AS608_SendPackHead();
	AS608_SendAddr();
	AS608_SendFlag(0x01);
	AS608_SendLength(0x04);
	AS608_SendCom(0x09);  //PS_DownChar
	AS608_SendData(BufferID);
	SendCheck(temp);
  while(AS_responseFlag==0&&count<500)
	{
		count++;
		AS_Delay(10);
	}
	if(count>=500)
	{
		//rt_kprintf("\r\n�豸��Ӧ��\r\n");
		return 0xff;
	}
	//rt_kprintf("\r\n�豸Ӧ��\r\n");
	sure=AS_responseBuff[9];
  return  sure;
}
/**
   *@Brief ��ģ�鷢������
   *@Param data ����������ָ��
	 *@Param byteCount �����������ֽ���
   */
void downloadFeatureFile(uint8_t *data,uint16_t byteCount)
{
	uint16_t count=0;
	uint16_t sum;
	uint16_t length;
	uint8_t tag;
	uint16_t i;
	while(count<byteCount)
	{
		if((byteCount-count)>PackSize)//�޸�Ϊ���ݰ��Ĵ�С
		{
			tag=0x02;
			length=PackSize+2;//�޸�Ϊ���ݰ��Ĵ�С��2
			
		}else
		{
			tag=0x08;
			length=byteCount-count+2;
		}
		sum=tag+((length>>8)&0x00ff)+(length&0x00ff);
		AS608_SendPackHead();
		AS608_SendAddr();
		AS608_SendFlag(tag);
		AS608_SendLength(length);
		for(i=0;i<length-2;i++)
		{
			sum=sum+data[count];
			AS608_SendData(data[count]);
			count++;
		}
		AS608_SendData(sum>>8);
		AS608_SendData(sum);
	}
}




