#include "play.h"
#include "rtthread.h"
#include "board.h"
#include "bsp_led.h"   

rt_mq_t play_mq=RT_NULL;
rt_thread_t play_thread=RT_NULL;
void play_entry(void *parameter)
{
	rt_err_t err;
	char temp;
	while(1)
	{
		err=rt_mq_recv(play_mq,&temp,1,RT_WAITING_FOREVER);
		if(err!=RT_EOK)
		{
			continue;
		}
		switch(temp)
		{
			case 0x00://消息发送失败
				LED_RED
				rt_thread_delay(600);
				LED_RGBOFF
				break;
			case 0x01://消息发送成功
				LED_GREEN
				rt_thread_delay(600);
				LED_RGBOFF
				break;
			case 0x02://成功连接服务器
//				LEDWEB_ON
				break;
			case 0x03://服务器失去连接
//				LEDWEB_OFF
				break;
			case 0x04://输入提示
//				LEDTIP_ON
				rt_thread_delay(600);
//				LEDTIP_OFF
				break;
			case 0x05://
				LED_BLUE
				rt_thread_delay(300);
				LED_RGBOFF
				break;
			case 0x06://
				LED_PURPLE
				rt_thread_delay(300);
				LED_RGBOFF
				break;
		}
	}
	
}


