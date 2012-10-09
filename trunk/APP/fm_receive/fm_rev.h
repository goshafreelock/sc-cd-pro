/*--------------------------------------------------------------------------*/
/**@file    fm_rev.h
   @brief   FM接收任务
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#ifndef _FM_REV_
#define _FM_REV_

#define MAX_FRE    1080
#define MIN_FRE	   870
#define MAX_CHANNL 30


#define FM_MAX_CH    20
#define AM_MAX_CH    10

#define PRESET_MASK				0xF0
#define PRESET_OK				0xA0
#define PRESET_ZONE_MASK		0x01

#define PRESET_SAVE_MASK		0x0E
#define PRESET_SAVE_OK			0x0A


#define SAVE_FREQ_MASK			0x80
enum{

	RESET_FM_PRESET,
	RESET_EUR_AM_PRESET,
	RESET_USA_AM_PRESET,
	GET_FM_PRESET_FROM_EPPROM=0xFE,
	GET_AM_PRESET_FROM_EPPROM,
};
#ifdef __C51__
#include "fm_api.h"
#include "typedef.h"
#include "key.h"
#include "disp_if.h"
#include "msgfor_hot.h"
#include "rtc_mode.h"

void fm_radio(void);
void fm_hdlr( void );
bool scan_fre(void);
void dealmsgforfm(u8 msg);
void enter_fm_rev(void);
#endif

typedef enum {

	SEARCH_UP =0,
	SEARCH_DN,
}AUTO_DIR;

#endif
