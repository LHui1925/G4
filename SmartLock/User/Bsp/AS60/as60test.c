#include "as60test.h"
#include "as608.h"
#include "board.h"
#include "rtthread.h"
#include "handle.h"
#include "as60.h"

extern rt_sem_t AS_Exit;
/**
   *@Brief 注册指纹
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
	/*验证指纹是否存在*/
	for(i=0;i<5;i++)
	{
		if(PS_Identify(&temp)==0x00)
		{
			return 0;//指纹已存在
		}
	}
	/*开始注册指纹*/
	for(i=0;i<10;i++)
	{
		if(PS_Enroll(&temp)==0x00)
		{
			*ID=temp;
			return 2;//录入成功
		}
	}
	return 1;//录入失败
}

/**
   *@Brief 验证指纹
   */
//uint8_t identifyFingerprint(uint16_t *ID)
//{
//	uint8_t i;
//	uint16_t temp;
//	/*验证指纹是否存在*/
//	for(i=0;i<5;i++)
//	{
//		if(PS_Identify(&temp)==0x00)
//		{
//			*ID=temp;
//			return 1;//验证成功
//		}
//	}
//	return 0;//失败
//}

/**
   *@Brief 获取特征文件
   */
uint8_t getFeatureFile(uint16_t ID,uint8_t *data,uint16_t *byteCount)
{
	uint8_t sure;
	sure=PS_LoadChar(CharBuffer1,ID);
	if(sure!=0x00)
	{
		return 0;//失败
	}
	sure=PS_UpChar(CharBuffer1,data,byteCount);
	return sure;
}

/**
   *@Brief 下载特征文件并保存
   */
uint8_t downloadSaveFeatureFile(uint16_t ID,uint8_t *data,uint16_t byteCount)
{
	uint8_t sure;
	uint16_t i;
	sure=PS_DownChar(CharBuffer1);
	if(sure!=0x00)
	{
		return 0;//失败
	}
	downloadFeatureFile(data,byteCount);
	sure=PS_StoreChar(CharBuffer1,ID);
	rt_kprintf("\r\n注册指纹特征文件:\r\n");
	for(i=0;i<byteCount;i++)
	{
		rt_kprintf("%x ",data[i]);
	}
	rt_kprintf("\r\n注册ID: %d\r\n",ID);
	rt_kprintf("\r\n注册指纹模板字节数: %d\r\n",byteCount);
	if(sure!=0x00)
	{
		return 0;
	}
	return 1;
}


