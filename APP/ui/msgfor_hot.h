/*--------------------------------------------------------------------------*/
/**@file    msgfor_hot.h
   @brief   HOT 消息处理
   @details
   @author
   @date   2010-12-13
   @note
*/
/*----------------------------------------------------------------------------*/
#include "Custom_config.h"

#ifndef _MSGFOR_HOT_
#define	_MSGFOR_HOT_

#ifdef __C51__
#include "config.h"
#include "uart.h"
#include "lcd.h"
#include "key.h"
#include "disp_if.h"
enum {

	DISP_NORMAL,
	DISP_RTC_SCEN,
	DISP_ALM_SCEN,
};

typedef enum {
	
	UNMUTE=0,
	MUTE,
}MUTE_TYPE;


void Mute_Ext_PA(bool M_Type);

u8 ap_handle_hotkey(u8 msg);
void rtc_setting_exit(void);
#if defined(USE_TIMER_POWER_OFF_FUNC)
void timer_pwr_off_hdlr();
void timer_pwr_setting();
#endif

#endif

#endif
