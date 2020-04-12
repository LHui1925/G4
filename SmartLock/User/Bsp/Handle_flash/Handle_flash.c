#include "rtthread.h"   
#include "string.h"
#include "stm32f10x.h"
#include "bsp_spi_flash.h"
#include "ff.h"		
#include "Handle_flash.h"
#include "stdio.h"

FATFS fsObject;
FIL fp;// 文件系统句柄
FRESULT res;


unsigned char wData1[150]="0\0";//存储（认证密码）
unsigned char wData2[150]="0\0";//存储（系统密码）

char rData[4096]="0";

extern int i;//写入标志位（系统密码）
extern int j;//写入标志位（认证密码）
extern unsigned char sys_password[];//系统密码
extern unsigned char flash_password[];//认证密码

UINT bw;//多少字节被正常写入
UINT br;//多少字节被正常读出


void Handle_flash(void)
{
	

		//挂载文件系统
		res = f_mount(&fsObject,"1:",1);//"1:"表示路径   1表示立即挂载

		rt_kprintf("f_mount res =%d\n\n",res);
		
		if(res == FR_NO_FILESYSTEM)
		{
			res = f_mkfs("1:",0,0);//格式化,0自动分配
			rt_kprintf("f_mkfs res =%d\n\n",res);
			//格式化后要取消挂载再重新挂载文件系统
			res = f_mount(NULL,"1:",1);
			res = f_mount(&fsObject,"1:",1);		
			rt_kprintf("second f_mount res =%d",res);
		}

	
//	   res = f_open(&fp,"1:开门密码.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
//		 rt_kprintf("\r\nf_open res =%d\n\n",res);
//     f_unlink("1:开门密码.txt");
	
		
/**********************存储系统密码*********************/
		
	  res = f_open(&fp,"1:系统密码.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
		rt_kprintf("f_open res =%d\n\n",res);
		if(res == FR_OK)
		{
		
//			f_lseek(&fp,0);//配置光标到开头
//			res = f_write(&fp,(char *)0,1,&bw);
//			rt_kprintf ("\r\nbw= %d",bw);	
			
			f_lseek(&fp,0);//配置光标到开头
			res = f_read (&fp,wData2,f_size(&fp),&br);

			if(res == FR_OK)
				rt_kprintf ("文件内容（系统密码）：%s br= %d\n\n",wData2,br);	
//			  rt_kprintf ("\r\n文件内容：%s br= %d\n\n",sizeof(wData2));	

			if(strlen((char *)wData2)<2||strlen((char *)wData2)>15)
			{
				rt_kprintf ("建议修改系统密码，否则使用原始密码！！！\n\n");
			}
			else
			{
				i=1;
				strcpy((char *)sys_password,(char *)wData2);
				
			}
			
		f_close(&fp);//操作完成，一定要关闭文件
//	
	}
		
/**********************存储认证密码*********************/	
	
		res = f_open(&fp,"1:认证密码.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
		rt_kprintf("f_open res =%d\n\n",res);

		if(res == FR_OK)
		{
			f_lseek(&fp,0);//配置光标到开头
			res = f_read (&fp,wData1,f_size(&fp),&br);
			
			if(res == FR_OK)
				rt_kprintf ("文件内容（认证密码）：%s br= %d\n\n",wData1,br);	
			
			if(strlen((char *)wData1)<2||strlen((char *)wData1)>12)
			{
				rt_kprintf ("建议修改认证密码，否则使用原始密码！！！\n\n");
			}
			else
			{
				j=1;
				strcpy((char *)flash_password,(char *)wData1);
				
			}
		}	
		f_close(&fp);//操作完成，一定要关闭文件
	
}
