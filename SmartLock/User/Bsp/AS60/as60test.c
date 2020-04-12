#include "as60test.h"
#include "as608.h"
#include "board.h"
#include "rtthread.h"
#include "handle.h"
#include "as60.h"

extern rt_sem_t AS_Exit;
/**
   *@Brief ע��ָ��
   */
uint8_t registerFingerprint(uint16_t *ID)
{
	uint8_t i;
	uint16_t temp;
	rt_err_t err;
	err=rt_sem_take(AS_Exit,10000);
	if(err!=RT_EOK)
	{
		return 1;
	}
	/*��ָ֤���Ƿ����*/
	for(i=0;i<5;i++)
	{
		if(PS_Identify(&temp)==0x00)
		{
			return 0;//ָ���Ѵ���
		}
	}
	/*��ʼע��ָ��*/
	for(i=0;i<10;i++)
	{
		if(PS_Enroll(&temp)==0x00)
		{
			*ID=temp;
			return 2;//¼��ɹ�
		}
	}
	return 1;//¼��ʧ��
}

/**
   *@Brief ��ָ֤��
   */
//uint8_t identifyFingerprint(uint16_t *ID)
//{
//	uint8_t i;
//	uint16_t temp;
//	/*��ָ֤���Ƿ����*/
//	for(i=0;i<5;i++)
//	{
//		if(PS_Identify(&temp)==0x00)
//		{
//			*ID=temp;
//			return 1;//��֤�ɹ�
//		}
//	}
//	return 0;//ʧ��
//}

/**
   *@Brief ��ȡ�����ļ�
   */
uint8_t getFeatureFile(uint16_t ID,uint8_t *data,uint16_t *byteCount)
{
	uint8_t sure;
	sure=PS_LoadChar(CharBuffer1,ID);
	if(sure!=0x00)
	{
		return 0;//ʧ��
	}
	sure=PS_UpChar(CharBuffer1,data,byteCount);
	return sure;
}

/**
   *@Brief ���������ļ�������
   */
uint8_t downloadSaveFeatureFile(uint16_t ID,uint8_t *data,uint16_t byteCount)
{
	uint8_t sure;
	uint16_t i;
	sure=PS_DownChar(CharBuffer1);
	if(sure!=0x00)
	{
		return 0;//ʧ��
	}
	downloadFeatureFile(data,byteCount);
	sure=PS_StoreChar(CharBuffer1,ID);
	rt_kprintf("\r\nע��ָ�������ļ�:\r\n");
	for(i=0;i<byteCount;i++)
	{
		rt_kprintf("%x ",data[i]);
	}
	rt_kprintf("\r\nע��ID: %d\r\n",ID);
	rt_kprintf("\r\nע��ָ��ģ���ֽ���: %d\r\n",byteCount);
	if(sure!=0x00)
	{
		return 0;
	}
	return 1;
}


