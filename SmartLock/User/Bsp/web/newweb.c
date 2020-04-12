#include "newweb.h"

//认证信息，这个认证信息只能用于dev00001
uint8_t token[]={
	0x02,0x00,0x42,0x7b,0x22,0x64,0x65,0x76,0x69,0x63,0x65,0x4e,0x75,0x6d,0x62,
	0x65,0x72,0x22,0x3a,0x22,0x64,0x65,0x76,0x30,0x30,0x30,0x30,0x31,0x22,0x2c,
	0x22,0x73,0x69,0x67,0x6e,0x61,0x74,0x75,0x72,0x65,0x22,0x3a,0x22,0x6f,0x4a,
	0x72,0x32,0x38,0x31,0x53,0x39,0x51,0x77,0x36,0x48,0x78,0x6f,0x62,0x74,0x38,
	0x73,0x34,0x34,0x74,0x77,0x3d,0x3d,0x22,0x7d,0x76,0x6
};

uint8_t auth_key[16]={0xc6,0xa5,0x7c,0x8c,0xa0,0xc6,0x4e,0x5d,0xd6,0x0c,0xb2,0x3e,0x4f,0x62,0x62,0xe0};//认证密钥
uint8_t comm_key[16];//通信密钥
rt_mq_t webmsg_sendmq=RT_NULL;//发送内容的消息队列
rt_mailbox_t webmsg_recemb = RT_NULL;//接收内容的邮箱
rt_thread_t webmsgsend_th=RT_NULL;   //网络连接和发送消息线程
rt_thread_t webmsgrece_th=RT_NULL;   //网络消息接收和处理线程

uint8_t conn_flag=0;//完成认证，连接到服务端的标志，1表示连接到服务端

//组装发送的数据，匹配数据包的格式
void assembleWebMsg(char *msg,int16_t msglen,char *res,int16_t *reslen)
{
	int16_t i;
	uint16_t crc;
	char *p;
	p = res + 3;
	//加密数据
	myaes_encrypt(comm_key,(uint8_t *)msg,msglen,(uint8_t *)p,&i);
	i = msglen + 3;
	//填充数据包头
	res[0] = 0x02;
	res[1] = 0xff&(i>>8);
	res[2] = 0xff&(i);
	//计算crc值
	crc = crc16tablefast((uint8_t *)res,i);
	//填充crc
	res[i++] = 0xff&(crc>>8);
	res[i++] = 0xff&(crc);
	*reslen = i;
}

void newwebconnect_entry(void *parameter)
{
	uint8_t i;
	int16_t caclen;
	char cache[1024];
	char cache2[1024];
	rt_err_t err;
	while(1)
	{
		//连接服务端部分
		while(1)
		{
			//检查是否连接服务端
			if(G4_CheckLIKA()==true)
			{
				break;
			}else
			{
				rt_thread_delay(10);
			}
		}
		//发送认证信息
		G4_SendArray(token,71);
		//等待服务端认证，最多等待5秒，时间可以另外设置
		i=0;
		while(i<50)
		{
			rt_thread_delay(100);
			if(conn_flag == 1)
			{
				break;
			}
			i++;
		}
		//如果i到50，说明没有认证成功，重新认证
		if(i >= 50)
		{
			continue;
		}
		//等待其他线程，发来的需要发送到服务端的数据
		while(1)
		{
			err = rt_mq_recv(webmsg_sendmq,cache,1024,RT_WAITING_FOREVER);
			if(G4_CheckLIKA()==false)
			{
				conn_flag=0;
				break;
			}
			if(err == RT_EOK)
			{
				caclen = rt_strlen(cache);//计算待发送的数据长度，因为发送的数据为JSON格式，是字符串，所以可以用strlen计算长度
				assembleWebMsg(cache,caclen,cache2,&caclen);//组装发送的数据
				G4_SendArray((uint8_t *)cache2,caclen);//发送数据
			}
		}
	}
}

void newwebhandle_entry(void *parameter)
{
	rt_err_t err;
	cJSON *json,*jitem1,*jitem2;
	struct G4_UART_BUFF *G4_MSG;
	char * recp;
	int16_t reclen;
	uint8_t cache[1024];
	int16_t caclen;
	while(1)
	{
		err=rt_mb_recv(webmsg_recemb,(rt_uint32_t *)&G4_MSG,RT_WAITING_FOREVER);
		if(err != RT_EOK)
		{
			continue;
		}
		reclen = G4_MSG->num - 5;
		recp = G4_MSG->data + 3;
		if(conn_flag == 0)
		{
			//未完成认证，收到的是服务端返回的认证结果信息
			recp[reclen] = '\0';
			json = cJSON_Parse(recp);
			if(json != RT_NULL)
			{
				jitem1 = cJSON_GetObjectItem(json,"res");
				if((rt_strcmp(jitem1->valuestring,"success")))
				{
					jitem2 = cJSON_GetObjectItem(json,"commkey");
					Base64_DecodeToUChArr(jitem2->valuestring,rt_strlen(jitem2->valuestring),(uint8_t *)cache,&caclen);
					myaes_decrypt(auth_key,(uint8_t *)cache,caclen,cache,&caclen);
					for(reclen=0;reclen<caclen;reclen++)
					{
						comm_key[reclen] = cache[reclen];
					}
					conn_flag = 1;
				}
			}
		}else
		{
			//完成认证后的通信内容，实际的对设备的控制指令
			myaes_decrypt(comm_key,(uint8_t *)recp,reclen,cache,&caclen);
			cache[caclen] = '\0';
			json = cJSON_Parse((char *)cache);
		}
	}
}

