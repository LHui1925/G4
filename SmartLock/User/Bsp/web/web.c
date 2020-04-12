#include "web.h"
#include "G4.h"
#include "rtthread.h"
#include "board.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "cJSON.h"
#include "bsp_led.h" 
#include "bsp_spi_flash.h"
#include "handle.h"
#include "SM4.h"
#include "BASE64.h"
#include "MD5.h"
#include "as60test.h"
#include "as60.h"
#include "delay.h"
#include "myaes.h"
#include "AES.h"
#include "CRC.h"
#include "stdlib.h"
/*
*************************************************************************
*                             全局变量
*************************************************************************
*/
extern SM4_Key sm4_key;

uint8_t aes_key[16] = {0xc6,0xa5,0x7c,0x8c,0xa0,0xc6,0x4e,0x5d,0xd6,0x0c,0xb2,0x3e,0x4f,0x62,0x62,0xe0};
uint8_t palin_text[512]="";//待加密内容
int16_t plain_text_len=8;//加密内容的长度

uint8_t cipler_text[512]="";//存放加密后的数据
int16_t cipler_text_len=0;//加密后的密文的数据长度

uint8_t decode_text[512];//存放解密后的数据
int16_t decode_text_len=0;//解密后的数据的长度v


uint8_t send_data[1024]="";//发送数据包
int16_t data_len=0;
uint16_t crc =0;


uint8_t unVanish=0;   
uint8_t webConnect=0;//表示网络的连接状态
uint8_t response=0;

extern char openCode[7];
extern char deviceNumber[9];
extern rt_sem_t AS_Exit;
extern rt_sem_t unlock_sem;//解锁信号量

/*
*************************************************************************
*                             线程控制块
*************************************************************************
*/
rt_thread_t webConnect_thread=RT_NULL;   //网络连接线程
rt_thread_t webMsqSend_thread=RT_NULL;   //网络消息发送线程
rt_thread_t webMsgHand_thread=RT_NULL;   //网络消息处理线程
rt_thread_t NetDetect_thread=RT_NULL;    //网络连接检查线程

/*
*************************************************************************
*                             消息队列控制块
*************************************************************************
*/
rt_mq_t webMsgSend_mq=RT_NULL;//发送网络消息的消息队列
extern rt_mq_t play_mq;
/*
*************************************************************************
*                             邮箱
*************************************************************************
*/
extern rt_mailbox_t G4_mail;

void aes(void)
{
	

//	   int32_t res;
//	   uint16_t i,j;
//	   int16_t dataNum;
//		 strcpy((char *)palin_text,"dev00001");
//	   rt_kprintf("\r\ncipler_text111==%s\r\n",palin_text);
//	
//		 plain_text_len=strlen((char *)palin_text);
//	   rt_kprintf("\r\npalin_text111==%d\r\n",plain_text_len);
//	
//			
//     //加密
//	   myaes_encrypt(aes_key,palin_text,plain_text_len,cipler_text,&cipler_text_len);
//	   G4_SendString((char *)cipler_text);
//     G4_SendString("111\n\n");
//	
//		 Base64_EncodeStr((char *)cipler_text,16,(char *)palin_text,&dataNum);
//		 G4_SendString((char *)palin_text);
//		 G4_SendString("222\n\n");
//			
//		 Base64_DecodeToStr((char *)palin_text,dataNum,(char *)decode_text,&decode_text_len);
//		 G4_SendString((char *)decode_text);
//		 G4_SendString("333\n\n");	
//			
//		 myaes_decrypt(aes_key,decode_text,decode_text_len,decode_text,&dataNum);
//		 G4_SendString((char *)decode_text);;
//		 G4_SendString("444\n\n");	
}


/**
   *@Brief 网络连接函数，连接服务器
   */
