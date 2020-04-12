/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 线程管理
  *********************************************************************
  * @attention
  *
  * 实验平台:野火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 

/*
*************************************************************************
*                             包含的头文件
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
*                               变量
*************************************************************************
*/
extern u16 num;

/*
*************************************************************************
*                             全局变量
*************************************************************************
*/

uint8_t MyKey[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
SM4_Key sm4_key;



/* 定义线程控制块 */
static rt_thread_t key_thread = RT_NULL;

/*线程控制块*/
extern rt_thread_t webConnect_thread;//网络连接线程
extern rt_thread_t webMsqSend_thread;//网络消息发送线程
extern rt_thread_t webMsgHand_thread;//网络消息处理线程
extern rt_thread_t handle_thread;//指纹解锁线程控制块
extern rt_thread_t codeOpen_thread;//密码解锁线程控制块
extern rt_thread_t unlock_thread;//开锁线程
extern rt_thread_t NetDetect_thread;
extern rt_thread_t play_thread;
rt_thread_t iwdg_thread=RT_NULL;
/*消息队列控制块*/
extern rt_mq_t webMsgSend_mq;//发送网络消息的消息队列
extern rt_mq_t codeOpen_mq;//密码解锁消息队列
extern rt_mq_t play_mq;
/*信号量控制块*/
extern rt_sem_t AS_Exit;//指纹解锁信号量
extern rt_sem_t unlock_sem;//解锁信号量


/*消息邮箱控制块*/
extern rt_mailbox_t G4_mail;



/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void key_thread_entry(void* parameter);


/*
*************************************************************************
*                             main 函数
*************************************************************************
*/
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
    /* 
	 * 开发板硬件初始化，RTT系统初始化已经在main函数之前完成，
	 * 即在component.c文件中的rtthread_startup()函数中完成了。
	 * 所以在main函数中，只需要创建线程和启动线程即可。
	 */
	
	
	
  rt_kprintf("\n\n基于安卓手机的智能门锁设计\n\n");
	
	
	/**************消息邮箱*************/
	G4_mail = rt_mb_create("G4_mail",
													10, 
													RT_IPC_FLAG_FIFO);
 if (G4_mail != RT_NULL)
 {
	 rt_kprintf("\r\nG4_mail 消息邮箱创建成功\n\n");
 }else
 {
	 rt_kprintf("\r\nG4_mail 消息邮箱创建失败\n\n");
 }
 
 
/**************创建消息队列*************/	
	play_mq = rt_mq_create("play_mq",/* 消息队列名字 */
                     2,     /* 消息的最大长度 */
                     10,    /* 消息队列的最大容量 */
                     RT_IPC_FLAG_FIFO);/* 队列模式 FIFO(0x00)*/
  if (play_mq != RT_NULL)
    rt_kprintf("\r\n play_mq 消息队列创建成功！\r\n");
	else
		rt_kprintf("\r\n play_mq 消息队列创建失败！\r\n");
	
	
	

  key_thread =                          /* 线程控制块指针 */
    rt_thread_create( "key",              /* 线程名字 */
                      key_thread_entry,   /* 线程入口函数 */
                      RT_NULL,             /* 线程入口函数参数 */
                      512,                 /* 线程栈大小 */
                      2,                   /* 线程的优先级 */
                      20);                 /* 线程时间片 */
                   
    /* 启动线程，开启调度 */
	
   if (key_thread != RT_NULL)
        rt_thread_startup(key_thread);
    else
        return -1;
		

		/*网络消息处理线程*/
		webMsgHand_thread =                          /* 线程控制块指针 */
    rt_thread_create( "webMsgHand",              /* 线程名字 */
                      WebMsgHandle_entry,   /* 线程入口函数 */
                      RT_NULL,             /* 线程入口函数参数 */
                      1024,                 /* 线程栈大小 */
                      3,                   /* 线程的优先级 */
                      20);                 /* 线程时间片 */
                   
    /* 启动线程，开启调度 */
   if (webMsgHand_thread != RT_NULL)
	 {
     rt_thread_startup(webMsgHand_thread);
		 rt_kprintf("\r\n webMsgHand_thread 线程创建成功\r\n");
	 }
    else
     rt_kprintf("\r\n webMsgHand_thread 线程创建失败\r\n");
		
		
		//-----------------------------------------------------------
		
		webmsg_sendmq = rt_mq_create("wmsgsend",/* 消息队列名字 */
                     2,     /* 消息的最大长度 */
                     10,    /* 消息队列的最大容量 */
                     RT_IPC_FLAG_FIFO);/* 队列模式 FIFO(0x00)*/
  
		webmsg_recemb = rt_mb_create("wmsgrec",
													5, 
													RT_IPC_FLAG_FIFO);
		
		webmsgsend_th =                          /* 线程控制块指针 */
    rt_thread_create( "wmsgst",              /* 线程名字 */
                      newwebconnect_entry,   /* 线程入口函数 */
                      RT_NULL,             /* 线程入口函数参数 */
                      3072,                 /* 线程栈大小 */
                      3,                   /* 线程的优先级 */
                      20);                 /* 线程时间片 */
                   
    /* 启动线程，开启调度 */
   if (webmsgsend_th != RT_NULL)
	 {
     rt_thread_startup(webmsgsend_th);
	 }
	 
	 
	 webmsgrece_th =                          /* 线程控制块指针 */
    rt_thread_create( "wmsgrt",              /* 线程名字 */
                      newwebhandle_entry,   /* 线程入口函数 */
                      RT_NULL,             /* 线程入口函数参数 */
                      3072,                 /* 线程栈大小 */
                      3,                   /* 线程的优先级 */
                      20);                 /* 线程时间片 */
                   
    /* 启动线程，开启调度 */
   if (webmsgrece_th != RT_NULL)
	 {
     rt_thread_startup(webmsgrece_th);
	 }
		
	}
/*
*************************************************************************
*                             线程定义
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
			if(num==12)Del_FR();		//删指纹
			
			if(num==11)
			{
          handle_FR();			
			}
			
			if(num==13)
			{
          handle_sys();	
				
			}	
			
		}
		
		if(PS_Sta)	 //检测PS_Sta状态，如果有手指按下
		{
			press_FR();//刷指纹			
		}		
	}
}





/********************************END OF FILE****************************/
