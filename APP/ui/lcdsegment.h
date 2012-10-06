#include "Custom_config.h"
#include "config.h"
#ifdef DISP_LCD_GPIO_DRV

#ifndef _LCDSEGMENT_H_
#define _LCDSEGMENT_H_

/*
COM1 P30
COM2 P31
COM3 P32
COM4 P33
SEG0~7   P10~P17
SEG8     P34
*/
#define 	DIG_A	0x01
#define 	DIG_B	0x02
#define 	DIG_C	0x04
#define 	DIG_D	0x08
#define 	DIG_E	0x10
#define 	DIG_F	0x20
#define 	DIG_G	0x40
#define 	DIG_H	0x80
#ifdef __C51__


#if defined(BK_DISP_LCD_DRV)
#define   com_init()      	P3PU |= (0x1F);P3PD |= (0x1F)
#define   close_com(n)    	com_init();P3DIR |= (0x1F);P3DIR &= ~(1<<(n))
    
#define	set_com(n)      P3 |= (1<<(n))
#define   clr_com(n)       P3 &= ~(1<<(n)) 
	
#define   seg_init()      	P1DIR =0;
#define   seg07_port(n)   	P1 = n
#define   seg8_port(n)	n = n
#elif defined(SEG_LCD_4COM_10SEG_DRV)
#define   com_init()      	P3PU |= (0x0F);P3PD |= (0x0F)
#define   close_com(n)    	com_init();P3DIR |= (0x0F);P3DIR &= ~(1<<(n))
    
#define	set_com(n)      	P3 |= (1<<(n))
#define   clr_com(n)       	P3 &= ~(1<<(n)) 
	
#define   seg_init()      	P1DIR = 0;P0DIR &= ~(BIT(2));P0PU &= ~(BIT(2));P3DIR &= ~(BIT(4));P3PU &= ~(BIT(4));P3PD &= ~(BIT(4))
#define   seg07_port(n)   	P1 = n
#define   seg8_port(n)	P02 = n
#define   seg9_port(n)	P34 = n
#elif defined(SEG_LCD_4COM_8SEG_DRV)
#define   com_init()      	P3PU |= (0x0F);P3PD |= (0x0F)
#define   close_com(n)    	com_init();P3DIR |= (0x0F);P3DIR &= ~(1<<(n))
    
#define	set_com(n)      	P3 |= (1<<(n))
#define   clr_com(n)       	P3 &= ~(1<<(n)) 
	
#define   seg_init()      	P1DIR = 0;P0DIR &= ~(BIT(2))
#define   seg07_port(n)   	P1 = n
#else

#define   com_init()      	P3PU |= (0x1F);P3PD |= (0x1F)
#define   close_com(n)    	com_init();P3DIR |= (0x1F);P3DIR &= ~(1<<(n))
    
#define	set_com(n)      	P3 |= (1<<(n))
#define   clr_com(n)      	P3 &= ~(1<<(n)) 
	
#define   seg_init()      	P1DIR = 0;P0DIR &= ~(BIT(2));P0PU &= ~(BIT(2))//;P3PD &= ~(BIT(4))
#define   seg07_port(n)   	P1 = n
#define   seg8_port(n)	P02 = n
//#define   bklt_init()	  	//P3DIR &= ~(1<<0);P3 |= (1<<0);
#endif


#if defined(BACK_LIGHT_GPIO_PORT_P05)
#define  LCD_BACKLIGHT_INIT()	  	P0DIR &=~(BIT(5));P0PU|= (BIT(5))
#define LCD_BACKLIGHT_ON()			P05 =0;
#define LCD_BACKLIGHT_OFF()			P05 =1;
#elif defined(BACK_LIGHT_GPIO_PORT_P04)
#define  LCD_BACKLIGHT_INIT()	  	P0DIR &=~(BIT(4));P0PU|= (BIT(4))
#define LCD_BACKLIGHT_ON()			P04 =0;
#define LCD_BACKLIGHT_OFF()			P04 =1;
#else 
#define  LCD_BACKLIGHT_INIT()	  	
#define LCD_BACKLIGHT_ON()			
#define LCD_BACKLIGHT_OFF()			
#endif


