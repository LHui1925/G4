#include "lock.h"
#include "board.h"
#include "rtthread.h"

void LOCK_GPIO_Config(void)
{		
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*开启lock相关的GPIO外设时钟*/
		RCC_APB2PeriphClockCmd( LOCK_GPIO_CLK , ENABLE);
		/*选择要控制的GPIO引脚*/
		GPIO_InitStructure.GPIO_Pin = LOCK_GPIO_PIN;	

		/*设置引脚模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*设置引脚速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*调用库函数，初始化GPIO*/
		GPIO_Init(LOCK_GPIO_PORT, &GPIO_InitStructure);	
		
    
    /* 关闭所有lock	*/
		GPIO_ResetBits(LOCK_GPIO_PORT, LOCK_GPIO_PIN);
}

void LockOpen(void)
{
	GPIO_SetBits(LOCK_GPIO_PORT, LOCK_GPIO_PIN);
	rt_thread_delay(2000);
	GPIO_ResetBits(LOCK_GPIO_PORT, LOCK_GPIO_PIN);
}


