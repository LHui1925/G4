#ifndef _MD5_H
#define _MD5_H


#include "board.h"
#include "rtthread.h"

void HASH_MD5CH(char *data,uint32_t dataNum,char *result);
void HASH_MD5UCH(uint8_t *data,uint32_t dataNum,char *result);

#endif