#endif

enum {

	ICON_PLAY,
	ICON_PAUSE,
	ICON_USB,
	ICON_SD,
	ICON_CD,
	ICON_MP3,
	ICON_AUX,
	ICON_FM,
	ICON_FM_MHZ,
	ICON_AM_KHZ,
	ICON_SW,
	ICON_RADIO_ST,
	ICON_REC,
	ICON_REP_1,
	ICON_REP_ALL,
	ICON_REP_RDM,
	ICON_COL,
	ICON_MUTE,
	ICON_BELL,
	ICON_PROG,
	
	ICON_BAT,
	ICON_BAT_S1,
	ICON_BAT_S2,
	ICON_BAT_S3,
	ICON_BAT_S4,

	ICON_NONE,
};


#if defined(BK_DISP_LCD_DRV)

#define USB_DEV_MASK	0x0004
#define F_USB_DEV  		(lcd_buff[1])	

#define AUX_DEV_MASK	0x0000
#define F_AUX_DEV  		(lcd_buff[0])

#define SD_DEV_MASK	0x0004
#define F_SD_DEV		(lcd_buff[1])	

#define FM_DEV_MASK	0x0004
#define F_FM_DEV		(lcd_buff[0])	


#define POINTS_MASK		0x0010
#define F_POINTS		(lcd_buff[1])	

#define FM_MHZ_MASK	0x0020
#define F_MHZ_DEV		(lcd_buff[0])	

#define AM_KHZ_MASK	0x0080
#define F_KHZ_DEV		(lcd_buff[0])	

#define SW_MHZ_MASK	0x0001
#define F_SW_DEV		(lcd_buff[1])	

#define FM_P1_MASK		0x0000
#define F_P1_DEV		(lcd_buff[3])	


#define RTC_ICON_MASK	0x0000
#define F_RTC_ICON		(lcd_buff[3])

#define RTC_ALM_MASK	0x0000
#define F_RTC_ALM		(lcd_buff[3])	

#define RTC_PM_MASK	0x0000
#define F_RTC_PM		(lcd_buff[2])

#define RTC_AM_MASK	0x0000
#define F_RTC_AM		(lcd_buff[3])

#define REC_ICON_MASK	0x0000
#define REC_ICON		(lcd_buff[4])	

#define SPK_MUTE_MASK	0x0040
#define F_SPK_DEV		(lcd_buff[0])	

#define BATTERY_MASK	0x0000
#define F_BAT_ICON_BUF	(lcd_buff[0])	


#define TRADEMARK_MASK	0x0000
#define TRADEMARK_ICON		(lcd_buff[0])	

#define NUM_1BC_MASK	0x0000
#define F_NUM_1BC		(lcd_buff[2])	

#define REP_ALL_MASK	0x0000
#define REP_ONE_MASK	0x0010
#define F_REP_ALL		(lcd_buff[0])
#define F_REP_ONE 		(lcd_buff[0])

//#define L_ICON_BUF		(lcd_buff[4])
#define S1_ICON_MASK	0x0000
#define S1_ICON_BUF		(lcd_buff[0])
#define S2_ICON_MASK	0x0000
#define S2_ICON_BUF		(lcd_buff[0])
#define S3_ICON_MASK	0x0000
#define S3_ICON_BUF		(lcd_buff[0])
#define S4_ICON_MASK	0x0000
#define S4_ICON_BUF		(lcd_buff[0])

#define FIGURE_NUM	4
#elif defined(JK_CD_ZG_KS218_V001)

#define USB_DEV_MASK	0x0080
#define F_USB_DEV  		(lcd_buff[3])	

#define AUX_DEV_MASK	0x0000
#define F_AUX_DEV  		(lcd_buff[0])

#define SD_DEV_MASK	0x0000
#define F_SD_DEV		(lcd_buff[3])	

#define FM_DEV_MASK	0x0000
#define F_FM_DEV		(lcd_buff[3])	

#define AM_DEV_MASK	0x0000
#define F_AM_DEV		(lcd_buff[2])	

