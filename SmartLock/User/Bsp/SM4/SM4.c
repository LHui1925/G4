#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "rtthread.h"
#include <string.h>
#include "SM4.h"

//const uint8_t Sbox[256]=
//{
//	0xD6,0x90,0xE9,0xFE,0xCC,0xE1,0x3D,0xB7,0x16,0xB6,0x14,0xC2,0x28,0xFB,0x2C,0x05,
//	0x2B,0x67,0x9A,0x76,0x2A,0xBE,0x04,0xC3,0xAA,0x44,0x13,0x26,0x49,0x86,0x06,0x99,
//	0x9C,0x42,0x50,0xF4,0x91,0xEF,0x98,0x7A,0x33,0x54,0x0B,0x43,0xED,0xCF,0xAC,0x62,
//	0xE4,0xB3,0x1C,0xA9,0xC9,0x08,0xE8,0x95,0x80,0xDF,0x94,0xFA,0x75,0x8F,0x3F,0xA6,
//	0x47,0x07,0xA7,0xFC,0xF3,0x73,0x17,0xBA,0x83,0x59,0x3C,0x19,0xE6,0x85,0x4F,0xA8,
//	0x68,0x6B,0x81,0xB2,0x71,0x64,0xDA,0x8B,0xF8,0xEB,0x0F,0x4B,0x70,0x56,0x9D,0x35,
//	0x1E,0x24,0x0E,0x5E,0x63,0x58,0xD1,0xA2,0x25,0x22,0x7C,0x3B,0x01,0x21,0x78,0x87,
//	0xD4,0x00,0x46,0x57,0x9F,0xD3,0x27,0x52,0x4C,0x36,0x02,0xE7,0xA0,0xC4,0xC8,0x9E,
//	0xEA,0xBF,0x8A,0xD2,0x40,0xC7,0x38,0xB5,0xA3,0xF7,0xF2,0xCE,0xF9,0x61,0x15,0xA1,
//	0xE0,0xAE,0x5D,0xA4,0x9B,0x34,0x1A,0x55,0xAD,0x93,0x32,0x30,0xF5,0x8C,0xB1,0xE3,
//	0x1D,0xF6,0xE2,0x2E,0x82,0x66,0xCA,0x60,0xC0,0x29,0x23,0xAB,0x0D,0x53,0x4E,0x6F,
//	0xD5,0xDB,0x37,0x45,0xDE,0xFD,0x8E,0x2F,0x03,0xFF,0x6A,0x72,0x6D,0x6C,0x5B,0x51,
//	0x8D,0x1B,0xAF,0x92,0xBB,0xDD,0xBC,0x7F,0x11,0xD9,0x5C,0x41,0x1F,0x10,0x5A,0xD8,
//	0x0A,0xC1,0x31,0x88,0xA5,0xCD,0x7B,0xBD,0x2D,0x74,0xD0,0x12,0xB8,0xE5,0xB4,0xB0,
//	0x89,0x69,0x97,0x4A,0x0C,0x96,0x77,0x7E,0x65,0xB9,0xF1,0x09,0xC5,0x6E,0xC6,0x84,
//	0x18,0xF0,0x7D,0xEC,0x3A,0xDC,0x4D,0x20,0x79,0xEE,0x5F,0x3E,0xD7,0xCB,0x39,0x48,
//};

const uint32_t ck[32]=
{
	0x00070E15,0x1C232A31,0x383F464D,0x545B6269,
	0x70777E85,0x8C939AA1,0xA8AFB6BD,0xC4CBD2D9,
	0xE0E7EEF5,0xFC030A11,0x181F262D,0x343B4249,
	0x50575E65,0x6C737A81,0x888F969D,0xA4ABB2B9,
	0xC0C7CED5,0xDCE3EAF1,0xF8FF060D,0x141B2229,
	0x30373E45,0x4C535A61,0x686F767D,0x848B9299,
	0xA0A7AEB5,0xBCC3CAD1,0xD8DFE6ED,0xF4FB0209,
	0x10171E25,0x2C333A41,0x484F565D,0x646B7279,
};

