/*--------------------------------------------------------------------------*/
/**@file   key.h
   @brief  按键消息扫描头文件
   @details
   @author
   @date   2010-12-13
   @note
*/
/*----------------------------------------------------------------------------*/

#ifndef	_KEY_H_
#define _KEY_H_
#include "Custom_config.h"

#ifdef __C51__
#include "config.h"
#include "gpio_if.h"



#define adc_start() ADCCON |= 0x80;

void keyInit(void);
void keyScan(void);
void adc_scan(void);
u8 ldoin_voltage(void);
u8 get_cur_select_func();
#if defined(USE_BAT_MANAGEMENT)
void bmt_hdlr(void);
#endif
#endif

#if 0
#define ADC_KEY_IO     0x4f
#define ADC_VDD_12     0x48
#define ADC_LDOIN      0x49
#define ADC_MAX_USED   0x03
#endif

#define KEY_BASE_CNT  6
#define KEY_LONG_CNT  (75/2)
#define KEY_HOLD_CNT  15//15


#define  NO_KEY              0xff
#define  NO_MSG              0xff



#define  INFO_0             0x00
#define  INFO_1             0x01
#define  INFO_2             0x02
#define  INFO_3             0x03
#define  INFO_4             0x04
#define  INFO_5             0x05
#define  INFO_6             0x06
#define  INFO_7             0x07
#define  INFO_8             0x08
#define  INFO_9             0x09
#define  INFO_PLAY          0x0a
#define  INFO_CH_UP         0x0b
#define  INFO_CH_DOWN       0x0c
#define  INFO_VOL_PLUS      0x0d
#define  INFO_VOL_MINUS     0x0e
#define  INFO_FRE_UP        	0x0f
#define  INFO_FRE_DOWN      	0x10
#define  INFO_NEXT_FIL      	0x11
#define  INFO_PREV_FIL      	0x12
#define  INFO_EQ_UP		    	0x13
#define  INFO_MUTE        		0x14
#define  INFO_EQ_DOWN		0x16
#define  INFO_PICK_SONG	    	0x15
//#define  INFO_PICK_FRE		0x16
#define  INFO_MODE			0x17
#define  INFO_PLAY_MODE	0x18
#define  INFO_FOLDER        	0x19
#define  INFO_MINUS         	0x1a
#define  INFO_POWER         	0x1b

#define INFO_ALM_BELL		0x1c
#define  INFO_250MS	         	0x1d
#define  INFO_SECOND	     	0x1e
#define  INFO_HALF_SECOND	 0x1f
#define  INFO_100         		0x1d
#define  INFO_STOP         		0x1d


#define MSG_USB_DISK_IN         0xf0
#define MSG_USB_DISK_OUT        0xf1
#define MSG_USB_PC_IN           0xf2
#define MSG_USB_PC_OUT          0xf3
#define MSG_SDMMC_IN            0xf4
#define MSG_SDMMC_OUT           0xf5
#define MSG_PC_MUTE             0xF6
#define MSG_PC_UNMUTE           0xF7
#define MSG_PC_AUDIO			0xF8
#define MSG_AUX_IN              0xfd
#define MSG_AUX_OUT             0xfe
#define INFO_TIMER_OFF		0xFA
#define INFO_TIMER_ON		0xFB
#define INFO_BAND_SEL		0xFC


/**/
#define  KEY_HOLD			 0x40
#define  KEY_LONG			 0x60
#define  KEY_SHORT_UP		 0x80
#define  KEY_LONG_UP		 0x00


#define  SEL_GIVEN_DEVICE_GIVEN_FILE        0xE0
#define  SEL_CUR_DEVICE_GIVEN_FILE          0xE1
#define  SEL_GIVEN_FILE                     0xE2
#define  INIT_PLAY                          0xE3
#define  INFO_AUTO_NEXTFILE                 0xe4
#define  INFO_DISP_FILENAME                 0xe5
#define  INFO_DEF_DEVICE                    0xe7
#define  INFO_NEWFILE		                0xe8
#define  INFO_NEWDEVICE		                0xe9
#define  INFO_NEXTMODE		                0xea
#define  INFO_ALM_ON						0xeb
#define  INFO_NEXT_SYS_MODE					0xEC
#define  INFO_NEXT_FM_MODE						0xEE

#define  MAXMSG_CNT                         32

