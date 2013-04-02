/************************************************************
*	SYSTEM GLOBAL OPTIONS
*************************************************************/
//#define USE_AUTO_FREQ
//#define AC209_28PIN		//28 PIN IC GPIO CONFIG


/*UART PORT ENABLE DEFINE*/
//#define   UART_ENABLE
#ifdef UART_ENABLE
#define 	_USE_UART_P2_
//#define _USE_UART_P0_
#endif

/*MUTE PORT DEFINE*/
//#define	MUTE_PORT_USE_P01
//#define   MUTE_PORT_USE_P00
#define MUTE_CTRL_LEVEL_NORMAL

/*PLAY STATUS LED PORT DEFINE*/
//DEFAULT GPIO P07

//#define _KEY_TONE_ENABEL_

//#define USE_POWER_KEY

//#define USE_GPIO_DETECT_EARPHONE_PLUGGED

//#define INDEPENDENT_VOLUME_KEY

//#define USE_LONG_PLAY_KEY_TO_SWITCH_MODE

//#define USE_SYS_MODE_RECOVER		//系统上电模式记忆功能

//#define EXCHANGE_LINE_IN_CHANNEL	//交换LINE IN 通道

//#define USE_MANUAL_IDLE_FUNC		//手动切换到IDLE

//#define OTP_MUSIC_FUNC_ENABLE

//#define VOLUME_CUSTOMER_DEFINE

//#define KEY_100_ENABLE

//#define VOLUME_DEFUALT 	0
/************************************************************
*	AUDIO DECODE  OPTIONS
*************************************************************/
//#define SUPPORT_PT2313
//#define USE_USB_AUDIO

//#define DECODE_STOP_MODE_ENABLE

//#define PLAY_MODE_MEM_RECOVER	//播放模式记忆功能
/************************************************************
*	FM REV OPTIONS
*************************************************************/
#define USE_RDAs_FM
#define USE_BK1080_FM
//#define USE_KT_FM_CHIP
//#define USE_SP3777_FM
#define USE_QN8035_FM
/*FM CRYSTAL DEFINE*/
//#define          _SHARE_CRYSTAL_24MHz_   	//共用24MHz晶振，请打开此宏
//#define       _SHARE_CRYSTAL_12MHz_   	//共用12MHz晶振，请打开此宏
#define       _SHARE_CRYSTAL_32KHz_   	//4共用32KHz晶振，请打开此宏
//#define       _FM_STEP_50K_           		//50K步进，请打开此宏

//#define KEEP_SILENT_WHEN_SCAN_CHANNEL

//#define ADKEY_NEXT_ADJST_FREQ

//#define ADKEY_PLAY_SHORT_SCAN_FRE

//#define SEMI_AUTO_SCAN_FREQ	//半自动搜台
/************************************************************
*	AUX  OPTIONS
*************************************************************/

//#define NOT_USE_LINE_IN_FUNC	//打开此宏来关闭AUX功能

//#define USE_LINE_IN_DETECT_FUNC	//打开此宏来启动LINE IN 检测功能

/************************************************************
*	RTC  OPTIONS
*************************************************************/

//#define USE_RTC_RAM_FUNC

//#define USE_RTC_FUNCTION

//#define USE_RTC_TIME_ONLY

//#define USE_RTC_YEAR_FUNCTION

//#define USE_RTC_ALARM_FUNCTION

/************************************************************
*	DISPLAY OPTIONS,LED TYPE SELECT
*************************************************************/
//#define DISPLAY_NO_DEV_WHEN_NO_SD_USB

//#define DISP_LED_GPIO_DRV		//GPIO直接驱动LED
//#define LED_DRVER_USE_SM1628// SM 1628 驱动IC
//#define DISP_LCD_GPIO_DRV		//GPIO直接驱动LCD
//#define USE_LCD_DRV_HT1621


//#define NO_LED_DISPLAY
//#define LED_USE_PLAY_STATUS	//默认LED 模组
//#define LED_USE_PLAY_MODE
//#define LED_USE_1X888


/************************************************************
*	OTHER CUSTOMED OPTIONS
*************************************************************/
//#define POWE_KEY_PLAY_KEY_IN_ONE_KEY	//PLAY POWER 共用一个按键
//#define  POWER_KEY_ON_WAIT_FOR_3_SEC
//#define  SYS_GO_IN_IDLE_IN_30_SEC
//#define MATRIX_KEY_ENABLE
//#define NO_ADKEY_FUNC
/************************************************************
*	OTHER CUSTOMED OPTIONS  END
*************************************************************/
/**Customer & version **/

//#define EEPROM_RTC_RAM_COMPATIBLE

#define JK_CD_IBC233B_V001


#ifdef JK_CD_IBC233B_V001

#define CUSTOMER_FW_VERSION	3


#define BLUE_TOOTH_UART_FUNC

#define WKUP_PIN_USE_ENABLE
#define BACK_LIGHT_GPIO_PORT_WKUP
//#define MUTE_PORT_USE_P03
#define IIC_GPIO_USE_P00_P01
#define DISP_LCD_GPIO_DRV		//GPIO直接驱动LCD

#define SEG_LCD_5COM_8SEG_DRV
#define JK_CD_IBC233B_LCD_MODULE

#define DISP_TOC_BAR
//#define USE_ERP_2_HDLR
//#define USE_USB_ERP_2_HDLR
#define WELCOME_DISP_ON_STR
#define USE_SYS_MODE_RECOVER
#define BLUETOOTH_CONFIG_DEV_NAME

#define USE_PROG_PLAY_MODE
#define ADKEY_PORT_P04
#define CUSTOMER_DEFINED_ADKEY_RESISTOR
#define JK_HEYUAN_RES_VALUE_11_KEYS

#define MASTER_SEL_CD_PLAY_MODE
#define MODE_KEY_SEL_FUNC
#define USE_AUX_FUNC
#define USE_CD_MCU_MASTER_FUNC
#define IIC_MCU_PORT_USE_P21_P22

#define USE_NEXT_PREV_SEL_FOLDER

#define MODE_KEY_LONG_PROG
#define DISABLE_P05_OSC_OUTPUT


#define NO_DEV_SHOW_NO_DEV

//#define VOLUME_DEFAULT			20

#define NO_SD_DECODE_FUNC
#define NO_PLAY_MODE_STR_DISP
#define USB_SD_DECODE_DEFAULT_REP_OFF
//#define FLASH_PLAY_ICON_WHEN_PAUSE
#define USE_POWER_KEY
#define USE_BLUE_TOOTH_FUNC
//#define USE_FOLDER_SELECT_FUNC
#define MUTE_PORT_USE_P36
//#define USE_USB_PROG_PLAY_MODE

#define USE_RADIO_FUNC
//#define AM_RADIO_FUNC
#define RADIO_FM_BAND_ONLY
#define SEMI_AUTO_SCAN_FUNC
#define GPIO_SEL_BAND_INFO_CONFIG
#define FM_SAVE_STATION_MANUAL
//#define RADIO_ST_INDICATOR
//#define NOT_USE_LINE_IN_FUNC
#define BT_GPIO_POWER_CTRL_AT_VPP

#define TWO_ADKEY_ENABLE
#define TWO_ADKEY_PORT_P03
#endif

