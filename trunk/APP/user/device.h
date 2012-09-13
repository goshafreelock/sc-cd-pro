/*--------------------------------------------------------------------------*/
/**@file    DEVICE.h
   @brief   设备控制接口
   @details
   @author
   @date   2010-12-13
   @note
*/
/*----------------------------------------------------------------------------*/
#ifndef __DEVICE_H__
#define __DEVICE_H__

#ifdef __C51__
#include "config.h"
#include "uart.h"
u8 find_device(u8);
u8 device_init(void);

/*----------------------------------------------------------------------------*/
/**@brief   检测当前活动的设备
   @param   无
   @return  0：当前活动的设备不在线：其他：当前活动的设备
   @note    u8 device_check(void)
*/
/*----------------------------------------------------------------------------*/
u8 device_check(void);
#endif

#define SDMMC           0
#define USB_DISK        1
#define PC_HOST         4
#define MAX_PLAY_DEVICE 2
#define NO_DEVICE       0



#define MAX_DEVICE 2
#endif