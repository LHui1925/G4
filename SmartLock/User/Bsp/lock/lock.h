#ifndef _LOCK_H
#define _LOCK_H

#define LOCK_GPIO_PORT    	GPIOA			              /* GPIO�˿� */
#define LOCK_GPIO_CLK 	    RCC_APB2Periph_GPIOA		/* GPIO�˿�ʱ�� */
#define LOCK_GPIO_PIN				GPIO_Pin_15			        /* ���ӵ�SCLʱ���ߵ�GPIO */

void LockOpen(void);
void LOCK_GPIO_Config(void);

#endif


