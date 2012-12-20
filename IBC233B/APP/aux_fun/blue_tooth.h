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

typedef enum {
	
	BT_POWER_ON,
	BT_PAIR_MODE,
	BT_PAIRED,
	BT_CONECTING,
	BT_CONECTED,
	BT_DISCONECT,	
	BT_ACK,
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

	BT_CMD_SET_MAX,
}AT_PROMPT_CMD;

void Blue_tooth_hdlr( void );
void Blue_tooth_main(void);
#endif




#endif
