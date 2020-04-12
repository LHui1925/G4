#ifndef _AS60_H
#define _AS60_H

#include "stm32f10x.h"

#define CHAR_BUFFER1 0x01
#define CHAR_BUFFER2 0x02

//uint16_t PS_MAXNUM=299;          /*指纹模块最大存储指纹模板数*/

/*串口2-USART2*/
#define  AS608_USART                    USART2
#define  AS608_USART_CLK                RCC_APB1Periph_USART2
#define  AS608_USART_APBxClkCmd         RCC_APB1PeriphClockCmd
#define  AS608_USART_BAUDRATE           57600

/*USART GPIO 引脚宏定义*/
#define  AS608_USART_GPIO_CLK           (RCC_APB2Periph_GPIOA)
#define  AS608_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
#define  AS608_USART_TX_GPIO_PORT       GPIOA   
#define  AS608_USART_TX_GPIO_PIN        GPIO_Pin_2
#define  AS608_USART_RX_GPIO_PORT       GPIOA
#define  AS608_USART_RX_GPIO_PIN        GPIO_Pin_3

#define  AS608_USART_IRQ                USART2_IRQn
#define  AS608_USART_IRQHandler         USART2_IRQHandler


/*TouchOut引脚定义*/
#define AS608_TouchOut_INT_GPIO_PORT         GPIOA
#define AS608_TouchOut_INT_GPIO_CLK          (RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO)
#define AS608_TouchOut_INT_GPIO_PIN          GPIO_Pin_4
#define AS608_TouchOut_INT_EXTI_PORTSOURCE   GPIO_PortSourceGPIOA
#define AS608_TouchOut_INT_EXTI_PINSOURCE    GPIO_PinSource4
#define AS608_TouchOut_INT_EXTI_LINE         EXTI_Line4
#define AS608_TouchOut_INT_EXTI_IRQ          EXTI4_IRQn

#define AS608_TouchOut_IRQHandler            EXTI4_IRQHandler

//#define AS_Delay                             rt_thread_delay
#define AS_Delay                               delay_ms


void AS608_Config(void);

uint8_t PS_GenChar(uint8_t BufferID);
uint8_t PS_RegModel(void);
uint8_t PS_StoreChar(uint8_t BufferID,uint16_t PageID);
uint8_t PS_DeletChar(uint16_t PageID,uint16_t N);
uint8_t PS_Empty(void);
uint8_t PS_Enroll(uint16_t *PageID);
uint8_t PS_Identify(uint16_t *PageID);
uint8_t PS_VfyPwd(uint32_t password);
uint8_t PS_LoadChar (uint8_t BufferID,uint16_t PageID);
uint8_t PS_UpChar(uint8_t BufferID,uint8_t *data,uint16_t *byteCount);
uint8_t PS_DownChar(uint8_t BufferID);
void downloadFeatureFile(uint8_t *data,uint16_t byteCount);
#endif


