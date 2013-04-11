/*--------------------------------------------------------------------------*/
/**@file      filesystem.h
   @brief     文件系统部分头文件
   @details
   @author
   @date   2010-12-13
   @note
*/
/*----------------------------------------------------------------------------*/
#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#ifdef __C51__
#include "config.h"

#define FAT_MEMORY_SIZE       69
#define DEVICE_COMPARE        67
#define FAT_MEMORY_VERIRY     65
typedef struct _FSAPIMSG
{
    u16     fileCounter;		// 文件计数,搜索文件时,可以用它来计算总的文件数
    u16	    fileNumber;			// 当前文件序号
    u16	    fileTotal;			// 文件总数
    u16     dirTotal;			// 文件夹总数
    u16     fileTotalInDir;     // 当前目录的根下有效文件的个数
    u16     fileTotalOutDir;	// 当前目录前的文件总数，目录循环模式下，需要用它来计算文件序号
    u8	    deepth;				// 文件深度
    u16     file_index;         // 文件目录项序号
    u16     dir_index;          // 目录目录项序号
    u8      fname[12];		    // 当前文件的短名
    u8      dname[12];		    // 当前目录的短名
} FSAPIMSG;

typedef struct _FIL
{
    u8 continue_sector; // 有多少个连续的簇
    u8      flag;
    u8 fattrib;
    u32  org_clust; // File start cluster
    u32  curr_clust; // Current cluster
    u32 curr_sect; // Current sector
    u32 fptr; // File R/W pointer
    u32 fsize; // File size
} FIL;



typedef	struct _FSINFO
{
	u16	dirNumber;				//有有效文件的文件夹序号
	u16	dirFileNumber;			//文件在当前文件夹内的序号
} FSINFO;

enum
{
	FIND_NEXT_DIR = 0,
	FIND_PREV_DIR,
	FIND_NEXT_ROOT_DIR,
};

u16 get_dir_file(u8 cmd);

#endif

#endif