//与解码有关的消息，从0xD0~0xDF,有些是在lib中调用，不能随便修改
#define MUSIC_DEF_NUMBER     0xD0
#define DECODE_MSG_FILE_END  0xD1
#define DECODE_MSG_DISK_ERR  0xD2




#define IR_KEY_1	   INFO_PLAY
#define IR_KEY_2	   INFO_FRE_DOWN
#define IR_KEY_3	   INFO_FRE_UP

#define IR_KEY_4	   INFO_EQ_UP
#define IR_KEY_5	   INFO_VOL_MINUS
#define IR_KEY_6	   INFO_VOL_PLUS

#define IR_KEY_7	   INFO_0
#define IR_KEY_8	   INFO_PREV_FIL
#define IR_KEY_9	   INFO_NEXT_FIL
#define IR_KEY_10	   INFO_1
#define IR_KEY_11	   INFO_2
#define IR_KEY_12	   INFO_3
#define IR_KEY_13	   INFO_4
#define IR_KEY_14	   INFO_5
#define IR_KEY_15	   INFO_6
#define IR_KEY_16	   INFO_7
#define IR_KEY_17	   INFO_8
#define IR_KEY_18	   INFO_9
#define IR_KEY_19	   INFO_PICK_SONG
#define IR_KEY_20	   NO_KEY
#define IR_KEY_21	   INFO_PICK_FRE



#define  BIT_SDREMOVE       0//BIT(0)
#define  BIT_SDINSERT       1//BIT(1)
#define  BIT_USBREMOVE      2//BIT(4)
#define  BIT_USBINSERT      3//BIT(5)


#define IR_00	INFO_PLAY
#define IR_01	INFO_FRE_DOWN
#define IR_02	INFO_FRE_UP

#define IR_03	INFO_EQ_UP
#define IR_04	INFO_VOL_MINUS
#define IR_05	INFO_VOL_PLUS

#define IR_06	INFO_0
#define IR_07	INFO_PREV_FIL
#define IR_08	INFO_NEXT_FIL

#define IR_09	INFO_1
#define IR_10	INFO_2
#define IR_11	INFO_3

#define IR_12	INFO_4
#define IR_13	INFO_5
#define IR_14	INFO_6

#define IR_15	INFO_7
#define IR_16	INFO_8
#define IR_17	INFO_9

#define IR_18	INFO_PICK_SONG
#define IR_19	NO_KEY
#define IR_20	INFO_PICK_FRE

#define NO_IR 0xff

#if 1//defined(NEW_RES_VALUE_CACULATING)

#if defined(JK_HEYUAN_RES_VALUE)
#define RES_PULL_UP			22000		/// 3.3K
#define RES_1				0
#define RES_2				3000
#define RES_3				6200	
#define RES_4				9100
#define RES_5				15000
#define RES_6				24000
#define RES_7				33000
#define RES_8				510000
#elif defined(CUSTOME_GW_RES_VALUE)
#define RES_PULL_UP			3300		/// 3.3K
#define RES_1				0
#define RES_2				1500
#define RES_3				5100	
#define RES_4				7500
#define RES_5				16000
#define RES_6				30000
#define RES_7				36000
#define RES_8				500000
#elif defined(CUSTOME_GW_RES_VALUE_2)
#define RES_PULL_UP			22000		/// 3.3K
#define RES_1				7500
#define RES_2				12000
#define RES_3				16000	
#define RES_4				24000
#define RES_5				36000
#define RES_6				51000
#define RES_7				91000
#define RES_8				220000
#else
#define RES_PULL_UP			3300		/// 3.3K
#define RES_1				0
#define RES_2				470
#define RES_3				1000	
#define RES_4				2000
#define RES_5				3300
#define RES_6				5600
#define RES_7				10000
#define RES_8				33000
#endif

