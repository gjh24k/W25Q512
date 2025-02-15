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
	uint16_t Timeout = 0xffff;
	uint8_t data = W25Q64_Read_Status_register_1;
	Enable_CS();
	HAL_SPI_Transmit(&hspi1, &data, 1, 1000);
	while(--Timeout > 0)
	{
		HAL_SPI_Receive(&hspi1, &data, 1, 1000);
		if((data & 0x01) == 0)
			break;
		else if(Timeout == 1)
			app_log("W25Q512 ERROR");
	}
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

void W25Q64Init(void)
{
	uint8_t rxbuff[5]={0,0,0,0,0};

	//得到ID
	 W25Q512IDGet(rxbuff);
	 app_log("ID == %x - %x - %x - %x",rxbuff[0],rxbuff[1],rxbuff[2],rxbuff[3]);
	 //配置为4Byte模式
	 W25Q512Enter4ByteAddr();
	 rxbuff[0] = W25Q512ReadReg3();
	 app_log("Status Register 3 %d ",rxbuff[0]);
}

void W25Q64SectorErase(uint32_t SectorAddr)
{
	uint8_t data[4] = {0};
	data[0] = W25Q64_Sector_Erase_4KB ;
	W25Q512WaitBusy();

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
}

void W25Q64PageWrite(uint32_t address , uint8_t *buffer, uint16_t size)
{
	uint8_t data[4] = {0};
	data[0] = W25Q64_Page_Program ;

	W25Q512WaitBusy();

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
}

void W25Q64Read(uint32_t address , uint8_t * buffer,uint16_t size)
{
	uint8_t data[4] = {0};
	data[0] = W25Q64_Read_Data ;

	W25Q512WaitBusy();
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

}
