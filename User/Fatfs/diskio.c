/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "w25q512.h"
#include "SD.h"
//#include "stm32f4xx_hal.h"
/* Definitions of physical drive number for each drive */
//#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
//#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
//#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */
#define DEV_SDIO_FLASH 2
#define DEV_SPI_FLASH 3

#define SECTOR_SIZE     4096
/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{

	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{

	switch (pdrv) {

	case DEV_SPI_FLASH :

		W25Q512Init();
			return RES_OK;
	}
	return RES_OK ;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{

	switch (pdrv)
	{

		case DEV_SPI_FLASH :
			uint32_t addr = sector * SECTOR_SIZE;
			for(int i = 0; i < count ; i++)
			{
				W25Q512Read(addr , buff , SECTOR_SIZE);  //SECTOR_SIZE == 4096
				addr += SECTOR_SIZE;
				buff += SECTOR_SIZE;
			}
			return RES_OK;
		case DEV_SDIO_FLASH :
			ReadSDCardDMA(buff,sector,count);
			return RES_OK;
	}


	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{


	switch (pdrv) {

	case DEV_SPI_FLASH :
			uint32_t addr = sector * SECTOR_SIZE;
			for(int i = 0; i < count ; i++)
			{
				W25Q512SectorErase(addr);

//				W25Q512WriteFree(addr , (uint8_t *)buff , SECTOR_SIZE); //SECTOR_SIZE == 4096
				W25Q512MultipageWrite(addr , (uint8_t *)buff,SECTOR_SIZE);
				addr += SECTOR_SIZE;
				buff += SECTOR_SIZE;
			}
			return RES_OK;
	case DEV_SDIO_FLASH :
			WriteSDCardDMA((uint8_t *)buff,sector,count);
			return RES_OK;
	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{

	switch (pdrv)
	{
		case DEV_SPI_FLASH :
			switch(cmd)
			{
			case CTRL_SYNC:
				return RES_OK;
			case GET_SECTOR_COUNT:
				*(DWORD *)buff = 16384;
				return RES_OK;
			case GET_SECTOR_SIZE:
				*(WORD*)buff = SECTOR_SIZE;
				return RES_OK;
			}
		case DEV_SDIO_FLASH :
				switch(cmd)
				{
				case CTRL_SYNC:
					return RES_OK;
				case GET_SECTOR_COUNT:
					*(DWORD *)buff = 29512 * 1024 / 512 * 1024 ;
					return RES_OK;
				case GET_SECTOR_SIZE:
					*(WORD*)buff = 512;
					return RES_OK;
				}


	}

	return RES_PARERR;
}

DWORD get_fattime(void)
{
	return (DWORD)(2022 - 80) << 25 |
			(DWORD)(3+1) << 21 |
			(DWORD)1 << 16 |
			(DWORD)0 << 11 |
			(DWORD)0 << 5 |
			(DWORD)0 >> 1;
}


