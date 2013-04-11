/*--------------------------------------------------------------------------*/
/**@file       msgfor_mp3.h
   @brief      文件查找头文件
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#ifndef  _MSGFOR_MP3_
#define  _MSGFOR_MP3_

#include "msgfor_hot.h"
#include "config.h"
#include "uart.h"
#include "lcd.h"
//#include "filesystem.h"

//#include "device.h"
#include "key.h"
#include "disp_if.h"
#include "eq.h"
#include "iic.h"

u16 fs_get_filenum(PLAY_MODE playmode, u8 searchMode);

#endif
