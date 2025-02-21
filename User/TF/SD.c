#include "../TF/SD.h"

#include "rtt_log.h"
#include "debug_cmd.h"

#include "sdio.h"
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


void SDTest(void)
{
	int8_t res = 0;
	uint8_t buffer[512];
	res = ReadSDCard(buffer,0,1);
	app_log("read res = %d  buffer[5]=%d",res,buffer[5]);

	for(uint16_t i = 0; i<512 ; i++)
	{
		buffer[i] = i;
	}
	app_log(" buffer[5]=%d",buffer[5]);

	res = WriteSDCard(buffer,0,1);
	app_log("write %d buffer[5]=%d",res,buffer[5]);
	res = ReadSDCard(buffer,0,1);
	app_log("read res = %d  buffer[5]=%d",res,buffer[5]);


}
