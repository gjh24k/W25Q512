#include "Test1.h"
#include "ff.h"
#include "rtt_log.h"
#include "debug_cmd.h"
#include "diskio.h"
#include "w25q512.h"
#include "string.h"
extern FATFS fs;
FIL fil;

extern FATFS fs_sd;
extern FATFS fs_flash;

FIL fil_sd;
FIL fil_flash;

extern FRESULT res;
char buffer[100] = {0};
UINT num = 0;

void FatfsSDCardAndW25Q512(void)
{

}

void FatfsTest(void)
{

	f_mount(&fs, "1:", 1);

//	UINT bw;
	res = f_open(&fil, "1:test2.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
	app_log("f_open res = %d",res);

//	res = f_lseek(&fil, f_size(&fil));
//	app_log("f_lseek error: %d",res);
//	res =  f_write(&fil, "success2",strlen("success2") , &bw);
//	app_log("f_write res = %d write data num = %d",res,bw);
	res = f_lseek(&fil, 0);
	app_log("f_lseek error: %d",res);
	res = f_read(&fil, buffer, 100, &num);
	app_log("f_read res = %d  /n buffer = %s num = %d",res,buffer,num);


	app_log("f_size(&fil)= %d",f_size(&fil));
	f_close(&fil);
}

void FatfsTestSD(void)
{

	res = f_mount(&fs, "0:", 1);
	app_log("f_mount 0 res = %d",res);

	res = f_open(&fil, "0:SDTest1.txt", FA_OPEN_ALWAYS | 	FA_READ | FA_WRITE);
	app_log("f_open res = %d",res);
//	res = f_lseek(&fil, f_size(&fil));
//	app_log("f_lseek error: %d",res);
//
//	res = f_write(&fil, "SDCardsuccessful", strlen("SDCardsuccessful"), &num);
//	app_log("f_write res = %d write data num = %d",res,num);

	res = f_lseek(&fil, 0);
	res = f_read(&fil, buffer, 100, &num);
	app_log("f_read res = %d  /n buffer = %s num = %d",res,buffer,num);

	f_close(&fil);
}


void FatfsFormat(void)
{
	BYTE work[FF_MAX_SS];
	FRESULT res;

	res = f_mkfs("1:", 0, work, sizeof(work));
	app_log("f_mkfs res = %d",res);


}
void ReadFileCatalogue(void)
{
	FATFS fs;
	DIR dir;
	FILINFO fno;
	FRESULT res;
	res = f_mount(&fs, "1:", 1);
	app_log("f_mount res = %d",res);

	f_opendir(&dir, "1:"); //打开这个路径
	app_log("f_opendir res = %d",res);

	while(1)
	{
		res = f_readdir(&dir, &fno);  //每次调用时，从当前目录位置读取下一个条目
		if(res != FR_OK || fno.fname[0] == 0) break;


		if(fno.fattrib & AM_DIR)
		{
			app_log("[catalogue] %s",fno.fname);
		}
		else
		{
			app_log("[file] %s (%lu)byte",fno.fname,fno.fsize);
		}
	}

	f_closedir(&dir);

}


void LowFatfsTestSD(void)
{
	uint16_t i = 0;
	uint8_t buffer[1500] = {0};


	disk_read(0,buffer,1,2);

	for( i = 0; i<1000 ; i++)
	{
		buffer[i] = i;
	}
	disk_write(0,buffer,1,2);

	for( i = 0; i<1000 ; i++)
	{
		buffer[i] = 0;
	}

	disk_read(0,buffer,1,2);

	for( i = 0; i<1000 ; i++)
	{
		buffer[i] = 55;
	}
	disk_write(0,buffer,1,2);
	for( i = 0; i<1000 ; i++)
	{
		buffer[i] = 0;
	}
	disk_read(0,buffer,1,2);

}

void LowFatfsTest(void)
{
	uint8_t BufferRead[4096] = {0};
	uint8_t BufferWrite[4096] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	//3号驱动盘 ， 写入buffer ， 写入第几个扇区 ， 写入几个扇区
//验证读
	disk_write(1,BufferWrite,3,1);

	disk_read(1,BufferRead,3,1);
	app_log("Fatfs writed read BufferRead :R1=%d R3=%d R7=%d R15=%d R20=%d",
			BufferRead[0],BufferRead[2],BufferRead[6],BufferRead[14],BufferRead[19]);

	for(int i=0; i<20 ; i++)
		BufferRead[i] = 0;

	app_log("clear BufferRead :R1=%d R3=%d R7=%d R15=%d R20=%d",
					BufferRead[0],BufferRead[2],BufferRead[6],BufferRead[14],BufferRead[19]);

	W25Q512Read(4096*3 , BufferRead,4096);

	app_log("W25Q512 BufferRead :R1=%d R3=%d R7=%d R15=%d R20=%d",
				BufferRead[0],BufferRead[2],BufferRead[6],BufferRead[14],BufferRead[19]);

	app_log("------------------------------------------------------------------------------------------------");
	app_log("------------------------------------------------------------------------------------------------");
//验证写
	W25Q512SectorErase(3 * 4096);
	W25Q512Read(4096*3 , BufferRead,4096);
	app_log("SectorErase BufferRead :R1=%d R3=%d R7=%d R15=%d R20=%d",
							BufferRead[0],BufferRead[2],BufferRead[6],BufferRead[14],BufferRead[19]);

	for(int i=0; i<20 ; i++)
		BufferWrite[i] = i*2;

	app_log("BufferWrite :R1=%d R3=%d R7=%d R15=%d R20=%d",
			BufferWrite[0],BufferWrite[2],BufferWrite[6],BufferWrite[14],BufferWrite[19]);

	W25Q512MultipageWrite(4096*3 , BufferWrite,4096);


	disk_read(1,BufferRead,3,1);
	app_log("disk_read W25Q512MultipageWrite BufferRead :R1=%d R3=%d R7=%d R15=%d R20=%d",
			BufferRead[0],BufferRead[2],BufferRead[6],BufferRead[14],BufferRead[19]);

	for(int i=0; i<20 ; i++)
			BufferRead[i] = 0;
		app_log("clear BufferRead :R1=%d R3=%d R7=%d R15=%d R20=%d",
						BufferRead[0],BufferRead[2],BufferRead[6],BufferRead[14],BufferRead[19]);
	W25Q512Read(4096*3 , BufferRead,4096);

	app_log("W25Q512 BufferRead :R1=%d R3=%d R7=%d R15=%d R20=%d",
				BufferRead[0],BufferRead[2],BufferRead[6],BufferRead[14],BufferRead[19]);

}