#define POINTS_MASK		0x0000
#define F_POINTS		(lcd_buff[0])	

#define FM_MHZ_MASK	0x0000
#define F_MHZ_DEV		(lcd_buff[0])	

#define AM_KHZ_MASK	0x0000
#define F_KHZ_DEV		(lcd_buff[1])	

#define FM_P1_MASK		0x0000
#define F_P1_DEV		(lcd_buff[3])	

#define SW_MHZ_MASK	0x0000
#define F_SW_DEV		(lcd_buff[2])	

#define MP3_ICON_MASK	0x0080
#define F_MP3_DEV		(lcd_buff[0])	

#define RADIO_ST_MASK	0x0000
#define F_ST_DEV		(lcd_buff[3])	

#define PLAY_ICON_MASK	0x0008
#define F_PLAY_DEV		(lcd_buff[0])	

#define PAUS_ICON_MASK	0x0000
#define F_PAUS_DEV		(lcd_buff[3])	

#define RTC_ICON_MASK	0x0000
#define F_RTC_ICON		(lcd_buff[3])

#define RTC_ALM_MASK	0x0000
#define F_RTC_ALM		(lcd_buff[4])	

#define RTC_PM_MASK	0x0000
#define F_RTC_PM		(lcd_buff[2])

#define RTC_AM_MASK	0x0000
#define F_RTC_AM		(lcd_buff[3])

#define REC_ICON_MASK	0x0000
#define F_REC_DEV		(lcd_buff[4])	

#define SPK_MUTE_MASK	0x0000
#define F_SPK_DEV		(lcd_buff[3])	

#define BATTERY_MASK	0x0000
#define F_BAT_ICON_BUF	(lcd_buff[0])	

#define PROG_ICON_MASK	0x0000
#define F_PROG_ICON_BUF	(lcd_buff[0])	

#define TRADEMARK_MASK	0x0000
#define TRADEMARK_ICON		(lcd_buff[0])	

#define NUM_1BC_MASK	0x0000
#define F_NUM_1BC		(lcd_buff[2])	

#define REP_ALL_MASK	0x0002
#define REP_ONE_MASK	0x0001
#define REP_RDM_MASK	0x0001
#define F_REP_ALL		(lcd_buff[0])
#define F_REP_ONE 		(lcd_buff[2])
#define F_REP_RDM 		(lcd_buff[1])

//#define L_ICON_BUF		(lcd_buff[4])
#define S1_ICON_MASK	0x0000
#define S1_ICON_BUF		(lcd_buff[0])
#define S2_ICON_MASK	0x0000
#define S2_ICON_BUF		(lcd_buff[0])
#define S3_ICON_MASK	0x0000
#define S3_ICON_BUF		(lcd_buff[0])
#define S4_ICON_MASK	0x0000
#define S4_ICON_BUF		(lcd_buff[0])

#define FIGURE_NUM	4
#elif defined(MCU_CD_728_LCD_MODULE)

#define USB_DEV_MASK	0x0001
#define F_USB_DEV  		(lcd_buff[4])	

#define AUX_DEV_MASK	0x0000
#define F_AUX_DEV  		(lcd_buff[0])

#define SD_DEV_MASK	0x0100
#define F_SD_DEV		(lcd_buff[4])	

#define CD_DEV_MASK	0x0004
#define F_CD_DEV		(lcd_buff[4])	

#define MP3_ICON_MASK	0x0002
#define F_MP3_DEV		(lcd_buff[4])	

#define FM_DEV_MASK	0x0010
#define F_FM_DEV		(lcd_buff[4])	

#define AM_DEV_MASK	0x0080
#define F_AM_DEV		(lcd_buff[3])	

#define POINTS_MASK		0x0080
#define F_POINTS		(lcd_buff[1])	

#define FM_MHZ_MASK	0x0080
#define F_MHZ_DEV		(lcd_buff[2])	

#define AM_KHZ_MASK	0x0090
#define F_KHZ_DEV		(lcd_buff[4])	

#define FM_P1_MASK		0x0000
#define F_P1_DEV		(lcd_buff[2])	