const uint32_t fk[4]={0xA3B1BAC6,0x56AA3350,0x677D9197,0xB27022DC};

/*Ñ­»·×óÒÆ*/
uint32_t lls(uint32_t a,uint8_t b)
{
    uint8_t i;
    uint32_t temp=a,temp1=a;
    i=b%32;
    temp=temp<<i;
    temp1=temp1>>(32-i);
    temp=temp|temp1;
    return temp;
}
//?????
uint32_t SM4_t(uint32_t A)
{
//	uint32_t temp;
//	uint8_t a0,a1,a2,a3;
//	a0=(A>>24)&0xff;
//	a1=(A>>16)&0xff;
//	a2=(A>>8)&0xff;
//	a3=A&0xff;
//	temp=(Sbox[a0]<<24)|
//         (Sbox[a1]<<16)|
//         (Sbox[a2]<<8)|
//         (Sbox[a3]);
//	return temp;
}

//????
uint32_t SM4_L(uint32_t A)
{
	uint32_t temp;
	temp=A^lls(A,2)^lls(A,10)^lls(A,18)^lls(A,24);
	return temp;
}

//????1
uint32_t SM4_L1(uint32_t A)
{
	uint32_t temp;
	temp=A^lls(A,13)^lls(A,23);
	return temp;
}

//????
uint32_t SM4_T(uint32_t A)
{
	return SM4_L(SM4_t(A));
}

//????1
uint32_t SM4_T1(uint32_t A)
{
	return SM4_L1(SM4_t(A));
}

