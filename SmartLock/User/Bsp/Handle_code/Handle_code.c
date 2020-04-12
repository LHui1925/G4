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
unsigned char password[MAX];//指纹录入密码
unsigned char openinig_password[MAX];//开锁密码
unsigned char flash_password[]="12345678";//认证密码
unsigned char sys_password[]="123456";//系统密码

int i=0;//写入标志位（系统密码）
int j=0;//写入标志位（认证密码）

extern FIL fp;// 文件系统句柄
extern FRESULT res;
extern char rData[4096];

extern UINT bw;//多少字节被正常写入
extern UINT br;//多少字节被正常读出

extern int verify_sign;


/******输入一次11，输入认证密码，成功则录取指纹，否则退出****/
/******输入两次11，修改认证密码****/

void handle_FR(void)
	{
		sign=0;
     while(1)
				{
					sign++;
					num=GetKeyVal();
					if(num==11)
					{
						rt_kprintf("请输入旧的录指纹验证密码:\n\n");
						del_str(password);
						GET_str(password);
						
						if(strcmp((const char *)password,(const char *)flash_password)==0)
						{
							rt_kprintf("请输入设置录指纹验证密码\n\n");
							del_str(flash_password);
							GET_str(flash_password);
							
						res = f_open(&fp,"1:认证密码.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);						
						rt_kprintf("\r\nf_open res =%d\n\n",res);
						if(res == FR_OK)
						{
							f_lseek(&fp,0);//配置光标到开头
							res = f_write(&fp,flash_password,sizeof(flash_password),&bw);
							rt_kprintf ("\r\nbw= %d",bw);	
							
							if(res == FR_OK)
							{
								f_lseek(&fp,0);//配置光标到开头
								res = f_read (&fp,rData,f_size(&fp),&br);
								if(res == FR_OK)
									rt_kprintf ("\r\n文件内容：%s br= %d",rData,br);	
							}		
							f_close(&fp);//操作完成，一定要关闭文件
						}
							
							rt_kprintf("flash_password:%s\n\n",flash_password);
							break;
						}
					}
					
					if(sign>1000)
					{
						rt_kprintf("请输入录指纹验证密码:\n\n");
						del_str(password);
						GET_str(password);
						
						if(j==1)
						{
						res = f_open(&fp,"1:认证密码.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);						
						rt_kprintf("\r\nf_open res =%d\n\n",res);
		
							if(res == FR_OK)
							{
								f_lseek(&fp,0);//配置光标到开头
								res = f_read (&fp,flash_password,f_size(&fp),&br);
								if(res == FR_OK)
									rt_kprintf ("文件内容：%s br= %d\n\n",flash_password,br);	
							}		
							f_close(&fp);//操作完成，一定要关闭文件
						}
	
						if(strcmp((const char *)password,(const char *)flash_password)==0)
						{
							del_str(password);
						
							verify_FR();//验证指纹			
					
							if(verify_sign==0)
							{
//							Add_FR();	//录指纹
							}
							break;
						}else
						{
						rt_kprintf ("认证密码输入错误！！！\n\n");
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


/******输入一次13，输入系统密码，成功则解锁，绿灯亮1秒熄灭，否则退出****/
/******输入两次13，修改系统密码****/
void handle_sys(void)
{    sign=0;
     while(1)
				{
					sign++;
					num=GetKeyVal();
					
					if(num==13)
					{
						rt_kprintf("请输入原开锁密码:\n\n");
						del_str(openinig_password);
						GET_str(openinig_password);
						if(strcmp((const char *)openinig_password,(const char *)sys_password)==0)
						{
							rt_kprintf("请设置开锁密码\n\n");
							del_str(sys_password);
							GET_str(sys_password);
							rt_kprintf("sys_password:%s\n\n",sys_password);

						res = f_open(&fp,"1:系统密码.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);						
						rt_kprintf("\r\nf_open res =%d\n\n",res);
						if(res == FR_OK)
						{
							f_lseek(&fp,0);//配置光标到开头
							res = f_write(&fp,sys_password,sizeof(sys_password),&bw);
							rt_kprintf ("\r\nbw= %d",bw);	
							
							if(res == FR_OK)
							{
								f_lseek(&fp,0);//配置光标到开头
								res = f_read (&fp,rData,f_size(&fp),&br);
								if(res == FR_OK)
									rt_kprintf ("\r\n文件内容：%s br= %d",rData,br);	
							}							
							f_close(&fp);//操作完成，一定要关闭文件
						}
			       break;
						}
					}
					
					
					if(sign>1000)
					{
						rt_kprintf("请输入开锁密码:\n\n");
						del_str(openinig_password);
						GET_str(openinig_password);
						
						if(i==1)
						{
						   res = f_open(&fp,"1:系统密码.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
						   rt_kprintf("\r\nf_open res =%d\n\n",res);
								if(res == FR_OK)
								{
									f_lseek(&fp,0);//配置光标到开头
									res = f_read (&fp,sys_password,f_size(&fp),&br);
									rt_kprintf("password:%s\n\n",sys_password);
									if(res == FR_OK)
										rt_kprintf ("\r\n文件内容：%s br= %d\n\n",sys_password,br);	
								}	
								f_close(&fp);//操作完成，一定要关闭文件
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
						rt_kprintf ("系统密码输入错误！！！");
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
