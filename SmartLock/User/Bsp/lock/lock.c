#include "lock.h"
#include "board.h"
#include "rtthread.h"

void LOCK_GPIO_Config(void)
{		
		/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*����lock��ص�GPIO����ʱ��*/
		RCC_APB2PeriphClockCmd( LOCK_GPIO_CLK , ENABLE);
		/*ѡ��Ҫ���Ƶ�GPIO����*/
		GPIO_InitStructure.GPIO_Pin = LOCK_GPIO_PIN;	

		/*��������ģʽΪͨ���������*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*������������Ϊ50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*���ÿ⺯������ʼ��GPIO*/
		GPIO_Init(LOCK_GPIO_PORT, &GPIO_InitStructure);	
		
    
    /* �ر�����lock	*/
		GPIO_ResetBits(LOCK_GPIO_PORT, LOCK_GPIO_PIN);
}

void LockOpen(void)
{
	GPIO_SetBits(LOCK_GPIO_PORT, LOCK_GPIO_PIN);
	rt_thread_delay(2000);
	GPIO_ResetBits(LOCK_GPIO_PORT, LOCK_GPIO_PIN);
}