#define RADIO_ST_MASK	0x0020
#define F_ST_DEV		(lcd_buff[4])	

#define SW_MHZ_MASK	0x0000
#define F_SW_DEV		(lcd_buff[2])	


#define PLAY_ICON_MASK	0x0080
#define F_PLAY_DEV		(lcd_buff[0])	

#define PAUS_ICON_MASK	0x0000
#define F_PAUS_DEV		(lcd_buff[3])	

#define RTC_ICON_MASK	0x0000
#define F_RTC_ICON		(lcd_buff[3])

#define RTC_ALM_MASK	0x0000
#define F_RTC_ALM		(lcd_buff[4])	

#define RTC_PM_MASK	0x0000
#define F_RTC_PM		(lcd_buff[2])

#define RTC_AM_MASK	0x0000
#define F_RTC_AM		(lcd_buff[3])

#define REC_ICON_MASK	0x0000
#define F_REC_DEV		(lcd_buff[4])	

#define SPK_MUTE_MASK	0x0000
#define F_SPK_DEV		(lcd_buff[3])	

#define BATTERY_MASK	0x0000
#define F_BAT_ICON_BUF	(lcd_buff[0])	



#define PROG_ICON_MASK		0x0040
#define F_PROG_ICON_BUF	(lcd_buff[2])	


#define TRADEMARK_MASK	0x0000
#define TRADEMARK_ICON		(lcd_buff[0])	

#define NUM_1BC_MASK	0x0000
#define F_NUM_1BC		(lcd_buff[2])	

#define REP_ALL_MASK	0x0040
#define REP_ONE_MASK	0x0040
#define REP_RDM_MASK	0x0040
#define F_REP_ALL		(lcd_buff[4])
#define F_REP_ONE 		(lcd_buff[3])
#define F_REP_RDM 		(lcd_buff[1])

//#define L_ICON_BUF		(lcd_buff[4])
#define S1_ICON_MASK	0x0000
#define S1_ICON_BUF		(lcd_buff[0])
#define S2_ICON_MASK	0x0000
#define S2_ICON_BUF		(lcd_buff[0])
#define S3_ICON_MASK	0x0000
#define S3_ICON_BUF		(lcd_buff[0])
#define S4_ICON_MASK	0x0000
#define S4_ICON_BUF		(lcd_buff[0])

#define FIGURE_NUM	4


#elif defined(MCU_CD_727_LCD_MODULE)
#define USB_DEV_MASK	0x0000
#define F_USB_DEV  		(lcd_buff[1])	

#define AUX_DEV_MASK	0x0000
#define F_AUX_DEV  		(lcd_buff[0])

#define SD_DEV_MASK	0x0000
#define F_SD_DEV		(lcd_buff[4])	

#define CD_DEV_MASK	0x0000
#define F_CD_DEV		(lcd_buff[4])	

#define MP3_ICON_MASK	0x0000
#define F_MP3_DEV		(lcd_buff[0])	

#define FM_DEV_MASK	0x0004
#define F_FM_DEV		(lcd_buff[3])	

#define AM_DEV_MASK	0x0200
#define F_AM_DEV		(lcd_buff[2])	

#define POINTS_MASK		0x0200
#define F_POINTS		(lcd_buff[0])	

#define FM_MHZ_MASK	0x0000
#define F_MHZ_DEV		(lcd_buff[2])	

#define AM_KHZ_MASK	0x0200
#define F_KHZ_DEV		(lcd_buff[1])	

#define FM_P1_MASK		0x0002
#define F_P1_DEV		(lcd_buff[3])	

#define RADIO_ST_MASK	0x0001
#define F_ST_DEV		(lcd_buff[3])	

#define SW_MHZ_MASK	0x0000
#define F_SW_DEV		(lcd_buff[2])	


#define PLAY_ICON_MASK	0x0040
#define F_PLAY_DEV		(lcd_buff[3])	

#define PAUS_ICON_MASK	0x0010
#define F_PAUS_DEV		(lcd_buff[3])	

#define RTC_ICON_MASK	0x0000
#define F_RTC_ICON		(lcd_buff[3])

