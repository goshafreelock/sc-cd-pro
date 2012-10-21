/*--------------------------------------------------------------------------*/
/**@file     mcu_master.h
   @brief    mcu_master ����ģ��
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#ifndef _MCU_MASTER_H_
#define _MCU_MASTER_H_

#include "typedef.h"
#include "key.h"
#include "disp_if.h"
#include "msgfor_hot.h"
#include "rtc_mode.h"

#ifdef __C51__
typedef struct
{
    u8 MIN;
    u8 SEC;
}TOC_TIME;

void mcu_main_hdlr( void );
void mcu_hdlr(void);
#endif






#endif
