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

#define MAX_BAND    12


void get_channl(u8 flag);
void init_fm_rev(void);
bool set_fre(u16 fre, u8 mode);
void fm_rev_powerdown(void);
void save_fre(u16 fre);
#endif

#endif