void SM4_SetKey(uint8_t *key,SM4_Key *a)
{
    uint8_t i=0,j=0;
    while(i<16)
    {
        a->mk[j]=key[i];
        a->mk[j]=a->mk[j]<<8;
        i++;
        a->mk[j]=a->mk[j]|key[i];
        a->mk[j]=a->mk[j]<<8;
        i++;
        a->mk[j]=a->mk[j]|key[i];
        a->mk[j]=a->mk[j]<<8;
        i++;
        a->mk[j]=a->mk[j]|key[i];
        i++;
        j++;
    }
}
//?????
void SM4_GenRK(SM4_Key *a)
{
    uint8_t i=0;
	a->rk[0]=a->mk[0]^fk[0];
	a->rk[1]=a->mk[1]^fk[1];
	a->rk[2]=a->mk[2]^fk[2];
	a->rk[3]=a->mk[3]^fk[3];
	while(i<4)
    {
        a->rk[i]=a->rk[i]^SM4_T1((a->rk[(i+1)%4]^a->rk[(i+2)%4]^a->rk[(i+3)%4]^ck[i]));
        i++;
    }
    while(i<32)
    {
        a->rk[i]=a->rk[i-4]^SM4_T1((a->rk[i-3]^a->rk[i-2]^a->rk[i-1]^ck[i]));
        i++;
    }
}
//??,????
void SM4_Encode(uint32_t *x,uint32_t *y,SM4_Key key)
{
    uint32_t temp[5];
    int8_t i,j=0;
    temp[0]=x[0];
    temp[1]=x[1];
    temp[2]=x[2];
    temp[3]=x[3];
    for(i=0;i<32;i++)
    {
        switch(j)
        {
            case 0:
                temp[4]=temp[0]^SM4_T((temp[1]^temp[2]^temp[3]^key.rk[i]));
                j=1;
                break;
            case 1:
                temp[0]=temp[1]^SM4_T((temp[2]^temp[3]^temp[4]^key.rk[i]));
                j=2;
                break;
            case 2:
                temp[1]=temp[2]^SM4_T((temp[3]^temp[4]^temp[0]^key.rk[i]));
                j=3;
                break;
            case 3:
                temp[2]=temp[3]^SM4_T((temp[4]^temp[0]^temp[1]^key.rk[i]));
                j=4;
                break;
            case 4:
                temp[3]=temp[4]^SM4_T((temp[0]^temp[1]^temp[2]^key.rk[i]));
                j=0;
                break;
        }
    }
    y[0]=temp[(j+3)%5];
    y[1]=temp[(j+2)%5];
    y[2]=temp[(j+1)%5];
    y[3]=temp[j];
}
//??,????
void SM4_Decode(uint32_t *x,uint32_t *y,SM4_Key key)
{
    uint32_t temp[5];
    int8_t i,j=0;
    temp[0]=x[0];
    temp[1]=x[1];
    temp[2]=x[2];
    temp[3]=x[3];
    for(i=31;i>=0;i--)
    {
        switch(j)
        {
            case 0:
                temp[4]=temp[0]^SM4_T((temp[1]^temp[2]^temp[3]^key.rk[i]));
                j=1;
                break;
            case 1:
                temp[0]=temp[1]^SM4_T((temp[2]^temp[3]^temp[4]^key.rk[i]));
                j=2;
                break;
            case 2:
                temp[1]=temp[2]^SM4_T((temp[3]^temp[4]^temp[0]^key.rk[i]));
                j=3;
                break;
            case 3:
                temp[2]=temp[3]^SM4_T((temp[4]^temp[0]^temp[1]^key.rk[i]));
                j=4;
                break;
            case 4:
                temp[3]=temp[4]^SM4_T((temp[0]^temp[1]^temp[2]^key.rk[i]));
                j=0;
                break;
        }
    }
    y[0]=temp[(j+3)%5];
    y[1]=temp[(j+2)%5];
    y[2]=temp[(j+1)%5];
    y[3]=temp[j];
}
//flag=0,encode flag=1,decode
void SM4_EncodeOrDecodeCh(char *data,int16_t dataNum,char *result,int16_t *resNum,SM4_Key key,uint8_t flag)
{
    int16_t offset=0,i,j,k=0;
    uint32_t temp[4];
    uint8_t chTemp[16],h;
    while(offset<dataNum)
    {
        i=0;
       while(i<16&&offset<dataNum)
       {
           chTemp[i]=data[offset];
           offset++;
           i++;
       }
       while(i<16)
       {
           chTemp[i]='\0';
           i++;
       }
       h=0;
       for(i=0;i<4;i++)
       {
           for(j=0;j<4;j++)
           {
               temp[i]=temp[i]<<8;
               temp[i]=temp[i]|(uint8_t)chTemp[h];
               h++;
            }
        }
        if(flag==0)
        {
            SM4_Encode(temp,temp,key);
        }else
        {
            SM4_Decode(temp,temp,key);
        }
        k=k+15;
        for(i=3;i>=0;i--)
       {
           for(j=0;j<4;j++)
           {
               result[k]=0xff&temp[i];
               temp[i]=temp[i]>>8;
               k--;
            }
        }
        k=k+17;
    }
    if(flag==0)
    {
        *resNum=k;
    }else
    {
        k--;
        while(result[k]=='\0')
        {
            k--;
        }
        result[k+1]='\0';
        *resNum=k+1;
    }

}

void SM4_EncodeOrDecodeUCh(uint8_t *data,int16_t dataNum,uint8_t *result,int16_t *resNum,SM4_Key key,uint8_t flag)
{
    int16_t offset=0,i,j,k=0;
    uint32_t temp[4];
    uint8_t chTemp[16],h;
    while(offset<dataNum)
    {
        i=0;
       while(i<16&&offset<dataNum)
       {
           chTemp[i]=data[offset];
           offset++;
           i++;
       }
       while(i<16)
       {
           chTemp[i]='\0';
           i++;
       }
       h=0;
       for(i=0;i<4;i++)
       {
           for(j=0;j<4;j++)
           {
               temp[i]=temp[i]<<8;
               temp[i]=temp[i]|chTemp[h];
               h++;
            }
        }
        if(flag==0)
        {
            SM4_Encode(temp,temp,key);
        }else
        {
            SM4_Decode(temp,temp,key);
        }
        k=k+15;
        for(i=3;i>=0;i--)
       {
           for(j=0;j<4;j++)
           {
               result[k]=0xff&temp[i];
               temp[i]=temp[i]>>8;
               k--;
            }
        }
        k=k+17;
    }
    if(flag==0)
    {
        *resNum=k;
    }else
    {
        k--;
        while(result[k]=='\0')
        {
            k--;
        }
        *resNum=k+1;
    }

}

