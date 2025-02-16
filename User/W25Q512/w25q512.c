#include "w25q512.h"
#include "spi.h"
#include "gpio.h"
#include "stm32f4xx_hal.h"
#include "rtt_log.h"
#include "debug_cmd.h"

void Enable_CS(void)
{
	HAL_GPIO_WritePin(GPIOA,  GPIO_PIN_4, GPIO_PIN_RESET);
}

void Disable_CS(void)
{
	HAL_GPIO_WritePin(GPIOA,  GPIO_PIN_4, GPIO_PIN_SET);
}

void W25Q512WaitBusy(void)
{
	uint32_t Timeout = 0xffffffff;
	uint8_t data = W25Q64_Read_Status_register_1;
	Enable_CS();
	HAL_SPI_Transmit(&hspi1, &data, 1, 1000);
	while(--Timeout > 0)
	{
		HAL_SPI_Receive(&hspi1, &data, 1, 1000);
//		app_log("Busy %x",data);
		if((data & 0x01) == 0)
			break;
		else if(Timeout == 1)
			app_log("W25Q512 ERROR");
	}
	Disable_CS();
}

void W25Q512WakeUp(void)
{
	uint8_t data = W25Q64_Erase_Resume;
	Enable_CS();
	HAL_SPI_Transmit(&hspi1, &data, 1, 1000);
	Disable_CS();
}

void W25Q512PowerDown(void)
{
	uint8_t data = W25Q64_Power_down;
	Enable_CS();
	HAL_SPI_Transmit(&hspi1, &data, 1, 1000);
	Disable_CS();
}

void W25Q512WriteEnable(void)
{
	uint8_t data = W25Q64_Write_Enable;
	Enable_CS();
	HAL_SPI_Transmit(&hspi1, &data, 1, 1000);
	Disable_CS();
}

void W25Q512WriteDisable(void)
{
	uint8_t data = W25Q64_Write_Disable;
	Enable_CS();
	HAL_SPI_Transmit(&hspi1, &data, 1, 1000);
	Disable_CS();
}

void W25Q512IDGet(uint8_t *buffer)
{
	uint8_t data = W25Q64_JEDEC_ID;
	Enable_CS();
	HAL_SPI_Transmit(&hspi1, &data, 1, 1000);
	HAL_SPI_Receive(&hspi1, buffer, 3, 1000);
	Disable_CS();
}


uint8_t W25Q512ReadReg3(void)
{
	uint8_t data = W25Q64_Read_Status_register_3;
	Enable_CS();
	HAL_SPI_Transmit(&hspi1, &data, 1, 1000);
	HAL_SPI_Receive(&hspi1, &data, 1, 1000);
	Disable_CS();
	return data;
}

void W25Q512Enter4ByteAddr(void)
{
	uint8_t data = W25Q64_ENTER_4Byte_ADDR;
	Enable_CS();
	HAL_SPI_Transmit(&hspi1, &data, 1, 1000);
	Disable_CS();
}

void W25Q512Exit4ByteAddr(void)
{
	uint8_t data = W25Q64_EXIT_4Byte_ADDR;
	Enable_CS();
	HAL_SPI_Transmit(&hspi1, &data, 1, 1000);
	Disable_CS();
}

void W25Q512Init(void)
{
	uint8_t rxbuff[5]={0,0,0,0,0};
	 W25Q512WakeUp();
	//得到ID
	 W25Q512IDGet(rxbuff);
	 app_log("ID == %x - %x - %x - %x",rxbuff[0],rxbuff[1],rxbuff[2],rxbuff[3]);
	 //配置为4Byte模式
	 W25Q512Enter4ByteAddr();
	 rxbuff[0] = W25Q512ReadReg3();
	 app_log("Status Register 3 %d ",rxbuff[0]);
	 if(rxbuff[0] & 0x0001)
		 app_log("current address is 4 byte");
	 else
		 app_log("current address is 3 byte");
}

void W25Q512SectorErase(uint32_t SectorAddr)
{
	uint8_t data[4] = {0};
	data[0] = W25Q64_Sector_Erase_4KB ;

	W25Q512WriteEnable();
	Enable_CS();
	//send command
	HAL_SPI_Transmit(&hspi1, data, 1, 1000);
	//send address
	data[0] = (SectorAddr & 0xff000000) >> 24;
	data[1] = (SectorAddr & 0xff0000) >> 16;
	data[2] = (SectorAddr & 0xff00) >> 8;
	data[3] = (SectorAddr & 0xff) ;
	HAL_SPI_Transmit(&hspi1, data, 4, 1000);
	Disable_CS();
	W25Q512WriteDisable();

	W25Q512WaitBusy();
}