//void WebConnect_entry(void *paramater)
void WebConnect(void)
{
	int32_t res;
	cJSON *json;
	char *out;
	uint16_t timeOut=0;//连接超时标志
	uint16_t i,j;
	int16_t dataNum;
	char signal;
	char send_d[512]="";
	
	int16_t high_len;
	int16_t low_len;
	
	
	/*基础设置*/
//	connect_init:
//	G4_RST();
//	timeOut=0;
//	rt_kprintf("\r\nenter AT------\r\n");
//	while(!G4_EnterATMode()&&timeOut<5) 
//	{
//	timeOut++;
//	rt_kprintf("\r\nceshi------\r\n");
//	}
//	if(timeOut>=5){goto connect_init;}
//	
//	rt_kprintf("\r\nlinkip------\r\n");
//	timeOut=0;
//	while(!G4_AT_SOCK("TCPC","47.97.165.179","9920")&&timeOut<5)
//	{timeOut++;}
//	if(timeOut>=5){goto connect_init;}	
//	
//	rt_kprintf("\r\nheart mode------\r\n");
//	timeOut=0;
//	while(!G4_AT_HEARTMOD("NET")&&timeOut<5)
//	{timeOut++;}
//	if(timeOut>=5){goto connect_init;}
//	
//	rt_kprintf("\r\nheart time------\r\n");
//	timeOut=0;
//	while(!G4_AT_HEARTM(0)&&timeOut<5)
//	{timeOut++;}
//	if(timeOut>=5){goto connect_init;}
//	
//	rt_kprintf("\r\nuart clear------\r\n");
//	timeOut=0;
//	while(!G4_AT_UARTCLR(1)&&timeOut<5)
//	{timeOut++;}
//	if(timeOut>=5){goto connect_init;}
//	
//	rt_kprintf("\r\nrst------\r\n");
//	timeOut=0;
//	while(!G4_AT_REBT()&&timeOut<5)
//	{timeOut++;}
//	if(timeOut>=5){goto connect_init;}
	
	while(1)
	{
		connect:
		G4_RST();
		unVanish=0;
		webConnect=0;
		/*连接网络*/
		while(G4_CheckLIKA()==0)
		{
		delay_ms(5000);
		rt_kprintf("\r\nwait connect to web------\r\n");
		}
		
		if(G4_CheckLIKA()==1)
		{
		 delay_ms(50);
//		 G4_SendString("success connect to web");
		 rt_kprintf("\r\nsuccess connect to web！\r\n");
		 
		}		
		unVanish=1;	
		/*连接验证*/
		while(1)
		{
	//发送验证数据
			rt_kprintf("\r\nsend identifitication message!\r\n");
/**********数据内容*************/
//加密
		 strcpy((char *)palin_text,"dev00001");
		 myaes_encrypt(aes_key,palin_text,8,cipler_text,&cipler_text_len);
		 Base64_EncodeStr((char *)cipler_text,16,(char *)palin_text,&dataNum);

			json=cJSON_CreateObject();
			cJSON_AddStringToObject(json,"deviceNumber","dev00001");
			cJSON_AddStringToObject(json,"signature",(char *)palin_text);
			out=cJSON_PrintUnformatted(json);
      
			data_len=strlen(out);	
			high_len= 0xff&(data_len>>8);
			low_len=0xff&(data_len);
			
			send_data[0]=0x02;
			send_data[1]=high_len;
			send_data[2]=low_len;
			
			data_len=0;
			while(out[data_len]!='\0')
			{
				send_data[data_len+3] = out[data_len];
        data_len++;
			}
			data_len=data_len+3;
			crc=crc16tablefast((uint8_t *)send_data,data_len);			
			send_data[data_len++]= 0xff&(crc>>8);
			send_data[data_len++]=0xff&(crc);
			G4_SendArray(send_data,data_len);
			
			Base64_EncodeStr((char *)send_data,data_len,send_d,&dataNum);
			rt_kprintf("\r\nsend_d=%s\r\n",send_d);
			
			rt_free(out);
			cJSON_Delete(json);
			delay_ms(5000);
			/*等待验证*/
			
//			timeOut=0;
//			rt_kprintf("\r\n等待服务器验证\r\n");
//			while(webConnect==0&&timeOut<10)
//			{
//				delay_ms(5000);
//				timeOut++;
//				rt_kprintf("\r\n等待服务器验证\r\n");
//			}
//			if(G4_CheckLIKA()==false)
//			{
//				goto connect;
//			}
//			if(webConnect==1)
//			{
//				timeOut=0;
//				break;
//			}  
		 }
		signal=0x02;//成功连接服务器
		rt_mq_send(play_mq,&signal,1);
		rt_kprintf("\r\n服务器验证验成功\r\n");
//		rt_thread_suspend(webConnect_thread);
//		rt_schedule();
	}
}

/**
   *@Brief 网络消息处理线程
   */
