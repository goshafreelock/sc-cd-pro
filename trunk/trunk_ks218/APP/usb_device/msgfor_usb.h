/*--------------------------------------------------------------------------*/
/**@file     msgfor_usb.h
   @brief    usb从机任务头
   @details
   @author
   @date   2010-12-13
   @note
*/
/*----------------------------------------------------------------------------*/

#ifndef _MSGFOR_USB_H_
#define _MSGFOR_USB_H_

#ifdef __C51__
#include "config.h"
#include "uart.h"

#include "key.h"
#include "disp_if.h"
#include "iic.h"
#include "msgfor_hot.h"
#include "rtc_mode.h"

void usb_audio_massstorage(void);
#endif

#define USB_SPEAKER           0x1
#define USB_HID               0x2
#define USB_MASS              0x4

#define USB_DEVICE_CFG        (USB_SPEAKER | USB_HID | USB_MASS)

#define USB_AUDIO_STOP        0x80
#define USB_AUDIO_FASTFORWARD 0x40
#define USB_AUDIO_PREFILE     0x20
#define USB_AUDIO_NEXTFILE    0x10
#define USB_AUDIO_PP          0x08
#define USB_AUDIO_MUTE        0x04
#define USB_AUDIO_VOLDOWN     0x02
#define USB_AUDIO_VOLUP       0x01
#define USB_AUDIO_NONE        0x00


#define VOLUME_UP	0x01
#define VOLUME_DOWN	0x02


#endif
