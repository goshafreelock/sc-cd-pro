/*--------------------------------------------------------------------------*/
/**@file     fm_api.h
   @brief    FM任务应用接口头文件
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#ifndef _FM_API_H_
#define _FM_API_H_


#ifdef __C51__
#include "rda5807.h"
#include "typedef.h"
#include "disp_if.h"
#include "fm_rev.h"

typedef struct{
	u16	MIN_FREQ;
	u16 	MAX_FREQ;
}FREQ_RAGE;

#define FM_CUR_FRE          0
#define FM_FRE_DEC          1
#define FM_FRE_INC			2


#define SHOW_FREQ		1
#define NO_SHOW_FREQ	0

#define MAX_BAND    12

bool radio_dev_init(void);
void radio_rev_standby(void);
void radio_force_st_mono(void);
void save_fre(u16 fre);
#endif

#endif
