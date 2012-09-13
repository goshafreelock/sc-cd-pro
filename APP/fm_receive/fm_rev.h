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
