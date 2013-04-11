/*--------------------------------------------------------------------------*/
/**@file    power.h
   @brief   电源管理头文件
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#ifndef	_POWER_H_
#define _POWER_H_

#include "config.h"
#include "uart.h"
#include "decode.h"
#include "filesystem.h"
#include "dac.h"
#include "clock.h"

#ifdef __C51__

//#define power_ctl(n)  P3DIR &= ~(1<<0);P30 = n

void sys_sleep_mode();
void sys_restore_mode();


void system_sleep(void);
void sys_clock_rc(void);
void sys_clock_pll(void);
void adc_clock(void);
void rc_pll_delay(void);
void sys_power_down(void);
void sys_power_up(void);
void waiting_power_key();
#endif

#endif
