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
#if 1
typedef enum{

	FM_DEV,
#ifdef AM_RADIO_FUNC
    	AM_DEV,
#endif    	
	
#ifdef USE_CD_MCU_MASTER_FUNC
	CD_DEV,
#endif
	USB_DEV,
	SD_DEV,


	AUX_DEV,

#ifdef USE_BLUE_TOOTH_FUNC			
	BT_DEV,
#endif

#if defined(IPONE_INDEPENDENT_MODE)	
	IPH_DEV,
#endif	

#if (RTC_ENABLE)
	RTC_DEV,
#endif	
	IDLE,
	
	PC_DEV,
}DEV_ON_LIST;
#else
typedef enum{

	USB_DEV,
	SD_DEV,
#ifdef USE_CD_MCU_MASTER_FUNC
	CD_DEV,
#endif
#if defined(EXCHANGE_FM_AUX_PRIO)    
	AUX_DEV,
	FM_DEV,
#ifdef AM_RADIO_FUNC
    	AM_DEV,
#endif    		
#else	
	FM_DEV,
#ifdef AM_RADIO_FUNC
    	AM_DEV,
#endif    	
	AUX_DEV,
#endif	
#if defined(IPONE_INDEPENDENT_MODE)	
	IPH_DEV,
#endif	

#if (RTC_ENABLE)
	RTC_DEV,
#endif	
	IDLE,
	
	PC_DEV,
}DEV_ON_LIST;
#endif
void Init_Func_List();
void Set_Curr_Func(u8 curr_mod);
void Remov_Func_From_List(DEV_ON_LIST Dev);
void set_sys_vol(u8 vol);

void Mute_Ext_PA(bool M_Type);

void aux_channel_crosstalk_improve(u8 ch_num);
void erp_2_test_mode_handlr(void);

u8 ap_handle_hotkey(u8 msg);
void rtc_setting_exit(void);
#if defined(USE_TIMER_POWER_OFF_FUNC)
void timer_pwr_off_hdlr();
void timer_pwr_setting();
#endif

#endif

#endif