void WebMsgHandle_entry(void *paramater)
{
	cJSON *json,*jitem1,*jitem2,*jitem3;
	cJSON *jsend;
	char *out;
	char mqRecv[1024];
	char mqSend[1024];//发送数据缓冲
	int16_t mqRecvNum;
	int16_t mqSendNum;
	uint8_t hugeData[1024];
	uint8_t *point,result;
	uint16_t i;
	uint16_t temp;
	char phone_temp[20];
	rt_err_t err;
	struct G4_UART_BUFF *G4_MSG;
	
	while(1)
	{
		
		err=rt_mb_recv(G4_mail,(rt_uint32_t *)&G4_MSG,RT_WAITING_FOREVER);
//		
		rt_kprintf("\r\nG4_MSG=%s\r\n",G4_MSG);
		
		if(err!=RT_EOK)
		{
			continue;
		}
		
		rt_kprintf("\r\nG4_MSG=%s\r\n",G4_MSG);
		rt_kprintf("\r\n收到网络消息，需要处理\r\n");
		//检查是否为心跳包
		for(i=0;i<10;i++)
		{
			if((uint8_t)G4_MSG->data[i]!=0xff)
			{
				break;
			}
		}
		
		if(i>=10)
		{
			for(i=0;i<10;i++)
			{
				mqSend[i]=0xff;
			}
			rt_mq_send(webMsgSend_mq,mqSend,10);
			continue;
		}
		
		if(webConnect==0)
		{
			json=cJSON_Parse(mqRecv);
			if(json!=0)
			{		
				jitem1=cJSON_GetObjectItem( json , "content" );
				if(strstr(jitem1->valuestring,"init"))
				{
					jitem2=cJSON_GetObjectItem( json , "init" );
					if(strstr(jitem2->valuestring,"success"))
					{
						webConnect=1;
					}
					goto webHandleEnd;
				}
			}
		}		
		//解密数据
		Base64_DecodeToStr(G4_MSG->data,G4_MSG->num,mqSend,&mqSendNum);
		SM4_EncodeOrDecodeCh(mqSend,mqSendNum,mqRecv,&mqRecvNum,sm4_key,1);
			/*将解密后的数据存放到mqRecv*/
		if((uint8_t)mqRecv[0]==0x00)
		{
			rt_kprintf("\r\n收到字节型消息\r\n");
			for(i=0;i<1024;i++)
			{
				hugeData[i]=(uint8_t)mqRecv[i];
			}
			switch(hugeData[1])
			{
				case 0x00:
					rt_kprintf("\r\n收到指纹特征文件： \r\n");
					for(i=0;i<1024;i++)
					{
						if(hugeData[i]<0x10)
						{
							rt_kprintf("0%x ",hugeData[i]);
						}else
						{
							rt_kprintf("%x ",hugeData[i]);
						}
					}
					rt_kprintf("\r\n");
					point=hugeData+6;
					jsend=cJSON_CreateObject();
					cJSON_AddStringToObject(jsend,"content","downloadFingerprint");
					if(downloadSaveFeatureFile((hugeData[2]<<8)+hugeData[3],point,(hugeData[4]<<8)+hugeData[5])==1) //下载特征文件并保存
					{
						cJSON_AddStringToObject(jsend,"result","success");
					}else
					{
						cJSON_AddStringToObject(jsend,"result","fail");
					}
					cJSON_AddStringToObject(jsend,"phoneNumber",phone_temp);
					out=cJSON_PrintUnformatted(jsend);
					i=strlen(out);
					rt_mq_send(webMsgSend_mq,out,i+1);
					rt_free(out);
					cJSON_Delete(jsend);
					break;
				}
			continue;
		}
		json=cJSON_Parse(mqRecv);
		if(json!=0)
		{
			rt_kprintf("\r\n收到json string:\n%s\r\n",mqRecv);
			jitem1=cJSON_GetObjectItem( json , "content" );
			
			/*开锁*/
			if(strstr(jitem1->valuestring,"openCode"))
			{
				//开锁
				rt_sem_release(unlock_sem);
				
				jsend=cJSON_CreateObject();
				cJSON_AddStringToObject(jsend,"content","openCode");
				cJSON_AddStringToObject(jsend,"result","success");
				jitem3=cJSON_GetObjectItem( json , "phoneNumber" );
				cJSON_AddStringToObject(jsend,"phoneNumber",jitem3->valuestring);
				out=cJSON_PrintUnformatted(jsend);
				rt_kprintf("\r\n开锁应答： %s\r\n",out);
				i=strlen(out);
				rt_mq_send(webMsgSend_mq,out,i+1);
				rt_free(out);
				cJSON_Delete(jsend);
				goto webHandleEnd;
			}
			/*远程指纹解锁*/
			if(strstr(jitem1->valuestring,"openFingerprint"))
			{
				//开锁
				rt_sem_release(unlock_sem);
				jsend=cJSON_CreateObject();
				cJSON_AddStringToObject(jsend,"content","openFingerprint");
				cJSON_AddStringToObject(jsend,"result","success");
				jitem3=cJSON_GetObjectItem( json , "phoneNumber" );
				cJSON_AddStringToObject(jsend,"phoneNumber",jitem3->valuestring);
				out=cJSON_PrintUnformatted(jsend);
				i=strlen(out);
				rt_mq_send(webMsgSend_mq,out,i+1);
				rt_free(out);
				cJSON_Delete(jsend);
				goto webHandleEnd;
			}
			/*密码解锁*/
			if(strstr(jitem1->valuestring,"unlockCode"))
			{
				jitem2=cJSON_GetObjectItem( json , "unlockCode" );
				if(strstr(jitem2->valuestring,"yes"))
				{
					rt_sem_release(unlock_sem);
				}
				goto webHandleEnd;
			}
			if(strstr(jitem1->valuestring,"response"))
			{
				response=1;
				goto webHandleEnd;
			}
			if(strstr(jitem1->valuestring,"userImport"))
			{
				jsend=cJSON_CreateObject();
				cJSON_AddStringToObject(jsend,"content","userImport");
				cJSON_AddStringToObject(jsend,"result","success");
				jitem3=cJSON_GetObjectItem( json , "phoneNumber" );
				strcpy(phone_temp,jitem3->valuestring);
				cJSON_AddStringToObject(jsend,"phoneNumber",jitem3->valuestring);
				out=cJSON_PrintUnformatted(jsend);
				i=strlen(out);
				rt_mq_send(webMsgSend_mq,out,i+1);
				rt_free(out);
				cJSON_Delete(jsend);
				goto webHandleEnd;
			}
			/*增加用户*/
			if(strstr(jitem1->valuestring,"userAdd"))
			{
				jsend=cJSON_CreateObject();
				cJSON_AddStringToObject(jsend,"content","userAdd");
				
				result=registerFingerprint(&temp);  //注册指纹
				
				if(result!=2)
				{
					cJSON_AddStringToObject(jsend,"result","fail");
				  cJSON_AddStringToObject(jsend,"id","");
				}else
				{
					cJSON_AddStringToObject(jsend,"result","success");
				  cJSON_AddNumberToObject(jsend,"id",temp);
				}
				jitem3=cJSON_GetObjectItem( json , "phoneNumber" );
				cJSON_AddStringToObject(jsend,"phoneNumber",jitem3->valuestring);
				out=cJSON_PrintUnformatted(jsend);
				i=strlen(out);
				rt_mq_send(webMsgSend_mq,out,i+1);
				rt_kprintf("\r\n增加用户应答： %s\r\n",out);
				if(result==2)
				{
					hugeData[0]=0x00;//告诉发送线程发送字节型数据
					hugeData[3]=0x00;//告诉服务器收到的是字节型数据
					hugeData[4]=0x00;//告诉服务器收到的是指纹模板特征文件
					hugeData[5]=temp>>8;//指纹ID号高字节
					hugeData[6]=temp;//指纹ID号低字节
					if(getFeatureFile(temp,hugeData+9,&i)==1) //获取特征文件
					{
						hugeData[7]=i>>8;//指纹特征文件的字节数高位
						hugeData[8]=i;//指纹特征文件的字节数低位
						hugeData[1]=(i+9-3)>>8;//高数发送线程发送数据的字节数的高位
						hugeData[2]=i+9-3;//告诉发送线程发送数据的字节数的低位
						rt_mq_send(webMsgSend_mq,hugeData,i+9);
					}
				}
				rt_free(out);
				cJSON_Delete(jsend);
				goto webHandleEnd;
			}
			/*删除用户*/
			if(strstr(jitem1->valuestring,"userDelete"))
			{
				jitem2=cJSON_GetObjectItem( json , "id" );
				jsend=cJSON_CreateObject();
				cJSON_AddStringToObject(jsend,"content","userDelete");
				if(PS_DeletChar(jitem2->valueint,1)==0x00)
				{
					cJSON_AddStringToObject(jsend,"result","success");
					cJSON_AddNumberToObject(jsend,"id",jitem2->valueint);
				}else
				{
					cJSON_AddStringToObject(jsend,"result","fail");
				}
				jitem3=cJSON_GetObjectItem( json , "phoneNumber" );
				cJSON_AddStringToObject(jsend,"phoneNumber",jitem3->valuestring);
				out=cJSON_PrintUnformatted(jsend);
				i=strlen(out);
				rt_mq_send(webMsgSend_mq,out,i+1);
				rt_kprintf("\r\n删除用户应答： %s\r\n",out);
				rt_free(out);
				cJSON_Delete(jsend);
				goto webHandleEnd;
			}
			if(strstr(jitem1->valuestring,"userClear"))
			{
				jsend=cJSON_CreateObject();
				cJSON_AddStringToObject(jsend,"content","userClear");
				if(PS_Empty()==0x00)
				{
					cJSON_AddStringToObject(jsend,"result","success");
				}else
				{
					cJSON_AddStringToObject(jsend,"result","fail");
				}
				jitem3=cJSON_GetObjectItem( json , "phoneNumber" );
				cJSON_AddStringToObject(jsend,"phoneNumber",jitem3->valuestring);
				out=cJSON_PrintUnformatted(jsend);
				i=strlen(out);
				rt_mq_send(webMsgSend_mq,out,i+1);
				rt_free(out);
				cJSON_Delete(jsend);
			}
			/*收到心跳包*/
webHandleEnd:
			cJSON_Delete(json);
			rt_kprintf("\r\n收到的网络消息处理结束\r\n");
			continue;
		}
		rt_kprintf("\r\n收到的网络消息格式错误\r\n");
	}
}

