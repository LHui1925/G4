#include "newweb.h"

//��֤��Ϣ�������֤��Ϣֻ������dev00001
uint8_t token[]={
	0x02,0x00,0x42,0x7b,0x22,0x64,0x65,0x76,0x69,0x63,0x65,0x4e,0x75,0x6d,0x62,
	0x65,0x72,0x22,0x3a,0x22,0x64,0x65,0x76,0x30,0x30,0x30,0x30,0x31,0x22,0x2c,
	0x22,0x73,0x69,0x67,0x6e,0x61,0x74,0x75,0x72,0x65,0x22,0x3a,0x22,0x6f,0x4a,
	0x72,0x32,0x38,0x31,0x53,0x39,0x51,0x77,0x36,0x48,0x78,0x6f,0x62,0x74,0x38,
	0x73,0x34,0x34,0x74,0x77,0x3d,0x3d,0x22,0x7d,0x76,0x6
};

uint8_t auth_key[16]={0xc6,0xa5,0x7c,0x8c,0xa0,0xc6,0x4e,0x5d,0xd6,0x0c,0xb2,0x3e,0x4f,0x62,0x62,0xe0};//��֤��Կ
uint8_t comm_key[16];//ͨ����Կ
rt_mq_t webmsg_sendmq=RT_NULL;//�������ݵ���Ϣ����
rt_mailbox_t webmsg_recemb = RT_NULL;//�������ݵ�����
rt_thread_t webmsgsend_th=RT_NULL;   //�������Ӻͷ�����Ϣ�߳�
rt_thread_t webmsgrece_th=RT_NULL;   //������Ϣ���պʹ����߳�

uint8_t conn_flag=0;//�����֤�����ӵ�����˵ı�־��1��ʾ���ӵ������

//��װ���͵����ݣ�ƥ�����ݰ��ĸ�ʽ
void assembleWebMsg(char *msg,int16_t msglen,char *res,int16_t *reslen)
{
	int16_t i;
	uint16_t crc;
	char *p;
	p = res + 3;
	//��������
	myaes_encrypt(comm_key,(uint8_t *)msg,msglen,(uint8_t *)p,&i);
	i = msglen + 3;
	//������ݰ�ͷ
	res[0] = 0x02;
	res[1] = 0xff&(i>>8);
	res[2] = 0xff&(i);
	//����crcֵ
	crc = crc16tablefast((uint8_t *)res,i);
	//���crc
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
		//���ӷ���˲���
		while(1)
		{
			//����Ƿ����ӷ����
			if(G4_CheckLIKA()==true)
			{
				break;
			}else
			{
				rt_thread_delay(10);
			}
		}
		//������֤��Ϣ
		G4_SendArray(token,71);
		//�ȴ��������֤�����ȴ�5�룬ʱ�������������
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
		//���i��50��˵��û����֤�ɹ���������֤
		if(i >= 50)
		{
			continue;
		}
		//�ȴ������̣߳���������Ҫ���͵�����˵�����
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
				caclen = rt_strlen(cache);//��������͵����ݳ��ȣ���Ϊ���͵�����ΪJSON��ʽ�����ַ��������Կ�����strlen���㳤��
				assembleWebMsg(cache,caclen,cache2,&caclen);//��װ���͵�����
				G4_SendArray((uint8_t *)cache2,caclen);//��������
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
			//δ�����֤���յ����Ƿ���˷��ص���֤�����Ϣ
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
			//�����֤���ͨ�����ݣ�ʵ�ʵĶ��豸�Ŀ���ָ��
			myaes_decrypt(comm_key,(uint8_t *)recp,reclen,cache,&caclen);
			cache[caclen] = '\0';
			json = cJSON_Parse((char *)cache);
		}
	}
}