#define RTC_ALM_MASK	0x0000
#define F_RTC_ALM		(lcd_buff[4])	

#define RTC_PM_MASK	0x0000
#define F_RTC_PM		(lcd_buff[2])

#define RTC_AM_MASK	0x0000
#define F_RTC_AM		(lcd_buff[3])

#define REC_ICON_MASK	0x0000
#define F_REC_DEV		(lcd_buff[4])	

#define SPK_MUTE_MASK	0x0000
#define F_SPK_DEV		(lcd_buff[3])	

#define BATTERY_MASK	0x0000
#define F_BAT_ICON_BUF	(lcd_buff[0])	


#define TRADEMARK_MASK	0x0000
#define TRADEMARK_ICON		(lcd_buff[0])	

#define NUM_1BC_MASK	0x0000
#define F_NUM_1BC		(lcd_buff[2])	


#define PROG_ICON_MASK		0x0000
#define F_PROG_ICON_BUF	(lcd_buff[2])	

#define REP_ALL_MASK	0x0080
#define REP_ONE_MASK	0x0080
#define REP_RDM_MASK	0x0080
#define F_REP_ALL		(lcd_buff[0])
#define F_REP_ONE 		(lcd_buff[1])
#define F_REP_RDM 		(lcd_buff[3])

//#define L_ICON_BUF		(lcd_buff[4])
#define S1_ICON_MASK	0x0000
#define S1_ICON_BUF		(lcd_buff[0])
#define S2_ICON_MASK	0x0000
#define S2_ICON_BUF		(lcd_buff[0])
#define S3_ICON_MASK	0x0000
#define S3_ICON_BUF		(lcd_buff[0])
#define S4_ICON_MASK	0x0000
#define S4_ICON_BUF		(lcd_buff[0])

#define FIGURE_NUM	4

#elif defined(NEW_DH_LCD_MODULE)

#define USB_DEV_MASK	0x0001
#define F_USB_DEV  		(lcd_buff[1])	

#define AUX_DEV_MASK	0x0000
#define F_AUX_DEV  		(lcd_buff[0])

#define SD_DEV_MASK	0x0002
#define F_SD_DEV		(lcd_buff[4])	

#define FM_DEV_MASK	0x0000
#define F_FM_DEV		(lcd_buff[1])	

#define AM_DEV_MASK	0x0200
#define F_AM_DEV		(lcd_buff[2])	

#define PLAY_ICON_MASK	0x0040
#define F_PLAY_DEV		(lcd_buff[3])	

#define PAUS_ICON_MASK	0x0010
#define F_PAUS_DEV		(lcd_buff[3])	

#define RADIO_ST_MASK	0x0002
#define F_ST_DEV		(lcd_buff[3])	

#define POINTS_MASK		0x0008
#define F_POINTS		(lcd_buff[0])	

#define FM_MHZ_MASK	0x0080
#define F_MHZ_DEV		(lcd_buff[0])	

#define AM_KHZ_MASK	0x0080
#define F_KHZ_DEV		(lcd_buff[1])	

#define FM_P1_MASK		0x0002
#define F_P1_DEV		(lcd_buff[0])	

#define SW_MHZ_MASK	0x0080
#define F_SW_DEV		(lcd_buff[2])	


#define RTC_ICON_MASK	0x0000
#define F_RTC_ICON		(lcd_buff[3])

#define RTC_ALM_MASK	0x0008
#define F_RTC_ALM		(lcd_buff[4])	

#define RTC_PM_MASK	0x0000
#define F_RTC_PM		(lcd_buff[2])

#define RTC_AM_MASK	0x0000
#define F_RTC_AM		(lcd_buff[3])

#define REC_ICON_MASK	0x0080
#define F_REC_DEV		(lcd_buff[4])	

#define SPK_MUTE_MASK	0x0080
#define F_SPK_DEV		(lcd_buff[3])	

#define BATTERY_MASK	0x0040
#define F_BAT_ICON_BUF	(lcd_buff[0])	


#define TRADEMARK_MASK	0x0100
#define TRADEMARK_ICON		(lcd_buff[0])	