void W25Q512BlockErase(uint32_t SectorAddr)
{
	uint8_t data[4] = {0};
	data[0] = W25Q64_Block_Erase_64KB ;


	W25Q512WriteEnable();
	Enable_CS();
	//send command
	HAL_SPI_Transmit(&hspi1, data, 1, 1000);
	//send address
	data[0] = (SectorAddr & 0xff000000) >> 24;
	data[1] = (SectorAddr & 0xff0000) >> 16;
	data[2] = (SectorAddr & 0xff00) >> 8;
	data[3] = (SectorAddr & 0xff) ;
	HAL_SPI_Transmit(&hspi1, data, 4, 1000);
	Disable_CS();
	W25Q512WriteDisable();

	W25Q512WaitBusy();
}

void W25Q512ChipErase(void)
{
	uint8_t data = W25Q64_Chip_Erase;


	W25Q512WriteEnable();
	Enable_CS();
	HAL_SPI_Transmit(&hspi1, &data, 1, 1000);
	Disable_CS();

	W25Q512WaitBusy();
}

void W25Q512PageWrite(uint32_t address , uint8_t *buffer, uint16_t size)
{
	uint8_t data[4] = {0};
	data[0] = W25Q64_Page_Program ;


	W25Q512WriteEnable();
	Enable_CS();

	//send command
	HAL_SPI_Transmit(&hspi1, data, 1, 1000);
	//send address
	data[0] = (address & 0xff000000) >> 24;
	data[1] = (address & 0xff0000) >> 16;
	data[2] = (address & 0xff00) >> 8;
	data[3] = (address & 0xff) ;
	HAL_SPI_Transmit(&hspi1, data, 4, 1000);
	//send data
	HAL_SPI_Transmit(&hspi1, buffer, size, 1000);
	Disable_CS();
	W25Q512WriteDisable();

	W25Q512WaitBusy();

}

void W25Q512Read(uint32_t address , uint8_t * buffer,uint16_t size)
{
	uint8_t data[4] = {0};
	data[0] = W25Q64_Read_Data ;


	Enable_CS();

	//send command
	HAL_SPI_Transmit(&hspi1, data, 1, 1000);
	//send address
	data[0] = (address & 0xff000000) >> 24;
	data[1] = (address & 0xff0000) >> 16;
	data[2] = (address & 0xff00) >> 8;
	data[3] = (address & 0xff) ;
	HAL_SPI_Transmit(&hspi1, data, 4, 1000);
	//receive data
	HAL_SPI_Receive(&hspi1, buffer, size, 1000);
	Disable_CS();

//	W25Q512WaitBusy();
}

