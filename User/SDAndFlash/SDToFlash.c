#include "SDToFlash.h"
#include "ff.h"
#include "rtt_log.h"
#include "debug_cmd.h"
#include "diskio.h"
#include "string.h"
#include "stdio.h"
FATFS fs_sd;    // SD 卡的文件系统对象
FATFS fs_flash; // W25Q512 的文件系统对象
FATFS fs;
FRESULT res;

#define BUFFER_SIZE 512  // 缓冲区大小，可调整
#define MAX_DEPTH 5      // 最大递归深度，可根据栈大小调整

static FRESULT CopyFile(const char* src_path, const char* dst_path);
static FRESULT CopyDirectory(const char* src_dir, const char* dst_dir, int depth);


static FRESULT recursion(const char * path , uint8_t depth);

void SDToFlashTest(void)
{
	// 挂载 SD 卡
	res = f_mount(&fs_sd, "0:", 1);
	app_log("f_mount SD (0:) res = %d\n", res);
	if (res != FR_OK) return;

	// 挂载 Flash
	res = f_mount(&fs_flash, "1:", 1);
	app_log("f_mount Flash (1:) res = %d\n", res);
	if (res != FR_OK) return;
//	CopyFile("1:test.txt", "0:test.txt");
	CopyDirectory("1:test2.txt", "0:test2.txt", 4);
}





 //拷贝单个文件
static FRESULT CopyFile(const char* src_path, const char* dst_path)
{
    FIL fil_src, fil_dst;
    BYTE buffer[BUFFER_SIZE];
    UINT bytes_read, bytes_written;
    FRESULT res;

    res = f_open(&fil_src, src_path, FA_READ);
    if (res != FR_OK)
    {
        app_log("f_open src (%s) failed, res = %d\n", src_path, res);
        return res;
    }

    res = f_open(&fil_dst, dst_path, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK)
    {
        app_log("f_open dst (%s) failed, res = %d\n", dst_path, res);
        f_close(&fil_src);
        return res;
    }

    while (1)
    {
        res = f_read(&fil_src, buffer, BUFFER_SIZE, &bytes_read);
        if (res != FR_OK || bytes_read == 0) break;

        res = f_write(&fil_dst, buffer, bytes_read, &bytes_written);
        if (res != FR_OK || bytes_written != bytes_read)
        {
            app_log("f_write failed, res = %d\n", res);
            break;
        }
    }

    f_close(&fil_src);
    f_close(&fil_dst);

    if (res == FR_OK)
        app_log("Copied file: %s -> %s\n", src_path, dst_path);
    else
        app_log("Copy file failed: %s -> %s, res = %d\n", src_path, dst_path, res);

    return res;
}


//在文件系统中，每个目录（除了根目录）都会包含两个特殊的条目：

// 递归拷贝文件夹，带深度限制
static FRESULT CopyDirectory(const char* src_dir, const char* dst_dir, int depth)
{
    DIR dir;
    FILINFO fno;
    FRESULT res;
    char src_path[256];
    char dst_path[256];

    // 检查递归深度
    if (depth >= MAX_DEPTH)
    {
        app_log("Max depth reached at %s, skipping deeper levels\n", src_dir);
        return FR_OK;  // 达到最大深度，跳过更深层
    }

    // 创建目标文件夹
    res = f_mkdir(dst_dir);   //:x / .. / ..
    if (res != FR_OK && res != FR_EXIST)
    {
        app_log("f_mkdir (%s) failed, res = %d\n", dst_dir, res);
        return res;
    }

    // 打开源文件夹
    res = f_opendir(&dir, src_dir);
    if (res != FR_OK)
    {
        app_log("f_opendir (%s) failed, res = %d\n", src_dir, res);
        return res;
    }

    // 遍历文件夹内容
    while (1)
    {
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == 0) break;

        // 跳过 "." 和 ".."
        if (strcmp(fno.fname, ".") == 0 || strcmp(fno.fname, "..") == 0)
            continue;

        // 构建路径
        snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, fno.fname);
        snprintf(dst_path, sizeof(dst_path), "%s/%s", dst_dir, fno.fname);

        if (fno.fattrib & AM_DIR)  // 子文件夹
        {
            res = CopyDirectory(src_path, dst_path, depth + 1);  // 增加深度
            if (res != FR_OK) break;
        }
        else  // 文件
        {
            res = CopyFile(src_path, dst_path);
            if (res != FR_OK) break;
        }
    }

    f_closedir(&dir);
    return res;
}

//directory == "1:" or "0:"
void TraversalDirectory(const char * path)
{
	DIR dir;
	res = f_mount(&fs_flash, path, 1);  //挂载
	app_log("f_mount  res = %d\n", res);
	recursion(path,0);
}
//
static FRESULT recursion(const char * path , uint8_t depth)
{
	DIR dir;
	FILINFO fno;
	FRESULT res;
	char sub_path[256];

	if(depth >= MAX_DEPTH)   //判断深度
	{
		app_log("depth %s",depth);
		return FR_OK;
	}

	res = f_opendir(&dir, path);  //打开目录
	app_log("f_opendir  res = %d\n", res);
	while(1)
	{
		res = f_readdir(&dir, &fno);   //读当前部分,读完指针往下移
		app_log("f_readdir  res = %d\n", res);
		if(fno.fname[0] == 0 || res != FR_OK) break;  //判断是否最后一个文件

		if(strcmp(fno.fname,".") == 0 || strcmp(fno.fname,"..") == 0)
			continue ;
		snprintf(sub_path,sizeof(sub_path),"%s/%s",path,fno.fname);  //准备下一个目录

		if(fno.fattrib & AM_DIR)
		{
			app_log("DIR %s",fno.fname);
			res = recursion(sub_path,depth+1);
		}
		else
		{
			app_log("FILE %s",fno.fname);
		}

	}
	f_closedir(&dir);
	return res;

}