#define NUM_1BC_MASK	0x0000
#define F_NUM_1BC		(lcd_buff[2])	

#define REP_ALL_MASK	0x0020
#define REP_ONE_MASK	0x0020
#define REP_RDM_MASK	0x0080

#define F_REP_ALL		(lcd_buff[4])
#define F_REP_ONE 		(lcd_buff[0])
#define F_REP_RDM 		(lcd_buff[3])

//#define L_ICON_BUF		(lcd_buff[4])
#define S1_ICON_MASK	0x0010
#define S1_ICON_BUF		(lcd_buff[0])
#define S2_ICON_MASK	0x0004
#define S2_ICON_BUF		(lcd_buff[0])
#define S3_ICON_MASK	0x0001
#define S3_ICON_BUF		(lcd_buff[0])
#define S4_ICON_MASK	0x0000
#define S4_ICON_BUF		(lcd_buff[0])

#define FIGURE_NUM	4

#elif defined(K2092_DH_288_V001_000)

#define USB_DEV_MASK	0x0004
#define F_USB_DEV  		(lcd_buff[1])	

#define AUX_DEV_MASK	0x0000
#define F_AUX_DEV  		(lcd_buff[1])

#define SD_DEV_MASK	0x0004
#define F_SD_DEV		(lcd_buff[1])	

#define FM_DEV_MASK	0x0000
#define F_FM_DEV		(lcd_buff[1])	

#define POINTS_MASK		0x0010
#define F_POINTS		(lcd_buff[1])	

#define FM_MHZ_MASK	0x0024
#define F_MHZ_DEV		(lcd_buff[0])	

#define AM_KHZ_MASK	0x0082
#define F_KHZ_DEV		(lcd_buff[0])	

#define FM_P1_MASK		0x0000
#define F_P1_DEV		(lcd_buff[3])	

#define SW_MHZ_MASK	0x0001
#define F_SW_DEV		(lcd_buff[1])	

#define RTC_ICON_MASK	0x0000
#define F_RTC_ICON		(lcd_buff[3])

#define RTC_ALM_MASK	0x0000
#define F_RTC_ALM		(lcd_buff[3])	

#define RTC_PM_MASK	0x0000
#define F_RTC_PM		(lcd_buff[2])

#define RTC_AM_MASK	0x0000
#define F_RTC_AM		(lcd_buff[3])

#define REC_ICON_MASK	0x0040
#define F_REC_DEV		(lcd_buff[1])	

#define SPK_MUTE_MASK	0x0040
#define F_SPK_DEV		(lcd_buff[0])	

#define BATTERY_MASK	0x0000
#define F_BAT_ICON_BUF	(lcd_buff[0])	


#define TRADEMARK_MASK	0x0000
#define TRADEMARK_ICON		(lcd_buff[0])	

#define NUM_1BC_MASK	0x0000
#define F_NUM_1BC		(lcd_buff[2])	

#define REP_ALL_MASK	0//0x0004
#define REP_ONE_MASK	0//0x0008
#define F_REP_ALL		(lcd_buff[4])
#define F_REP_ONE 		(lcd_buff[0])

//#define L_ICON_BUF		(lcd_buff[4])
#define S1_ICON_MASK	0x0000
#define S1_ICON_BUF		(lcd_buff[0])
#define S2_ICON_MASK	0x0000
#define S2_ICON_BUF		(lcd_buff[0])
#define S3_ICON_MASK	0x0000
#define S3_ICON_BUF		(lcd_buff[0])
#define S4_ICON_MASK	0x0000
#define S4_ICON_BUF		(lcd_buff[0])

#define FIGURE_NUM	4

#elif //defined(K820_LHD_820_REC_V001)

#define USB_DEV_MASK	0x0100
#define F_USB_DEV  		(lcd_buff[4])	

#define AUX_DEV_MASK	0x0100
#define F_AUX_DEV  		(lcd_buff[0])

#define SD_DEV_MASK	0x0040
#define F_SD_DEV		(lcd_buff[4])	

