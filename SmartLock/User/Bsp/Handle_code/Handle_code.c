#include "board.h"
#include "rtthread.h"
#include "key.h"
#include "Handle_code.h"
#include "Handle_as608.h"
#include "string.h"
#include "delay.h"
#include "AS608.h"

#include "bsp_spi_flash.h"
#include "ff.h"		
#include "Handle_flash.h"

#define MAX 100
extern u16 num;
u16 code;
int sign=0;
unsigned char password[MAX];//ָ��¼������
unsigned char openinig_password[MAX];//��������
unsigned char flash_password[]="12345678";//��֤����
unsigned char sys_password[]="123456";//ϵͳ����

int i=0;//д���־λ��ϵͳ���룩
int j=0;//д���־λ����֤���룩

extern FIL fp;// �ļ�ϵͳ���
extern FRESULT res;
extern char rData[4096];

extern UINT bw;//�����ֽڱ�����д��
extern UINT br;//�����ֽڱ���������

extern int verify_sign;


/******����һ��11��������֤���룬�ɹ���¼ȡָ�ƣ������˳�****/
/******��������11���޸���֤����****/

void handle_FR(void)
	{
		sign=0;
     while(1)
				{
					sign++;
					num=GetKeyVal();
					if(num==11)
					{
						rt_kprintf("������ɵ�¼ָ����֤����:\n\n");
						del_str(password);
						GET_str(password);
						
						if(strcmp((const char *)password,(const char *)flash_password)==0)
						{
							rt_kprintf("����������¼ָ����֤����\n\n");
							del_str(flash_password);
							GET_str(flash_password);
							
						res = f_open(&fp,"1:��֤����.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);						
						rt_kprintf("\r\nf_open res =%d\n\n",res);
						if(res == FR_OK)
						{
							f_lseek(&fp,0);//���ù�굽��ͷ
							res = f_write(&fp,flash_password,sizeof(flash_password),&bw);
							rt_kprintf ("\r\nbw= %d",bw);	
							
							if(res == FR_OK)
							{
								f_lseek(&fp,0);//���ù�굽��ͷ
								res = f_read (&fp,rData,f_size(&fp),&br);
								if(res == FR_OK)
									rt_kprintf ("\r\n�ļ����ݣ�%s br= %d",rData,br);	
							}		
							f_close(&fp);//������ɣ�һ��Ҫ�ر��ļ�
						}
							
							rt_kprintf("flash_password:%s\n\n",flash_password);
							break;
						}
					}
					
					if(sign>1000)
					{
						rt_kprintf("������¼ָ����֤����:\n\n");
						del_str(password);
						GET_str(password);
						
						if(j==1)
						{
						res = f_open(&fp,"1:��֤����.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);						
						rt_kprintf("\r\nf_open res =%d\n\n",res);
		
							if(res == FR_OK)
							{
								f_lseek(&fp,0);//���ù�굽��ͷ
								res = f_read (&fp,flash_password,f_size(&fp),&br);
								if(res == FR_OK)
									rt_kprintf ("�ļ����ݣ�%s br= %d\n\n",flash_password,br);	
							}		
							f_close(&fp);//������ɣ�һ��Ҫ�ر��ļ�
						}
	
						if(strcmp((const char *)password,(const char *)flash_password)==0)
						{
							del_str(password);
						
							verify_FR();//��ָ֤��			
					
							if(verify_sign==0)
							{
//							Add_FR();	//¼ָ��
							}
							break;
						}else
						{
						rt_kprintf ("��֤����������󣡣���\n\n");
						break;
						}
					}

//					if(sign>=2000)
//					{
//						sign=0;
//						break;
//					}
					delay_ms(1);
				}
}


/******����һ��13������ϵͳ���룬�ɹ���������̵���1��Ϩ�𣬷����˳�****/
/******��������13���޸�ϵͳ����****/
void handle_sys(void)
{    sign=0;
     while(1)
				{
					sign++;
					num=GetKeyVal();
					
					if(num==13)
					{
						rt_kprintf("������ԭ��������:\n\n");
						del_str(openinig_password);
						GET_str(openinig_password);
						if(strcmp((const char *)openinig_password,(const char *)sys_password)==0)
						{
							rt_kprintf("�����ÿ�������\n\n");
							del_str(sys_password);
							GET_str(sys_password);
							rt_kprintf("sys_password:%s\n\n",sys_password);

						res = f_open(&fp,"1:ϵͳ����.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);						
						rt_kprintf("\r\nf_open res =%d\n\n",res);
						if(res == FR_OK)
						{
							f_lseek(&fp,0);//���ù�굽��ͷ
							res = f_write(&fp,sys_password,sizeof(sys_password),&bw);
							rt_kprintf ("\r\nbw= %d",bw);	
							
							if(res == FR_OK)
							{
								f_lseek(&fp,0);//���ù�굽��ͷ
								res = f_read (&fp,rData,f_size(&fp),&br);
								if(res == FR_OK)
									rt_kprintf ("\r\n�ļ����ݣ�%s br= %d",rData,br);	
							}							
							f_close(&fp);//������ɣ�һ��Ҫ�ر��ļ�
						}
			       break;
						}
					}
					
					
					if(sign>1000)
					{
						rt_kprintf("�����뿪������:\n\n");
						del_str(openinig_password);
						GET_str(openinig_password);
						
						if(i==1)
						{
						   res = f_open(&fp,"1:ϵͳ����.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
						   rt_kprintf("\r\nf_open res =%d\n\n",res);
								if(res == FR_OK)
								{
									f_lseek(&fp,0);//���ù�굽��ͷ
									res = f_read (&fp,sys_password,f_size(&fp),&br);
									rt_kprintf("password:%s\n\n",sys_password);
									if(res == FR_OK)
										rt_kprintf ("\r\n�ļ����ݣ�%s br= %d\n\n",sys_password,br);	
								}	
								f_close(&fp);//������ɣ�һ��Ҫ�ر��ļ�
							}
					      							
						if(strcmp((const char *)openinig_password,(const char *)sys_password)==0)
						{
							del_str(openinig_password);
							LED2_ON;
							delay_ms(1000);
							LED2_OFF;
							break;
						}
						else{
						rt_kprintf ("ϵͳ����������󣡣���");
						break;
						}
					}
//					if(sign>=2000)
//					{
//						sign=0;
//						break;
//					}
					delay_ms(1);
				}
}
