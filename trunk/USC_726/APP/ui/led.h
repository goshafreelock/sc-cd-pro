/*--------------------------------------------------------------------------*/
/**@file   led.h
   @brief  LED驱动头文件
   @details
   @author
   @date   2011-9-8
   @note
*/
/*----------------------------------------------------------------------------*/
#include "Custom_config.h"

#ifdef DISP_LED_GPIO_DRV

#ifndef _LED_H_
#define	_LED_H_

#include "config.h"

#ifdef __C51__

#define led_com_init() 	P3DIR &= ~0x1F; P3HD = 0x1f,P3PU|= 0x1f; PWM4CON = 0xdf
#define led_seg_init() 	P1DIR &= ~0x7F

#define COM1   	BIT(0)
#define COM2		BIT(1)
#define COM3		BIT(2)
#define COM4		BIT(3)
#define COM5		BIT(4)

#define LED_A   	0
#define LED_B	1
#define LED_C	2
#define LED_D	3
#define LED_E	4
#define LED_F	5
#define LED_G	6

//for LED0
#if defined(K015_SW005_REC_V001)
#define ICON_AUX	  BIT(LED_A)
#define ICON_FM	      	  BIT(LED_B)//0//BIT(LED_B)
#define ICON_USB	  BIT(LED_C)
#define ICON_SD		  BIT(LED_D)
#define ICON_COL	  BIT(LED_E)
#define ICON_WMA      BIT(LED_F)
#define ICON_MP3	  BIT(LED_G)
#define ICON_WAV	  0
#define ICON_1POINT   0
#define ICON_PLAY	  0//BIT(LED_A)
#define ICON_PAUSE	  0//BIT(LED_B)
#define ICON_REC	  0//BIT(LED_G)

#elif defined(K800_ZX_800_REC_V001)||defined(K646_ZX_308_REC_V001)
#define ICON_AUX	  0//BIT(LED_A)
#define ICON_FM	      BIT(LED_G)//0//BIT(LED_B)
#define ICON_USB	  BIT(LED_C)
#define ICON_SD		  BIT(LED_D)
#define ICON_COL	  BIT(LED_E)
#define ICON_WMA      BIT(LED_F)
#define ICON_MP3	  BIT(LED_F)
#define ICON_WAV	  0
#define ICON_1POINT   0
#define ICON_PLAY	  BIT(LED_A)
#define ICON_PAUSE	  BIT(LED_B)
#define ICON_REC	  0//BIT(LED_G)
#elif defined(K000_CHENGQIANG_REC_V003)
#define ICON_AUX	  0//BIT(LED_A)
#define ICON_FM	      BIT(LED_F)//0//BIT(LED_B)
#define ICON_USB	  BIT(LED_C)
#define ICON_SD		  BIT(LED_D)
#define ICON_COL	  BIT(LED_E)
#define ICON_WMA      BIT(LED_F)
#define ICON_MP3	  //T(LED_G)
#define ICON_WAV	  0
#define ICON_1POINT   0
#define ICON_PLAY	  BIT(LED_A)
#define ICON_PAUSE	  BIT(LED_B)
#define ICON_REC	  BIT(LED_G)//BIT(LED_G)
#elif defined(K000_XL_X15_REC_V001)
#define ICON_AUX	  BIT(LED_A)
#define ICON_FM	      	  BIT(LED_C)//0//BIT(LED_B)
#define ICON_USB	  BIT(LED_E)
#define ICON_SD		  BIT(LED_D)
#define ICON_COL	  BIT(LED_G)
#define ICON_WMA      0//BIT(LED_F)
#define ICON_MP3	  0//BIT(LED_G)
#define ICON_WAV	  0
#define ICON_1POINT   0
#define ICON_PLAY	  0//BIT(LED_A)
#define ICON_PAUSE	  0//BIT(LED_B)
#define ICON_REC	  0//BIT(LED_G)
#define ICON_BAT_H	  BIT(LED_F)
#define ICON_BAT_L     BIT(LED_B)
#elif defined(K1081_WK_1081_REC_V001)
#define ICON_AUX	  BIT(LED_D)
#define ICON_FM	      	  BIT(LED_B)//0//BIT(LED_B)
#define ICON_USB	  BIT(LED_C)
#define ICON_SD		  BIT(LED_A)
#define ICON_COL	  BIT(LED_E)
#define ICON_WMA       0//BIT(LED_F)
#define ICON_MP3	  0//BIT(LED_G)
#define ICON_WAV	  0
#define ICON_1POINT   0
#define ICON_PLAY	  0//BIT(LED_A)
#define ICON_PAUSE	  0//BIT(LED_B)
#define ICON_REC	  0//BIT(LED_G)
#define ICON_BAT_H	  BIT(LED_F)
#define ICON_BAT_L     BIT(LED_G)

