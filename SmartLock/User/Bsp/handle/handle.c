#include "handle.h"
#include "rtthread.h"
#include "board.h"
#include "as608.h"
#include "cJSON.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "cJSON.h"
#include "lock.h"
#include "as60.h"

/*��Ϣ����*/
extern rt_mq_t webMsgSend_mq;//������Ϣ��Ϣ����
rt_mq_t codeOpen_mq=RT_NULL;//���������Ϣ����
extern rt_mq_t play_mq;
/*�߳̿��ƿ�*/
rt_thread_t handle_thread=RT_NULL;//ָ�ƽ����߳̿��ƿ�
rt_thread_t codeOpen_thread=RT_NULL;//��������߳̿��ƿ�
rt_thread_t unlock_thread=RT_NULL;//�����߳�
/*�ź���*/
extern rt_sem_t AS_Exit;//ָ�ƽ����ź���
rt_sem_t unlock_sem=RT_NULL;//�����ź���

/**
   *@Brief ����ָ�ƽ���
   */
void handle_entry(void *parameter)
{
	rt_err_t err=0xff;
	uint16_t temp;
	cJSON *jsend;
	char *out;
	int i;
	char sendTemp[128];
	char signal;
	while(1)
	{
		/*�����󴥣����ν���֮�䲻�����̫��*/
		err=rt_sem_take(AS_Exit,5000);
		if(err==RT_EOK)
		{
			continue;
		}
		err=rt_sem_take(AS_Exit,RT_WAITING_FOREVER);
		if(err==RT_EOK)
		{
			signal=0x04;//������ʾ
			rt_mq_send(play_mq,&signal,1);
			//��֤
			for(i=0;i<5;i++)
			{
				if(PS_Identify(&temp)==0x00)
				{
					break;
				}
			}
			if(i>=5)
			{
				signal=0x00;//
				rt_mq_send(play_mq,&signal,1);
				//ָ����֤ʧ��
			}else
			{
				signal=0x01;//
				rt_mq_send(play_mq,&signal,1);
				//ָ����֤�ɹ�
				rt_sem_release(unlock_sem);
				jsend=cJSON_CreateObject();
				cJSON_AddStringToObject(jsend,"content","login");
				cJSON_AddNumberToObject(jsend,"id",temp);
				out=cJSON_PrintUnformatted(jsend);
				rt_kprintf("\r\n����login�� %s\r\n",out);
				i=strlen(out);
				rt_mq_send(webMsgSend_mq,out,i+1);
				rt_free(out);
				cJSON_Delete(jsend);
			}
		}
	}
}

/**
   *@Brief ��������������߳�
   */
void codeOpen_entry(void *parameter)
{
	char temp;
	char codeTemp[25];
	char sendTemp[128];
	char *out;
	cJSON *jsend;
	uint8_t count=0;
	uint16_t i;
	rt_err_t err;
	char signal;
	while(1)
	{
		rt_kprintf("\r\ncodeTemp :%s",codeTemp);
		if(count==0)
		{
			//count=0����ʾ֮ǰû���������룬����һֱ�ȴ�
			err=rt_mq_recv(codeOpen_mq,&temp,1,RT_WAITING_FOREVER);
			if(err==RT_EOK)
			{
				rt_kprintf("\r\nkeyBoard :%c\r\n",temp);
				if(temp>='0'&&temp<='9')
				{
					codeTemp[count]=temp;
				  count++;
					codeTemp[count]='\0';
					//��ȷ������ʾ
					signal=0x05;//
					rt_mq_send(play_mq,&signal,1);
				}else
				{
					//����������ʾ
					signal=0x06;//
					rt_mq_send(play_mq,&signal,1);
				}
			}
		}else
		{
			//count��=0��֮ǰ����������룬��λ����֮��ʱ�������ܲ��5�룬������Ϊ����ʧ��
			err=rt_mq_recv(codeOpen_mq,&temp,1,5000);
			if(err!=RT_EOK)
			{
				count=0;
				codeTemp[count]='\0';
			}
			else
			{
				signal=0x04;//������ʾ
				rt_mq_send(play_mq,&signal,1);
				rt_kprintf("\r\nkeyBoard :%c\r\n",temp);
				if(count<6)
				{
					if(temp>='0'&&temp<='9')
					{
						codeTemp[count]=temp;
						count++;
						codeTemp[count]='\0';
						//��ȷ������ʾ
						signal=0x05;//
						rt_mq_send(play_mq,&signal,1);
					}else if(temp=='c')
					{
						count=0;
						codeTemp[count]='\0';
						//��ȷ������ʾ
						signal=0x05;//
						rt_mq_send(play_mq,&signal,1);
					}else
					{
						//����������ʾ
						signal=0x06;//
						rt_mq_send(play_mq,&signal,1);
					}
				}else if(count<20)
				{
					if(temp>='0'&&temp<='9')
					{
						codeTemp[count]=temp;
						count++;
						codeTemp[count]='\0';
						
						signal=0x05;//
						rt_mq_send(play_mq,&signal,1);
						
					}else if(temp=='c')
					{
						count=0;
						codeTemp[count]='\0';
						//��ȷ������ʾ
						signal=0x05;//
						rt_mq_send(play_mq,&signal,1);
					}else if(temp=='d')
					{
						signal=0x05;//
						rt_mq_send(play_mq,&signal,1);
						
						
						codeTemp[count]='\0';
						jsend=cJSON_CreateObject();
						cJSON_AddStringToObject(jsend,"content","unlockCode");
						cJSON_AddStringToObject(jsend,"unlockCode",codeTemp);
						out=cJSON_PrintUnformatted(jsend);
						rt_kprintf("\r\n����login�� %s\r\n",out);
						i=strlen(out);
						rt_mq_send(webMsgSend_mq,out,i+1);
						rt_free(out);
						cJSON_Delete(jsend);
						count=0;
						codeTemp[count]='\0';
					}else
					{
						//����������ʾ
						signal=0x06;//
						rt_mq_send(play_mq,&signal,1);
					}
				}else
				{
					if(temp=='c')
					{
						count=0;
						codeTemp[count]='\0';
						//��ȷ������ʾ
						signal=0x05;//
						rt_mq_send(play_mq,&signal,1);
					}else if(temp=='d')
					{
						signal=0x05;//
						rt_mq_send(play_mq,&signal,1);
						
						
						codeTemp[count]='\0';
						jsend=cJSON_CreateObject();
						cJSON_AddStringToObject(jsend,"content","unlockCode");
						cJSON_AddStringToObject(jsend,"unlockCode",codeTemp);
						out=cJSON_PrintUnformatted(jsend);
						rt_kprintf("\r\n����login�� %s\r\n",out);
						i=strlen(out);
						rt_mq_send(webMsgSend_mq,out,i+1);
						rt_free(out);
						cJSON_Delete(jsend);
						count=0;
						codeTemp[count]='\0';
					}else
					{
						//����������ʾ
						signal=0x06;//
						rt_mq_send(play_mq,&signal,1);
					}
				}
			}	
		}
	}
}
	
	
void unlock_entry(void *parameter)
{
	rt_err_t err;
	while(1)
	{
		err=rt_sem_take(unlock_sem,5000);
		if(err!=RT_EOK)
		{
			//����
			err=rt_sem_take(unlock_sem,RT_WAITING_FOREVER);
			LockOpen();
		}
	}
}


