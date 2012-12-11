/*--------------------------------------------------------------------------*/
/**@file   "disp_if.h"
   @brief    显示模块头文件
   @details
   @author
   @date   2010-12-13
   @note
*/
/*----------------------------------------------------------------------------*/
#ifndef   _DISP_H_
#define  _DISP_H_

#ifdef __C51__
#include "AC209N.h"
#include "typedef.h"
#include "uart.h"
#include "main.h"


typedef enum
{
    DISP_NULL = 0,
    DISP_HELLO,
    DISP_SCAN_DISK,
    DISP_SCAN_TOC,
    DISP_ERROR,
    DISP_OPEN,
    DISP_NUMBER,
    DISP_PLAY,
    DISP_PAUSE,
    DISP_STOP,
    DISP_VOL,
    DISP_EQ,
    DISP_DIR,
    DISP_POWER_UP,
    DISP_POWER_OFF,
    DISP_PWR_TIMER,
    DISP_PLAYMODE,
    DISP_NOFILE,
    DISP_NODEVICE,
    DISP_FILENUM,
    DISP_PROG_FILENUM,    
    DISP_DWORD_NUMBER,
    DISP_WAIT,
    DISP_USBDEV,
    DISP_FREQ,
    DISP_SAVE_POS,
    DISP_SEL_POS,
    DISP_BAND_NUM,
    DISP_USBOUT,
    DISP_SDOUT,
    DISP_AUX,
    DISP_BT,
    DISP_RTC,
    DISP_ALARM,
    DISP_RTC_PWD,
    DISP_ALM_UP,
    DISP_ALM_SET,
};

void disp_init_if(void);

void Disp_Con(u8 LCDinterf);

void disp_file_time(void);

void set_brightness_fade_out(void);
void set_brightness_all_on(void);

#endif

#endif
