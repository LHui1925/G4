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
			case 0x00://��Ϣ����ʧ��
				LED_RED
				rt_thread_delay(600);
				LED_RGBOFF
				break;
			case 0x01://��Ϣ���ͳɹ�
				LED_GREEN
				rt_thread_delay(600);
				LED_RGBOFF
				break;
			case 0x02://�ɹ����ӷ�����
//				LEDWEB_ON
				break;
			case 0x03://������ʧȥ����
//				LEDWEB_OFF
				break;
			case 0x04://������ʾ
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


