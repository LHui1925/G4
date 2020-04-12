#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "rtthread.h"
#include <string.h>
#include "MD5.h"

#define A 0x67452301
#define B 0xEFCDAB89
#define C 0x98BADCFE
#define D 0x10325476

const uint8_t MD5_S[]=
{
     7,12,17,22, 7,12,17,22, 7,12,
    17,22, 7,12,17,22, 5, 9,14,20,
     5, 9,14,20, 5, 9,14,20, 5, 9,
    14,20, 4,11,16,23, 4,11,16,23,
     4,11,16,23, 4,11,16,23, 6,10,
    15,21, 6,10,15,21, 6,10,15,21,
     6,10,15,21
};

const uint32_t MD5_K[]=
{
    0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,0x698098d8,0x8b44f7af,
    0xffff5bb1,0x895cd7be,0x6b901122,0xfd987193,0xa679438e,0x49b40821,0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,
    0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,0xa9e3e905,0xfcefa3f8,
    0x676f02d9,0x8d2a4c8a,0xfffa3942,0x8771f681,0x6d9d6122,0xfde5380c,0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,
    0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,0xf4292244,0x432aff97,
    0xab9423a7,0xfc93a039,0x655b59c3,0x8f0ccc92,0xffeff47d,0x85845dd1,0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,
    0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391
};

uint32_t MD5_F(uint32_t x,uint32_t y,uint32_t z)
{
    return (x&y)|((~x)&z);
}

uint32_t MD5_G(uint32_t x,uint32_t y,uint32_t z)
{
    return (x&z)|(y&(~z));
}

uint32_t MD5_H(uint32_t x,uint32_t y,uint32_t z)
{
    return x^y^z;
}

uint32_t MD5_I(uint32_t x,uint32_t y,uint32_t z)
{
    return y^(x|(~z));
}

uint32_t MD5_LLS(uint32_t x,uint8_t num)
{
    uint8_t number=num%32;
    return (x<<number)|(x>>(32-number));
}


void HASH_MD5CH(char *data,uint32_t dataNum,char *result)
{
    char Fill[128];
    uint8_t FillNum=0,i=0,j=0,FillPoint=0;//填充指针
    uint32_t temp,dataTemp[16];//原文缓冲
    uint32_t dataPoint=0;//原文指针
    uint32_t MD5Temp[4],MD5[4]={A,B,C,D};//存放中间结果
    uint32_t loop;
    //计算需要填充多少字节数据
    FillNum=dataNum%64;
    if(FillNum<=56)
    {
        FillNum=56-FillNum;
    }else
    {
        FillNum=64-FillNum+56;
    }
    //填充数据
    if(FillNum!=0)
    {
        Fill[0]=0x80;
        for(i=1;i<FillNum;i++)
        {
            Fill[i]=0x00;
        }
    }
    temp=dataNum*8;//计算原文有多少位数
		//填充长度，长度务必小于2^32
    for(j=0;j<4;j++)
    {
        Fill[i]=temp&0xff;
        temp=temp>>8;
        i++;
    }
    for(j=0;j<4;j++)
    {
        Fill[i]=0x00;
        i++;
    }
    temp=dataNum+FillNum+8;
    FillNum=FillNum+8;
    loop=temp/64;//计算循环次数
    while(loop>0)
    {
        for(i=0;i<16;i++)
        {
            dataTemp[i]=0;
            for(j=0;j<4;j++)
            {
                if(dataPoint<dataNum)
                {
                    temp=(uint8_t)data[dataPoint];
                    temp=temp<<(j*8);
                    dataTemp[i]=dataTemp[i]|temp;
                    dataPoint++;
                }else if(FillPoint<FillNum)
                {
                    temp=(uint8_t)Fill[FillPoint];
                    temp=temp<<(j*8);
                    dataTemp[i]=dataTemp[i]|temp;
                    FillPoint++;
                }

            }
        }
        for(j=0;j<4;j++)
        {
            MD5Temp[j]=MD5[j];
        }
        for(j=0;j<64;j++)
        {           
            if(j<16)
            {
                temp=MD5_F(MD5[1],MD5[2],MD5[3]);
                MD5[0]=MD5[0]+temp;
                temp=j;
            }else if(j<32)
            {
                temp=MD5_G(MD5[1],MD5[2],MD5[3]);
                MD5[0]=MD5[0]+temp;
                temp=(5*j+1)%16;
            }else if(j<48)
            {
                temp=MD5_H(MD5[1],MD5[2],MD5[3]);
                MD5[0]=MD5[0]+temp;
                temp=(3*j+5)%16;
            }else
            {
                temp=MD5_I(MD5[1],MD5[2],MD5[3]);
                MD5[0]=MD5[0]+temp;
                temp=(7*j)%16;
            }
            MD5[0]=MD5[0]+dataTemp[temp];
            MD5[0]=MD5[0]+MD5_K[j];
            MD5[0]=MD5_LLS(MD5[0],MD5_S[j]);
            MD5[0]=MD5[0]+MD5[1];
            temp=MD5[0];
            MD5[0]=MD5[3];
            MD5[3]=MD5[2];
            MD5[2]=MD5[1];
            MD5[1]=temp;
        }
        for(j=0;j<4;j++)
        {
            MD5[j]=MD5[j]+MD5Temp[j];
        }
        loop--;
    }
    temp=0;
    for(j=0;j<4;j++)
    {
        for(i=0;i<4;i++)
        {
            result[temp]=MD5[j]&0xff;
            MD5[j]=MD5[j]>>8;
            temp++;
        }
    }
}


