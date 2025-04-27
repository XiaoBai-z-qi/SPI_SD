#include "SD_Driver.h"
#include <stdio.h>
#include <string.h>
#include "fatfs.h"

FATFS fs;       // FATFS 文件系统对象，用于挂载和管理文件系统
FIL fil;        // FIL 文件对象，用于表示一个打开的文件，进行读写操作
FILINFO fno;    // 文件信息结构体，存储文件的元数据（如文件名、大小、时间戳等）
FRESULT fresult; // FatFs 函数调用的返回结果，用于检查操作是否成功（如打开、读写文件等）
UINT br, bw;    // 无符号整数，分别记录实际读取（br）和写入（bw）的字节数

/**** 存储容量相关变量 *****/
FATFS *pfs;         // 指向 FATFS 文件系统对象的指针，用于获取磁盘信息
DWORD fre_clust;    // 空闲簇（cluster）的数量，文件系统分配存储空间的基本单位
uint32_t total;     // 总存储空间（单位：字节或簇，具体取决于计算方式）
uint32_t free_space; // 剩余可用存储空间（单位：字节或簇）

#define BUFFER_SIZE 128  // 定义缓冲区大小为 128 字节
char buffer[BUFFER_SIZE]; // 通用缓冲区，用于临时存储字符串或文件读写数据
int i=0;

int bufsize (char *buf)
{
	int i=0;
	while (*buf++ != '\0') i++;
	return i;
}

void clear_buffer (void)
{
	for (int i=0; i<BUFFER_SIZE; i++) buffer[i] = '\0';
}
void SD_Test(void)
{
    clear_buffer();
    /*挂载SD卡*/
    fresult = f_mount(&fs, "/", 1);
        if (fresult != FR_OK)
            printf("SD卡挂载失败!!!\r\n");
        else
            printf("SD卡挂载成功!!!\r\n");

    /*计算SD卡容量信息*/
    f_getfree("", &fre_clust, &pfs);

        total = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
        printf("SD卡容量: \t%u\r\n",total);
        
        free_space = (uint32_t)(fre_clust * pfs->csize * 0.5);
        printf("SD卡剩余空间: \t%u\r\n",free_space);

    /*创建并写入文件*/
    // fresult = f_open(&fil, "file1.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
    //     f_puts("你好世接贺岁庵后", &fil);
    // fresult = f_close(&fil);

    fresult = f_open(&fil, "file2.txt", FA_CREATE_ALWAYS | FA_WRITE);
        strcpy(buffer, "哈喽哈喽\r\n");
        fresult = f_write(&fil, buffer, bufsize(buffer), &bw);
    f_close(&fil);

    clear_buffer();

    // /*读取文件*/
    // fresult = f_open(&fil, "file2.txt", FA_READ);
    //     f_read(&fil, buffer, f_size(&fil), &br);
    //     buffer[br] = '\0';  // 添加字符串结束符
    //     printf("%s", buffer);
    // f_close(&fil);

    // clear_buffer();

    /*更新文件内容*/
    fresult = f_open(&fil, "file2.txt", FA_OPEN_EXISTING | FA_READ | FA_WRITE);
    fresult = f_lseek(&fil, f_size(&fil));  // 移动到文件末尾
    strcpy(buffer, "更新文件\r\n");
    fresult = f_write(&fil, buffer, bufsize(buffer), &bw);
    f_close(&fil);

    clear_buffer();
	
	/*读取文件*/
    fresult = f_open(&fil, "file2.txt", FA_READ);
        f_read(&fil, buffer, f_size(&fil), &br);
        printf("%s", buffer);
    f_close(&fil);

    clear_buffer();
	
    /*删除文件和卸载SD卡*/
    fresult = f_unlink("/file1.txt");
  	if (fresult == FR_OK) printf("file1.txt删除成功\r\n");

  	fresult = f_unlink("/file2.txt");
  	if (fresult == FR_OK) printf("file2.txt删除成功\r\n");
		
  	/* Unmount SDCARD */
  	fresult = f_mount(NULL, "/", 1);
  	if (fresult == FR_OK) printf ("SD卡已移除\r\n");
	
}


