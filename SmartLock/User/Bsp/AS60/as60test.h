#ifndef _AS60TEST_H
#define _AS60TEST_H
#include "stm32f10x.h"




uint8_t registerFingerprint(uint16_t *ID);
//uint8_t identifyFingerprint(uint16_t *ID);
uint8_t getFeatureFile(uint16_t ID,uint8_t *data,uint16_t *byteCount);
uint8_t downloadSaveFeatureFile(uint16_t ID,uint8_t *data,uint16_t byteCount);
#endif



