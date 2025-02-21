#include "../TF/SD.h"

#include "rtt_log.h"
#include "debug_cmd.h"

#include "sdio.h"

volatile uint8_t RxCplt = 0;
volatile uint8_t TxCplt = 0;
volatile uint8_t Errorf = 0;  //错误标准
volatile uint8_t Abortf = 0;  //终止标准

int8_t ReadSDCard(uint8_t *buffer,uint32_t BlockAdd,uint32_t NumberOfBlocks)
{
	uint32_t timeout = 0xfffff;
	HAL_SD_ReadBlocks(&hsd,buffer, BlockAdd, NumberOfBlocks, 0xfffff);
	while(HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER)
	{
		if(--timeout == 0)
			return -1;
	}
	return 0;
}

int8_t WriteSDCard(uint8_t *buffer,uint32_t BlockAdd,uint32_t NumberOfBlocks)
{
	uint32_t timeout = 0xfffff;
	HAL_SD_WriteBlocks(&hsd,buffer, BlockAdd, NumberOfBlocks, 0xfffff);
	while(HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER)
	{
		if(--timeout == 0)
			return -1;
	}
	return 0;

}


int8_t ReadSDCardDMA(uint8_t *buffer,uint32_t BlockAdd,uint32_t NumberOfBlocks)
{
	uint32_t timeout = 0xfffff;
	while(HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER)
	{
		if(--timeout == 0)
			return -1;
	}
	timeout = 0xfffff;
	HAL_SD_ReadBlocks_DMA(&hsd,buffer, BlockAdd, NumberOfBlocks);
	while(RxCplt != 1)
	{
		if(--timeout == 0)
			return -1;
	}
	RxCplt = 0;
	return 0;
}

int8_t WriteSDCardDMA(uint8_t *buffer,uint32_t BlockAdd,uint32_t NumberOfBlocks)
{
	uint32_t timeout = 0xfffff;
	while(HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER)  //等待传输状态
	{
		if(--timeout == 0)
			return -1;
	}
	HAL_SD_WriteBlocks_DMA(&hsd,buffer, BlockAdd, NumberOfBlocks);
	timeout = 0xfffff;
	while(TxCplt != 1)
	{
		if(--timeout == 0)
			return -1;
	}
	TxCplt = 0;
	return 0;

}



void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
	RxCplt = 1;
}
void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
	TxCplt = 1;
}
void HAL_SD_ErrorCallback(SD_HandleTypeDef *hsd)
{
	Errorf = 1;
}
void HAL_SD_AbortCallback(SD_HandleTypeDef *hsd)
{
	Abortf = 1;
}

void SDTest(void)
{
	volatile	uint16_t i=0;
	int8_t res = 0;
	uint8_t buffer[1500] = {0};
	res = ReadSDCardDMA(buffer,0,2);
	app_log("read res = %d  buffer[5]=%d",res,buffer[5]);

	for( i = 0; i<1000 ; i++)
	{
		buffer[i] = i;
	}
	app_log(" buffer[5]=%d",buffer[5]);

	res = WriteSDCardDMA(buffer,0,2);
	for( i = 0; i<1000 ; i++)
	{
		buffer[i] = 0;
	}
	app_log("write %d buffer[5]=%d",res,buffer[5]);
	res = ReadSDCardDMA(buffer,0,2);
	app_log("read res = %d  buffer[5]=%d",res,buffer[5]);

	for( i = 0; i<1000 ; i++)
	{
		buffer[i] = 55;
	}
	res = WriteSDCardDMA(buffer,0,2);
	for( i = 0; i<1000 ; i++)
	{
		buffer[i] = 0;
	}
	res = ReadSDCardDMA(buffer,0,2);
	app_log("read res = %d  buffer[5]=%d",res,buffer[5]);


}
