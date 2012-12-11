/*--------------------------------------------------------------------------*/
/**@file      filesystem.h
   @brief     �ļ�ϵͳ����ͷ�ļ�
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
    u16     fileCounter;		// �ļ�����,�����ļ�ʱ,���������������ܵ��ļ���
    u16	    fileNumber;			// ��ǰ�ļ����
    u16	    fileTotal;			// �ļ�����
    u16     dirTotal;			// �ļ�������
    u16     fileTotalInDir;     // ��ǰĿ¼�ĸ�����Ч�ļ��ĸ���
    u16     fileTotalOutDir;	// ��ǰĿ¼ǰ���ļ�������Ŀ¼ѭ��ģʽ�£���Ҫ�����������ļ����
    u8	    deepth;				// �ļ����
    u16     file_index;         // �ļ�Ŀ¼�����
    u16     dir_index;          // Ŀ¼Ŀ¼�����
    u8      fname[12];		    // ��ǰ�ļ��Ķ���
    u8      dname[12];		    // ��ǰĿ¼�Ķ���
} FSAPIMSG;

typedef struct _FIL
{
    u8 continue_sector; // �ж��ٸ������Ĵ�
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
	u16	dirNumber;				//����Ч�ļ����ļ������
	u16	dirFileNumber;			//�ļ��ڵ�ǰ�ļ����ڵ����
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