void W25Q512WriteFree(uint32_t WriteAddr, uint8_t * pBuffer , uint16_t NumByteToWrite)
{
	/* NumOfPage：   计算需要写入的页数；
	 * NumOfSingle： 计算出不满一页时剩余的数据量
	 * Addr：        写入地址与SPI_FLASH_PageSize求余，为0则与页首对齐；
	 * count：       计算前端差多少数据可以与页首对齐；
	 */
	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
	Addr = WriteAddr % SPI_FLASH_PageSize;
	count = SPI_FLASH_PageSize - Addr;  //还差多少满256
	NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;   //要写多少页
	NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;  //剩余的

	/* 情况1：页首对齐  */
	if (Addr == 0)
	{
		/* 情况1.a ：写入首地址与页首地址相同，写入数据 ≤ 256 byte */
		if (NumOfPage == 0)
		{
			W25Q512PageWrite( WriteAddr,pBuffer, NumByteToWrite);
		}
		else /* 情况1.b ：写入首地址与页首地址相同 ,写入数据超过1页 */
		{
			while (NumOfPage--)     /* 将整页数据逐页写完 */
			{
				W25Q512PageWrite( WriteAddr,pBuffer, SPI_FLASH_PageSize);
				WriteAddr +=  SPI_FLASH_PageSize;
				pBuffer += SPI_FLASH_PageSize;
			}
			if(NumOfSingle != 0 )     /* 若尾端仍有数据，将剩余数据写完 */
				W25Q512PageWrite( WriteAddr,pBuffer, NumOfSingle);
		}
	}
	else /* 情况2：页首不对齐  */
	{
		if (NumOfPage == 0) /* 情况2.a ：页首不对齐，写入数据 ≤ 256 byte */
		{
			/* 数据不超过256个，但是跨页，情况可在细分 */
			if (NumOfSingle > count)        /* 数据不超过256，但当首地址当页不能写完 */
			{
				temp = NumOfSingle - count;
				W25Q512PageWrite( WriteAddr,pBuffer, count);  /* 先将首地址页数据写完 */
				WriteAddr +=  count;
				pBuffer += count;
				W25Q512PageWrite( WriteAddr , pBuffer, temp);      /* 下一页数据在写入 */
			}
			else /*数据不超过256个，且首地址当页能将所有数据写完 */
			{
				W25Q512PageWrite(WriteAddr, pBuffer , NumByteToWrite);
			}
		}
		else /* 情况2.b 首地址不对齐，且数据量超256个 */
		{
			NumByteToWrite -= count;
			NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
			NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
			W25Q512PageWrite( WriteAddr,pBuffer, count);
			WriteAddr +=  count;
			pBuffer += count;
			while (NumOfPage--)         /* 先写整页 */
			{
				W25Q512PageWrite( WriteAddr,pBuffer, SPI_FLASH_PageSize);
				WriteAddr +=  SPI_FLASH_PageSize;
				pBuffer += SPI_FLASH_PageSize;
			}
			if (NumOfSingle != 0)       /* 再写多出来的数据 */
			{
				W25Q512PageWrite( WriteAddr,pBuffer, NumOfSingle);
			}
		}
	}
}

void W25Q512Test(void)
{
	uint8_t buffer[4096]={0};
	uint8_t TxBuffer[10] = {1,2,3,4,5,6,77,88,99,100};
//	W25Q512Read(0x3fff000, buffer,4096);
//	app_log("valid buffer[0] = %d  buffer[4000] = %d",buffer[0],buffer[4000]);

	W25Q512SectorErase(0);
	W25Q512PageWrite(0 , TxBuffer, 10);
	W25Q512Read(0, buffer,10);
	app_log("PageWrite buffer[0] = %d  buffer[1] = %d buffer[2] = %d  buffer[3] = %d buffer[7] = %d buffer[9] = %d",buffer[0],buffer[1] ,buffer[2],buffer[3],
																				  buffer[7] ,buffer[9]);
	W25Q512SectorErase(0);
	W25Q512Read(0, buffer,10);
	app_log("SectorErase buffer[0] = %d  buffer[1] = %d buffer[2] = %d  buffer[3] = %d buffer[7] = %d buffer[9] = %d",buffer[0],buffer[1] ,buffer[2],buffer[3],
	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  buffer[7] ,buffer[9]);

	W25Q512PageWrite(0xf0000 , TxBuffer, 10);
	W25Q512Read(0xf0000, buffer,10);
	app_log("PageWrite 0xf0000 buffer[0] = %d  buffer[1] = %d buffer[2] = %d  buffer[3] = %d buffer[7] = %d buffer[9] = %d",buffer[0],buffer[1] ,buffer[2],buffer[3],
																			  buffer[7] ,buffer[9]);

	W25Q512BlockErase(0xf0000 );
	W25Q512Read(0xf0000, buffer,10);
	app_log("BlockErase  buffer[0] = %d  buffer[1] = %d buffer[2] = %d  buffer[3] = %d buffer[7] = %d buffer[9] = %d",buffer[0],buffer[1] ,buffer[2],buffer[3],
			  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  buffer[7] ,buffer[9]);


	W25Q512WriteFree(890, TxBuffer , 10);
	W25Q512Read(890, buffer,10);
	app_log("WriteFree buffer[0] = %d  buffer[1] = %d buffer[2] = %d  buffer[3] = %d buffer[7] = %d buffer[9] = %d",buffer[0],buffer[1] ,buffer[2],buffer[3],
		  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  buffer[7] ,buffer[9]);
	W25Q512ChipErase();
	W25Q512Read(0xf0000, buffer,10);
	app_log("ChipErase buffer[0] = %d  buffer[1] = %d buffer[2] = %d  buffer[3] = %d buffer[7] = %d buffer[9] = %d",buffer[0],buffer[1] ,buffer[2],buffer[3],
																						  buffer[7] ,buffer[9]);

}




