/*--------------------------------------------------------------------------*/
/**@file     main.h
   @brief    主流程头文件
   @details
   @author
   @date   2010-12-13
   @note
*/
/*----------------------------------------------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __C51__
#include "config.h"
#include "uart.h"
#include "decode.h"
#include "filesystem.h"
#include "mp3mode.h"
#include "dac.h"
#include "clock.h"

#if 0
enum
{
    SYS_MP3DECODE = 1,
#ifdef USE_CD_MCU_MASTER_FUNC
    SYS_MCU_CD,
#endif
    SYS_FMREV,
    SYS_AUX,
    SYS_RTC,
    SYS_USBDEVICE,
    SYS_IDLE,
};
#endif
#endif

#if RTC_ENABLE
#define MAX_WORK_MODE  SYS_RTC
#else
#define MAX_WORK_MODE  SYS_IDLE
#endif




#endif
