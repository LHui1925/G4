#ifndef _SM4_H
#define _SM4_H


#include "board.h"
#include "rtthread.h"

typedef struct
{
    uint32_t mk[4];
    uint32_t rk[32];
}SM4_Key;

void SM4_SetKey(uint8_t *key,SM4_Key *a);
void SM4_GenRK(SM4_Key *a);
void SM4_EncodeOrDecodeCh(char *data,int16_t dataNum,char *result,int16_t *resNum,SM4_Key key,uint8_t flag);
void SM4_EncodeOrDecodeUCh(uint8_t *data,int16_t dataNum,uint8_t *result,int16_t *resNum,SM4_Key key,uint8_t flag);

#endif

