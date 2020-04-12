#ifndef _LOCK_H
#define _LOCK_H

#define LOCK_GPIO_PORT    	GPIOA			              /* GPIO端口 */
#define LOCK_GPIO_CLK 	    RCC_APB2Periph_GPIOA		/* GPIO端口时钟 */
#define LOCK_GPIO_PIN				GPIO_Pin_15			        /* 连接到SCL时钟线的GPIO */

void LockOpen(void);
void LOCK_GPIO_Config(void);

#endif