#elif defined(K2035_SES_2035_REC_V001)||defined(K2035_SES_2035_REC_V002)
#define ICON_AUX	  0//BIT(LED_A)
#define ICON_FM	      BIT(LED_F)//0//BIT(LED_B)
#define ICON_USB	  BIT(LED_C)
#define ICON_SD		  BIT(LED_D)
#define ICON_COL	  BIT(LED_E)
#define ICON_WMA      BIT(LED_F)
#define ICON_MP3	  0//BIT(LED_G)
#define ICON_REC	  BIT(LED_G)
#define ICON_WAV	  0
#define ICON_1POINT   0
#define ICON_PLAY	  BIT(LED_A)
#define ICON_PAUSE	  BIT(LED_B)
#else
#define ICON_AUX	  0//BIT(LED_A)
#define ICON_FM	      BIT(LED_F)//0//BIT(LED_B)
#define ICON_USB	  BIT(LED_C)
#define ICON_SD		  BIT(LED_D)
#define ICON_COL	  BIT(LED_E)
#define ICON_WMA      BIT(LED_F)
#define ICON_MP3	  BIT(LED_G)
#define ICON_WAV	  0
#define ICON_1POINT   0
#define ICON_PLAY	  BIT(LED_A)
#define ICON_PAUSE	  BIT(LED_B)
#define ICON_REC	  0//BIT(LED_G)

#ifdef USE_BAT_MANAGEMENT

#ifndef ICON_BAT_H
#define ICON_BAT_H	  BIT(LED_F)
#endif

#ifndef ICON_BAT_L
#define ICON_BAT_L	  BIT(LED_G)
#endif
#endif


#endif

enum									//LED 数码管显示ICON定义
{
    LED_MASK_FM_ICON = 0,
    LED_MASK_UDISK_ICON,
    LED_MASK_SD_ICON,
    LED_MASK_MP3_ICON,
    LED_MASK_2DOT_ICON,
    LED_MASK_PLAY_ICON,
    LED_MASK_PAUSE_ICON,	   	//0x0C
			

    LED_MASK_CHAR0 = 0x40,				//LED字符显示定义
    LED_MASK_CHAR1,
    LED_MASK_CHAR2,
    LED_MASK_CHAR3,
    LED_MASK_CHAR4,
    LED_MASK_CHAR5,
    LED_MASK_CHAR6,
    LED_MASK_CHAR7,						//0x47

    LED_FM_ICON = 0x80,
    LED_UDISK_ICON,
    LED_SD_ICON,
    LED_MP3_ICON,
    LED_2DOT_ICON,
    LED_PLAY_ICON,
    LED_PAUSE_ICON,					//0x9C
	  				

    LED_CHAR0 = 0xC0,				//LED字符显示定义
    LED_CHAR1,
    LED_CHAR2,
    LED_CHAR3,
    LED_CHAR4,
    LED_CHAR5,
    LED_CHAR6,
    LED_CHAR7,						//0xC7


    LED_ALL_NO_FLASH = 0xfe,
    LED_ALL_FLASH = 0xFF,
};

void init_disp_buf();
void init_disp(void);
void disp_scan(void);
void disp_putchar(u8 chardata,u8 loc);
void init_led_disp();
void disp_set_brightness(u8 br);
void disp_icon(u8 id);
void disp_clr_icon(u8 id);
void disp_flash_icon(u8 id);
void disp_clr_flash_icon(u8 flag);

#endif


#define LED_COM1   P30
#define LED_COM2   P31
#define LED_COM3   P32
#define LED_COM4   P33
#define LED_COM5   P34

#define LED_COM    P3

#define LED_COM_MASK   P3PU

#define LED_SEG    P1

#define FIGURE_NUM	4

#endif
#endif

