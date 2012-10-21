/*--------------------------------------------------------------------------*/
/**@file       get_music_file.h
   @brief      文件查找头文件
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/

#ifndef  _MSGFOR_MP3_
#define  _MSGFOR_MP3_

#ifdef __C51__
#include "msgfor_hot.h"
#include "config.h"
#include "uart.h"
#include "lcd.h"
//#include "filesystem.h"

#include "mp3mode.h"
#include "key.h"
#include "disp_if.h"
#include "eq.h"
#include "iic.h"

void get_music_file1(u8 dir);
void get_music_file2(void);
void get_music_file3(void);
bool fs_get_filenum(PLAY_MODE, u8);

#ifdef USE_FOLDER_SELECT_FUNC
void select_folder_file(u8 cmd);
#endif

#endif

#define GET_NEXT_FILE       0
#define GET_PREV_FILE       1
#define GET_PLAY_FILE       2


#endif