#define FM_DEV_MASK	0x0000
#define F_FM_DEV		(lcd_buff[1])	


#define POINTS_MASK		0x0010
#define F_POINTS		(lcd_buff[0])	

#define FM_MHZ_MASK	0x0005
#define F_MHZ_DEV		(lcd_buff[0])	

#define AM_KHZ_MASK	0x0001
#define F_KHZ_DEV		(lcd_buff[1])	

#define SW_MHZ_MASK	0x0041
#define F_SW_DEV		(lcd_buff[0])	

#define FM_P1_MASK		0x0000
#define F_P1_DEV		(lcd_buff[3])	


#define RTC_ICON_MASK	0x0000
#define F_RTC_ICON		(lcd_buff[3])

#define RTC_ALM_MASK	0x0000
#define F_RTC_ALM		(lcd_buff[3])	

#define RTC_PM_MASK	0x0000
#define F_RTC_PM		(lcd_buff[2])

#define RTC_AM_MASK	0x0000
#define F_RTC_AM		(lcd_buff[3])

#define REC_ICON_MASK	0x0001
#define REC_ICON		(lcd_buff[4])	

#define SPK_MUTE_MASK	0x0001
#define F_SPK_DEV		(lcd_buff[2])	

#define BATTERY_MASK	0x0002
#define F_BAT_ICON_BUF	(lcd_buff[0])	


#define TRADEMARK_MASK	0x0000
#define TRADEMARK_ICON		(lcd_buff[0])	

#define NUM_1BC_MASK	0x0000
#define F_NUM_1BC		(lcd_buff[2])	

#define REP_ALL_MASK	0x0000
#define REP_ONE_MASK	0x0004
#define F_REP_ALL		(lcd_buff[0])
#define F_REP_ONE 		(lcd_buff[4])

//#define L_ICON_BUF		(lcd_buff[4])
#define S1_ICON_MASK	0x0008
#define S1_ICON_BUF		(lcd_buff[0])
#define S2_ICON_MASK	0x0020
#define S2_ICON_BUF		(lcd_buff[0])
#define S3_ICON_MASK	0x0080
#define S3_ICON_BUF		(lcd_buff[0])
#define S4_ICON_MASK	0x0080
#define S4_ICON_BUF		(lcd_buff[0])

#define FIGURE_NUM	4

#else

#define USB_DEV_MASK	0x0000
#define REP_ONE_MASK	0x0000
#define F_USB_DEV  		lcd_buff[0]	
#define F_REP_ONE 		lcd_buff[0]

#define FM_DEV_MASK	0x0000
#define SD_DEV_MASK	0x0000
#define F_FM_DEV		(lcd_buff[3])	
#define F_SD_DEV		(lcd_buff[1])	

#define RTC_PM_MASK	0x0000
#define NUM_1BC_MASK	0x0000
#define F_RTC_PM		(lcd_buff[2])
#define F_NUM_1BC		(lcd_buff[1])	

#define RTC_AM_MASK	0x0000
#define AUX_DEV_MASK	0x0000
#define POINTS_MASK		0x0004
#define RTC_ALM_MASK	0x0000
#define REP_ALL_MASK	0x0000
#define F_RTC_AM		(lcd_buff[3])
#define F_AUX_DEV		(lcd_buff[3])	
#define F_POINTS		(lcd_buff[3])	
#define F_RTC_ALM		(lcd_buff[3])	
#define F_REP_ALL		(lcd_buff[3])	



#define F_BAT_1			(lcd_buff[0])	
#define F_BAT_2			(lcd_buff[1])	
#define F_BAT_3			(lcd_buff[2])	
#define F_BAT_4			(lcd_buff[3])	
#endif

void init_disp_buf(void);
void disp_scan(void);
void lcd_backlight_ctrl(bool on);

void disp_set_brightness(u8 br);
void disp_putchar(u8 chardata,u8 loc);
void clr_lcd_digit(u8 lcd_num);

void disp_icon(u8 id);
void disp_clr_icon(u8 id);
void disp_flash_icon(u8 id);
void disp_clr_flash_icon(u8 id);

#endif

#endif