/**
   *@Brief 发送网络消息线程
   */
void WebMsgSend_entry(void *paramater)
{
	uint8_t mqRecv[1500];
	char mqSend[1500];
	int16_t mqRecvNum,mqSendNum;
	uint8_t *point;
	uint16_t i;
	char signal;
	while(1)
	{
		rt_mq_recv(webMsgSend_mq,mqRecv,1024,RT_WAITING_FOREVER);
		if(webConnect!=1)
		{
			signal=0x00;//消息发送失败
			rt_mq_send(play_mq,&signal,1);
			continue;	
		}
		if((G4_CheckSTAT()==false)||(G4_CheckLIKA()==false))
		{
			signal=0x00;//消息发送失败
			rt_mq_send(play_mq,&signal,1);
			continue;	
		}
		//发送心跳包
		if(mqRecv[0]==0xff)
		{
			G4_SendArray(mqRecv,10);
			continue;
		}
		rt_kprintf("\r\n收到需要发送的网络消息\r\n");
		if(mqRecv[0]==0x00)
		{
			rt_kprintf("\r\n收到指纹特征文件\r\n");
			i=(mqRecv[1]<<8)+mqRecv[2];
			point=mqRecv+3;
			SM4_EncodeOrDecodeUCh(point,i,mqRecv,&mqRecvNum,sm4_key,0);
			Base64_EncodeUChArr(mqRecv,mqRecvNum,mqSend,&mqSendNum);
		}else
		{
			rt_kprintf("\r\n收到字符型数据:\r\n    %s\r\n",mqRecv);
			i=strlen((char *)mqRecv);
			SM4_EncodeOrDecodeUCh(mqRecv,i,mqRecv,&mqRecvNum,sm4_key,0);
			Base64_EncodeUChArr(mqRecv,mqRecvNum,mqSend,&mqSendNum);
		}
send:
		G4_SendString(mqSend);
		response=0;
		i=0;
		while(response==0&&i<10)
		{
			delay_ms(1000);
			i++;
			if(i==5)
			{
				goto send;
			}
		}
		if(i>=10)
		{
			rt_kprintf("\r\n文件发送失败\r\n");
			signal=0x00;//消息发送失败
			rt_mq_send(play_mq,&signal,1);
			continue;
		}
		signal=0x01;//消息发送成功
		rt_mq_send(play_mq,&signal,1);
		rt_kprintf("\r\n文件发送成功\r\n");
	}
}

/**
   *@Brief 网络监测线程
   */
void NetDetect_entry(void *parameter)
{
	char signal;
	while(1)
	{
		if(webConnect==1)
		{
			if((G4_CheckSTAT()==false)||(G4_CheckLIKA()==false))
				{
					unVanish=0;
					webConnect=0;
					signal=0x03;//服务器失去连接
					rt_mq_send(play_mq,&signal,1);
					rt_thread_resume(webConnect_thread);
				}
		}
		delay_ms(1000);
	}
}
