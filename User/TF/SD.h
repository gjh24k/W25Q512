#ifndef SD_H
#define SD_H
#include "stm32f4xx_hal.h"
int8_t ReadSDCard(uint8_t *buffer,uint32_t BlockAdd,uint32_t NumberOfBlocks);
int8_t WriteSDCard(uint8_t *buffer,uint32_t BlockAdd,uint32_t NumberOfBlocks);

void SDTest(void);

#endif