#define RES_VOLT_1			(((255*RES_2)/(RES_2+RES_PULL_UP)+(255*RES_1)/(RES_1+RES_PULL_UP))/2)
#define RES_VOLT_2			(((255*RES_3)/(RES_3+RES_PULL_UP)+(255*RES_2)/(RES_2+RES_PULL_UP))/2)	
#define RES_VOLT_3			(((255*RES_4)/(RES_4+RES_PULL_UP)+(255*RES_3)/(RES_3+RES_PULL_UP))/2)	
#define RES_VOLT_4			(((255*RES_5)/(RES_5+RES_PULL_UP)+(255*RES_4)/(RES_4+RES_PULL_UP))/2)
#define RES_VOLT_5			(((255*RES_6)/(RES_6+RES_PULL_UP)+(255*RES_5)/(RES_5+RES_PULL_UP))/2)
#define RES_VOLT_6			(((255*RES_7)/(RES_7+RES_PULL_UP)+(255*RES_6)/(RES_6+RES_PULL_UP))/2)
#define RES_VOLT_7			(((255*RES_8)/(RES_8+RES_PULL_UP)+(255*RES_7)/(RES_7+RES_PULL_UP))/2)
#define RES_VOLT_NO_KEY	0xF0
#endif

#if defined( CUSTOMER_DEFINED_ADKEY_RESISTOR)
/*Customer define adkey value */

#if 0
#define ADKEY_RES_1		0x10
#define ADKEY_RES_2		0x30
#define ADKEY_RES_3		0x50
#define ADKEY_RES_4		0x70
#define ADKEY_RES_5		0x90
#define ADKEY_RES_6		0xB0
#define ADKEY_RES_7		0xD0	/**/
#define ADKEY_RES_NOKEY		0xF0	/*NO KEY*/
#else
#define ADKEY_RES_1		RES_VOLT_1
#define ADKEY_RES_2		RES_VOLT_2
#define ADKEY_RES_3		RES_VOLT_3
#define ADKEY_RES_4		RES_VOLT_4
#define ADKEY_RES_5		RES_VOLT_5
#define ADKEY_RES_6		RES_VOLT_6
#define ADKEY_RES_7		RES_VOLT_7	
#define ADKEY_RES_NOKEY		RES_VOLT_NO_KEY	/*NO KEY*/
#endif
/*Default adkey value */

#if defined(JK_CD_ZG_KS218_V001)
#define ADKEY_1		INFO_PLAY
#define ADKEY_2		INFO_NEXT_FIL
#define ADKEY_3		INFO_PREV_FIL
#define ADKEY_4		INFO_STOP
#define ADKEY_5		INFO_MODE
#define ADKEY_6		INFO_FOLDER
#define ADKEY_7		NO_KEY
#define ADKEY_8		NO_KEY
#define ADKEY_NOKEY	NO_KEY
#else
#define ADKEY_1		INFO_POWER
#define ADKEY_2		INFO_STOP
#define ADKEY_3		INFO_NEXT_FIL
#define ADKEY_4		INFO_PREV_FIL
#define ADKEY_5		INFO_MODE
#define ADKEY_6		INFO_VOL_PLUS
#define ADKEY_7		INFO_VOL_MINUS
#define ADKEY_8		INFO_PLAY
#define ADKEY_NOKEY	NO_KEY
#endif

#else

/*Customer define adkey value */
#define ADKEY_RES_1		RES_VOLT_1
#define ADKEY_RES_2		RES_VOLT_2
#define ADKEY_RES_3		RES_VOLT_3
#define ADKEY_RES_4		RES_VOLT_4
#define ADKEY_RES_5		RES_VOLT_5
#define ADKEY_RES_6		RES_VOLT_6
#define ADKEY_RES_7		RES_VOLT_7	
#define ADKEY_RES_NOKEY		RES_VOLT_NO_KEY	/*NO KEY*/

/*Default adkey value */
#define ADKEY_1		INFO_PLAY
#define ADKEY_2		INFO_PREV_FIL
#define ADKEY_3		INFO_NEXT_FIL
#define ADKEY_4		INFO_MODE
#define ADKEY_5		INFO_STOP
#define ADKEY_6		INFO_PLAY_MODE
#define ADKEY_7		INFO_VOL_MINUS
#define ADKEY_8		INFO_VOL_PLUS
#define ADKEY_NOKEY	NO_KEY
#endif


#define ADKEY2_RES_1		0x0F
#define ADKEY2_RES_2		0x2A
#define ADKEY2_RES_3		0x41
#define ADKEY2_RES_4		0x58
#define ADKEY2_RES_5		0x73
#define ADKEY2_RES_6		0x8E
#define ADKEY2_RES_7		0xA5	/**/
#define ADKEY2_RES_8		0xC0
#define ADKEY2_RES_9		0xDB
#define ADKEY2_RES_10		0xF0	/**/
#define ADKEY2_RES_NOKEY	0xF0	/*NO KEY*/
#endif

