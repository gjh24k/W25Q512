#include "SD.h"

#include "rtt_log.h"
#include "debug_cmd.h"
#include "stdio.h"
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

void Check_SD_Capacity(void)
{
    HAL_SD_CardInfoTypeDef SDCardInfo;  // 定义 SD 卡信息结构体
    char buffer[50];  // 用于存储字符串

    // 确保 SD 卡已初始化
    if (HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER)
    {
    	app_log("SD Card is not ready!\n");
        return;
    }

    // 获取 SD 卡信息
    if (HAL_SD_GetCardInfo(&hsd, &SDCardInfo) != HAL_OK)
    {
    	app_log("Failed to get SD card info!\n");
        return;
    }

    // 计算总容量（以字节为单位）
    uint64_t capacity = (uint64_t)SDCardInfo.BlockNbr * (uint64_t)SDCardInfo.BlockSize;

    // 输出基本信息
    sprintf(buffer, "Block Size: %lu bytes\n", SDCardInfo.BlockSize);
    app_log(buffer);
    sprintf(buffer, "Block Number: %lu\n", SDCardInfo.BlockNbr);
    app_log(buffer);
    sprintf(buffer, "Total Capacity: %llu bytes\n", capacity);
    app_log(buffer);

    // 转换为 MB（整数部分和小数部分分开）
    uint32_t capacity_MB = capacity / (1024 * 1024);  // 整数部分（MB）
    uint32_t capacity_MB_remainder = ((capacity % (1024 * 1024)) * 100) / (1024 * 1024);  // 小数部分（两位精度）
    sprintf(buffer, "Total Capacity: %lu.%02lu MB\n", capacity_MB, capacity_MB_remainder);
    app_log(buffer);

    // 转换为 GB（整数部分和小数部分分开）
    uint32_t capacity_GB = capacity / (1024 * 1024 * 1024);  // 整数部分（GB）
    uint32_t capacity_GB_remainder = ((capacity % (1024 * 1024 * 1024)) * 100) / (1024 * 1024 * 1024);  // 小数部分（两位精度）
    sprintf(buffer, "Total Capacity: %lu.%02lu GB\n", capacity_GB, capacity_GB_remainder);
    app_log(buffer);
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
