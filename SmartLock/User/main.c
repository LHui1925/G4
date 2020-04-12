/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 �̹߳���
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 

/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
#include "board.h"
#include "rtthread.h"
#include "key.h"
#include "delay.h"     
#include "usart2.h"
#include "AS608.h"
#include "Handle_as608.h"
#include "string.h"
#include "Handle_code.h"
#include "Handle_flash.h"

#include "as60.h"
#include "as60test.h"
#include "SM4.h"
#include "play.h"
#include "web.h"
#include "handle.h"
#include "G4.h"
#include "newweb.h"

/*
*************************************************************************
*                               ����
*************************************************************************
*/
extern u16 num;

/*
*************************************************************************
*                             ȫ�ֱ���
*************************************************************************
*/

uint8_t MyKey[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
SM4_Key sm4_key;



/* �����߳̿��ƿ� */
static rt_thread_t key_thread = RT_NULL;

/*�߳̿��ƿ�*/
extern rt_thread_t webConnect_thread;//���������߳�
extern rt_thread_t webMsqSend_thread;//������Ϣ�����߳�
extern rt_thread_t webMsgHand_thread;//������Ϣ�����߳�
extern rt_thread_t handle_thread;//ָ�ƽ����߳̿��ƿ�
extern rt_thread_t codeOpen_thread;//��������߳̿��ƿ�
extern rt_thread_t unlock_thread;//�����߳�
extern rt_thread_t NetDetect_thread;
extern rt_thread_t play_thread;
rt_thread_t iwdg_thread=RT_NULL;
/*��Ϣ���п��ƿ�*/
extern rt_mq_t webMsgSend_mq;//����������Ϣ����Ϣ����
extern rt_mq_t codeOpen_mq;//���������Ϣ����
extern rt_mq_t play_mq;
/*�ź������ƿ�*/
extern rt_sem_t AS_Exit;//ָ�ƽ����ź���
extern rt_sem_t unlock_sem;//�����ź���


/*��Ϣ������ƿ�*/
extern rt_mailbox_t G4_mail;



/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void key_thread_entry(void* parameter);


/*
*************************************************************************
*                             main ����
*************************************************************************
*/
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
    /* 
	 * ������Ӳ����ʼ����RTTϵͳ��ʼ���Ѿ���main����֮ǰ��ɣ�
	 * ����component.c�ļ��е�rtthread_startup()����������ˡ�
	 * ������main�����У�ֻ��Ҫ�����̺߳������̼߳��ɡ�
	 */
	
	
	
  rt_kprintf("\n\n���ڰ�׿�ֻ��������������\n\n");
	
	
	/**************��Ϣ����*************/
	G4_mail = rt_mb_create("G4_mail",
													10, 
													RT_IPC_FLAG_FIFO);
 if (G4_mail != RT_NULL)
 {
	 rt_kprintf("\r\nG4_mail ��Ϣ���䴴���ɹ�\n\n");
 }else
 {
	 rt_kprintf("\r\nG4_mail ��Ϣ���䴴��ʧ��\n\n");
 }
 
 
/**************������Ϣ����*************/	
	play_mq = rt_mq_create("play_mq",/* ��Ϣ�������� */
                     2,     /* ��Ϣ����󳤶� */
                     10,    /* ��Ϣ���е�������� */
                     RT_IPC_FLAG_FIFO);/* ����ģʽ FIFO(0x00)*/
  if (play_mq != RT_NULL)
    rt_kprintf("\r\n play_mq ��Ϣ���д����ɹ���\r\n");
	else
		rt_kprintf("\r\n play_mq ��Ϣ���д���ʧ�ܣ�\r\n");
	
	
	

  key_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "key",              /* �߳����� */
                      key_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      512,                 /* �߳�ջ��С */
                      2,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
	
   if (key_thread != RT_NULL)
        rt_thread_startup(key_thread);
    else
        return -1;
		

		/*������Ϣ�����߳�*/
		webMsgHand_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "webMsgHand",              /* �߳����� */
                      WebMsgHandle_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      1024,                 /* �߳�ջ��С */
                      3,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (webMsgHand_thread != RT_NULL)
	 {
     rt_thread_startup(webMsgHand_thread);
		 rt_kprintf("\r\n webMsgHand_thread �̴߳����ɹ�\r\n");
	 }
    else
     rt_kprintf("\r\n webMsgHand_thread �̴߳���ʧ��\r\n");
		
		
		//-----------------------------------------------------------
		
		webmsg_sendmq = rt_mq_create("wmsgsend",/* ��Ϣ�������� */
                     2,     /* ��Ϣ����󳤶� */
                     10,    /* ��Ϣ���е�������� */
                     RT_IPC_FLAG_FIFO);/* ����ģʽ FIFO(0x00)*/
  
		webmsg_recemb = rt_mb_create("wmsgrec",
													5, 
													RT_IPC_FLAG_FIFO);
		
		webmsgsend_th =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "wmsgst",              /* �߳����� */
                      newwebconnect_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      3072,                 /* �߳�ջ��С */
                      3,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (webmsgsend_th != RT_NULL)
	 {
     rt_thread_startup(webmsgsend_th);
	 }
	 
	 
	 webmsgrece_th =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "wmsgrt",              /* �߳����� */
                      newwebhandle_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      3072,                 /* �߳�ջ��С */
                      3,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (webmsgrece_th != RT_NULL)
	 {
     rt_thread_startup(webmsgrece_th);
	 }
		
	}
/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/
static void key_thread_entry(void* parameter)
{	 	
	  
   	Handle_AS608();
    Handle_flash();
//	  aes();
   	WebConnect();
	  

		while(1)
	{
	
		num=GetKeyVal();
		if(num!=100)
		{
			rt_kprintf("num=%d\n\n",num);
			if(num==12)Del_FR();		//ɾָ��
			
			if(num==11)
			{
          handle_FR();			
			}
			
			if(num==13)
			{
          handle_sys();	
				
			}	
			
		}
		
		if(PS_Sta)	 //���PS_Sta״̬���������ָ����
		{
			press_FR();//ˢָ��			
		}		
	}
}





/********************************END OF FILE****************************/
