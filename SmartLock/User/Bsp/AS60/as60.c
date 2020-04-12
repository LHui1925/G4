#include "as60.h"
#include "bsp_led.h" 
#include "board.h"
#include "rtthread.h"
#include "delay.h"

rt_sem_t AS_Exit=RT_NULL;//as608按下发回的

uint32_t AS608_addr=0xffffffff;//as608地址
uint16_t capacity;//指纹库容量
uint16_t PackSize;//数据包大小
uint16_t baudRate;//波特率


uint8_t AS_responseBuff[1024];//应答数据缓冲区
uint8_t AS_responseFlag=0;//发送指令后是否有应答
//uint8_t *temporaryData;
//uint8_t temporaryFlag=0;
//uint16_t temporaryByteCount=0;


/**
  * @brief  AS608_USART串口向指纹模块传递数据 
  * @param  data;传输的数据
  */
void AS608_SendData(uint8_t data)
{
  USART_SendData(AS608_USART,data);
  while(USART_GetFlagStatus(AS608_USART,USART_FLAG_TXE) == RESET);
}

/**
  * @brief  发送包头
  */
void AS608_SendPackHead()
{
	AS608_SendData(0xef);
	AS608_SendData(0x01);
}

/**
  * @brief  发送地址
  */
void AS608_SendAddr()
{
	AS608_SendData(AS608_addr>>24);
	AS608_SendData(AS608_addr>>16);
	AS608_SendData(AS608_addr>>8);
	AS608_SendData(AS608_addr);
}

/**
  * @brief  发送包标识
  */
void AS608_SendFlag(uint8_t flag)
{
	AS608_SendData(flag);
}

/**
  * @brief  发送包长度
  */
void AS608_SendLength(uint16_t length)
{
	AS608_SendData(length>>8);
	AS608_SendData(length);
}

/**
  * @brief  发送包指令码
  */
void AS608_SendCom(uint8_t com)
{
	AS608_SendData(com);
}

/**
  * @brief  发送校验和
  * @param  check:检查位
  * @retval 无
  */
void SendCheck(uint16_t check)
{
	AS608_SendData(check>>8);
	AS608_SendData(check);
}




/**
  * @brief  自动注册。
	* @param  PageID 注册得到的ID 
  * @retval 确认码=00H 注册成功；
						确认码=01H 表示收包有错；
						确认码=1eH 注册失败；
						确认码=FFH 超时
  */
uint8_t PS_Enroll(uint16_t *PageID)
{
	uint16_t temp;//中间变量
  uint8_t sure;//返回的确认码
	uint16_t count=0;//计数，记到50则超时
	AS_responseFlag=0;//应答标志清零
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
		//rt_kprintf("\r\n设备无应答\r\n");
		return 0xff;
	}
	//rt_kprintf("\r\n设备应答\r\n");
	sure=AS_responseBuff[9];
	*PageID=(AS_responseBuff[10]<<8)+AS_responseBuff[11];
  return  sure;
}


/**
  * @brief  自动验证。
	* @param  PageID 搜索得到的ID 
  * @retval 确认码=00H 搜索成功；
						确认码=01H 表示收包有错；
						确认码=09H 搜索失败，没有搜索到；
						确认码=FFH 超时
  */
uint8_t PS_Identify(uint16_t *PageID)
{
	uint16_t temp;//中间变量
  uint8_t sure;//返回的确认码
	uint16_t count=0;//计数，记到50则超时
	AS_responseFlag=0;//应答标志清零
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
		//rt_kprintf("\r\n设备无应答\r\n");
		return 0xff;
	}
	//rt_kprintf("\r\n设备应答\r\n");
	sure=AS_responseBuff[9];
	*PageID=(AS_responseBuff[10]<<8)+AS_responseBuff[11];
  return  sure;
}


/**
   *@Brief 验证password
   *@Param 密码
   *@RetVal 确认码=00H 成功；
						确认码=ffHΤ 超时。
   */
uint8_t PS_VfyPwd(uint32_t password)
{
	uint16_t temp;//中间变量
  uint8_t sure;//返回的确认码
	uint16_t count=0;//计数，记到50则超时
	AS_responseFlag=0;//应答标志清零
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
		//rt_kprintf("\r\n设备无应答\r\n");
		return 0xff;
	}
	//rt_kprintf("\r\n设备应答\r\n");
	sure=AS_responseBuff[9];
  return  sure;
}

/**
  * @brief   将flash数据库中指定ID号的指纹模板读出放在模板缓冲区CharBuffer1或CharBuffer2
	* @param  BufferID 模板缓冲区号
	* @param  PageID ID号
  * @retval 确认码=00H 成功；
						确认码=01H 表示收包有错；
						确认码=0cH 读出有错或模板无效
						确认码=0bH pageID超出范围
						确认码=FFH 超时
  */
uint8_t PS_LoadChar (uint8_t BufferID,uint16_t PageID)
{
	uint16_t temp;//中间变量
  uint8_t sure;//返回的确认码
	uint16_t count=0;//计数，记到50则超时
	AS_responseFlag=0;//应答标志清零
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
		//rt_kprintf("\r\n设备无应答\r\n");
		rt_kprintf("\r\nload char fail*********\r\n");
		return 0xff;
	}
	//rt_kprintf("\r\n设备应答\r\n");
	sure=AS_responseBuff[9];
	rt_kprintf("\r\nload char end*********\r\n");
  return  sure;
}

/**
  * @brief  将指定特征文件缓冲区的指纹特征文件上传至上位机
	* @param  BufferID 模板缓冲区号
  * @retval 0 失败
						1 成功
  */
uint8_t PS_UpChar(uint8_t BufferID,uint8_t *data,uint16_t *byteCount)
{
	uint16_t temp;//中间变量
  uint8_t sure;//返回的确认码
	uint8_t tag;//记录包标识
	uint8_t *buffer;//指向数据缓冲区
	uint16_t count=0,i;//计数
	uint16_t length;//一个数据包的数据长度
	AS_responseFlag=0;//应答标志清零
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
  * @brief  上位机将指纹特征文件下载至指纹模块指定的特征文件缓冲区
	* @param  BufferID 模板缓冲区号
  * @retval 确认码=00H 成功，上位机后续可以发送数据包
						确认码=01H 表示收包有错；
						确认码=0fH 不能发送数据包
						确认码=FFH 超时
  */
uint8_t PS_DownChar(uint8_t BufferID)
{
	uint16_t temp;//中间变量
  uint8_t sure;//返回的确认码
	uint16_t count=0;//计数，记到50000则超时
	AS_responseFlag=0;//应答标志清零
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
		//rt_kprintf("\r\n设备无应答\r\n");
		return 0xff;
	}
	//rt_kprintf("\r\n设备应答\r\n");
	sure=AS_responseBuff[9];
  return  sure;
}
/**
   *@Brief 向模块发送数据
   *@Param data 待发送数据指针
	 *@Param byteCount 待发送数据字节数
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
		if((byteCount-count)>PackSize)//修改为数据包的大小
		{
			tag=0x02;
			length=PackSize+2;//修改为数据包的大小加2
			
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