void HASH_MD5UCH(uint8_t *data,uint32_t dataNum,char *result)
{
    uint8_t Fill[128];
    uint8_t FillNum=0,i=0,j=0,FillPoint=0;//填充指针
    uint32_t temp,dataTemp[16];//原文缓冲
    uint32_t dataPoint=0;//原文指针
    uint32_t MD5Temp[4],MD5[4]={A,B,C,D};//存放中间结果
    uint32_t loop;
    //计算需要填充多少字节数据
    FillNum=dataNum%64;
    if(FillNum<=56)
    {
        FillNum=56-FillNum;
    }else
    {
        FillNum=64-FillNum+56;
    }
    //填充数据
    if(FillNum!=0)
    {
        Fill[0]=0x80;
        for(i=1;i<FillNum;i++)
        {
            Fill[i]=0x00;
        }
    }
    temp=dataNum*8;//计算原文有多少位数
		//填充长度，长度务必小于2^32
    for(j=0;j<4;j++)
    {
        Fill[i]=temp&0xff;
        temp=temp>>8;
        i++;
    }
    for(j=0;j<4;j++)
    {
        Fill[i]=0x00;
        i++;
    }
    temp=dataNum+FillNum+8;
    FillNum=FillNum+8;
    loop=temp/64;//计算循环次数
    while(loop>0)
    {
        for(i=0;i<16;i++)
        {
            dataTemp[i]=0;
            for(j=0;j<4;j++)
            {
                if(dataPoint<dataNum)
                {
                    temp=data[dataPoint];
                    temp=temp<<(j*8);
                    dataTemp[i]=dataTemp[i]|temp;
                    dataPoint++;
                }else if(FillPoint<FillNum)
                {
                    temp=Fill[FillPoint];
                    temp=temp<<(j*8);
                    dataTemp[i]=dataTemp[i]|temp;
                    FillPoint++;
                }

            }
        }
        for(j=0;j<4;j++)
        {
            MD5Temp[j]=MD5[j];
        }
        for(j=0;j<64;j++)
        {           
            if(j<16)
            {
                temp=MD5_F(MD5[1],MD5[2],MD5[3]);
                MD5[0]=MD5[0]+temp;
                temp=j;
            }else if(j<32)
            {
                temp=MD5_G(MD5[1],MD5[2],MD5[3]);
                MD5[0]=MD5[0]+temp;
                temp=(5*j+1)%16;
            }else if(j<48)
            {
                temp=MD5_H(MD5[1],MD5[2],MD5[3]);
                MD5[0]=MD5[0]+temp;
                temp=(3*j+5)%16;
            }else
            {
                temp=MD5_I(MD5[1],MD5[2],MD5[3]);
                MD5[0]=MD5[0]+temp;
                temp=(7*j)%16;
            }
            MD5[0]=MD5[0]+dataTemp[temp];
            MD5[0]=MD5[0]+MD5_K[j];
            MD5[0]=MD5_LLS(MD5[0],MD5_S[j]);
            MD5[0]=MD5[0]+MD5[1];
            temp=MD5[0];
            MD5[0]=MD5[3];
            MD5[3]=MD5[2];
            MD5[2]=MD5[1];
            MD5[1]=temp;
        }
        for(j=0;j<4;j++)
        {
            MD5[j]=MD5[j]+MD5Temp[j];
        }
        loop--;
    }
    temp=0;
    for(j=0;j<4;j++)
    {
        for(i=0;i<4;i++)
        {
            result[temp]=MD5[j]&0xff;
            MD5[j]=MD5[j]>>8;
            temp++;
        }
    }
}


