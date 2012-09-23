/*--------------------------------------------------------------------------*/
/**@file     config.h
   @brief    一些基本参数的配置
   @author
   @date   2010-12-13
   @note   AC209N
*/
/*----------------------------------------------------------------------------*/
#include "Custom_config.h"

#ifndef _CONFIG_
#define _CONFIG_

#ifdef __C51__
#include "AC209N.h"
#include "intrins.h"
#include "typedef.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "entry.h"
#include "power.h"

#define chars_table_l  ((char _code *)(0xc39b))


typedef enum
{
	DIS_SD_CMD_CHK = 0,	///<在命令检测SD卡在线状态的情况下，暂停SD在线检测
	EN_SD_CMD_CHK,		///<在命令检测SD卡在线状态的情况下，恢复SD在线检测
	SET_SD_H_CHK,		///<在命令检测SD卡在线状态的情况下，SD不在线时检测100MS一次
	SET_SD_L_CHK,		///<在命令检测SD卡在线状态的情况下，SD不在线时检测1S一次
}Sd_api_cmd;


typedef enum
{
    SYS_MP3DECODE_USB,
    SYS_MP3DECODE_SD,

#ifdef USE_CD_MCU_MASTER_FUNC
    SYS_MCU_CD,
#endif

#if defined(EXCHANGE_FM_AUX_PRIO)    
    SYS_AUX,
    
    SYS_FMREV,
#ifdef AM_RADIO_FUNC
    SYS_AMREV,
#endif

#else
    SYS_FMREV,
#ifdef AM_RADIO_FUNC
    SYS_AMREV,
#endif
    SYS_AUX,
#endif

#if defined(IPONE_INDEPENDENT_MODE)	
    SYS_IPH,
#endif    

    SYS_RTC,
    SYS_IDLE,
    SYS_USB_DEVICE,
}SYS_WORK_MODE;
#endif
									   

#define OSC_CLOCK		32768L		///<系统晶振选择
#define OTP_MUSIC_ENABLE 0			///<是否需要使用开机音乐

/*0: DECODE EQ, 1: DSP EQ*/
#define DECODE_EQ       0
#define DSP_EQ          1
#define EQ_WORK_USE     DECODE_EQ
/**/


#define  LED_DISP  1
#define  LCD_DISP  0

/*延时显示时间*/
#define RETURN_TIME      6

/*sd检测模式**/
#define SDMMC_CMD_MODE   0	  ///<使用SD card 命令查询模式
#define SDMMC_CLK_MODE   1	  ///<使用SD clk 查询模式
#define SDMMC_GPIO_MODE  0	  ///<使用独立的GPIO做SD在线查询模式

#define FF_FR_MUSIC                      1			//在快进快退时，是否需要听到声音
#define RTC_ENABLE                       0		    //<是否需要使用RTC功能(与是否使用RTC内部的RAM无关)
#define OTP_MUSIC_ENABLE                 0			//<是否需要使用开机音乐
#define VOICE_TIME_ENABLE                0          //语音报时功能使能；     1：打开  0：关闭


#define FILE_ENCRYPTION                  0          //文件加密功能使能;      1：打开  0：关闭
#define DEVICE_ENCRYPTION                0          //设备加密功能使能；     1：打开  0：关闭
#define FAT_MEMORY                       0          //设备记忆功能使能；     1：打开  0：关闭
#endif
