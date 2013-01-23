/*--------------------------------------------------------------------------*/
/**@file     blue_tooth.h
   @brief    blue_toothÈÎÎñÄ£¿é
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#include "Custom_config.h"

#ifndef _BLUE_TOOTH_H_
#define _BLUE_TOOTH_H_



#ifdef __C51__

#include "typedef.h"
#include "key.h"
#include "disp_if.h"
#include "msgfor_hot.h"
#include "rtc_mode.h"

enum {
	
	BT_STA_STOP,
	BT_STA_PAUSE,
	BT_STA_PLAY,	
	BT_STA_MASK=0x80,	
};


typedef enum {
	
	BT_POWER_ON,
	BT_PAIR_MODE,
	BT_PAIRED,
	BT_CONECTED_A2DP,
	BT_CONECTED_AVRCP,
	BT_DISCONECT_A2DP,	
	BT_DISCONECT_AVRCP,	
	BT_ACK,
	BT_DEV_ISSUE_PLAY,
	BT_STATUS_MAX,
}AT_REV_CMD;

typedef enum {
	
	BT_PLAY,
	BT_NEXT,
	BT_PREV,
	BT_VOL_P,
	BT_VOL_M,	
	BT_FAST_FARWORD,
	BT_REWIND,
	BT_STOP,
	BT_DISPAIR,
	BT_ENTER_PAIRING_MODE,
	BT_FAST_PAIRING_MODE,

	BT_CMD_SET_MAX,
}AT_PROMPT_CMD;

void Blue_tooth_hdlr( void );
void Blue_tooth_main(void);
#endif




#endif
