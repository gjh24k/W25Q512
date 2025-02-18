#include "Test1.h"
#include "ff.h"
#include "rtt_log.h"
#include "debug_cmd.h"
#include "diskio.h"
#include "w25q512.h"

void FatfsTest(void)
{
	static FATFS fs;
	static FIL fil;
	static BYTE work[FF_MAX_SS];

	FRESULT res ;
//	res = f_mount(&fs,"3:",1);
//	if(res == FR_NO_FILESYSTEM)
//	{
		res = f_mkfs("3:",0,work,sizeof(work));
		app_log("wait mkfs");
		if(res)
		{
			app_log("forma fail");
		}
		else
		{
			app_log("forma successful");
		}
		res = f_mount(&fs,"3:",1);
		if(res)
		{
			app_log("fail fatfs system to mount  one. res = %d",res);
		}
		else
		{
			app_log("success fatfs system to mount  one. res = %d",res);
		}
//	}
//	else if(res)
//	{
//		app_log("fail fatfs system to mount  two  res = %d",res);
//	}


}

void LowFatfsTest(void)
{
	uint8_t BufferRead[20] = {0};
	uint8_t BufferWrite[15] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	//3号驱动盘 ， 写入buffer ， 写入第几个扇区 ， 写入几个扇区
	disk_write(3,BufferWrite,3,1);

	disk_read(3,BufferRead,3,1);
	app_log("Fatfs writed BufferRead :R1=%d R3=%d R7=%d R15=%d R20=%d",
			BufferRead[0],BufferRead[2],BufferRead[6],BufferRead[14],BufferRead[19]);

	for(int i=0; i<20 ; i++)
		BufferRead[i] = 0;

	app_log("clear BufferRead :R1=%d R3=%d R7=%d R15=%d R20=%d",
					BufferRead[0],BufferRead[2],BufferRead[6],BufferRead[14],BufferRead[19]);

	W25Q512Read(4096*3 , BufferRead,20);

	app_log("W25Q512 BufferRead :R1=%d R3=%d R7=%d R15=%d R20=%d",
				BufferRead[0],BufferRead[2],BufferRead[6],BufferRead[14],BufferRead[19]);



}
