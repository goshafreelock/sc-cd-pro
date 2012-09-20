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
//#define USE_CL6017G_FM
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

//#define NEW_DH_LCD_MODULE

//#define JK2092_AMFM_BK_V001
//#define JK2092_AMFM_BK5018_V001
/////#define JK2092_DH_289_V001
//#define K2092_DH_228_V001
//#define K2092_DH_129_V001
#define JK_CD_727_V001

#ifdef JK_CD_727_V001

#define MUTE_PORT_USE_P03
#define IIC_GPIO_USE_P00_P01
#define DISP_LCD_GPIO_DRV		//GPIO直接驱动LCD
#define MCU_CD_727_LCD_MODULE
#define SEG_LCD_4COM_10SEG_DRV

#define ADKEY_PORT_P06
#define CUSTOMER_DEFINED_ADKEY_RESISTOR
//#define JK_HEYUAN_RES_VALUE
#define USE_SYS_MODE_RECOVER

#define MODE_KEY_SEL_FUNC
#define USE_POWER_KEY_SHORT_FOR_MODE
#define USE_CD_MCU_MASTER_FUNC
#define USE_RADIO_FUNC
#define USE_POWER_KEY

#define AM_FM_BAND_ONLY
#define SEMI_AUTO_SCAN_FUNC
#define GPIO_SEL_BAND_INFO_CONFIG
#define FM_SAVE_STATION_MANUAL

#endif

#ifdef JK2092_AMFM_BK5018_V001

#define IIC_GPIO_USE_P00_P01
#define DISP_LCD_GPIO_DRV		//GPIO直接驱动LCD
#define BK_DISP_LCD_DRV		//GPIO直接驱动LCD
#define BACK_LIGHT_GPIO_PORT_P05

#define DISP_PAUS_STR

#define AM_FM_BAND_ONLY

#define JOG_STICK_FUNC
#define JOG_STICK_PORT_P02_P03
//#define ADKEY_SELECT_MODE
//#define USE_ADVOLT_FOR_FUNC_MODE_SEL

#define SAVE_BAND_FREQ_INFO
//#define USE_BAT_MANAGEMENT
#define USE_VALIDSTATION_CHECK
//#define USE_ADVOLT_FOR_RADIO_BAND_SEL
#define SEMI_AUTO_SCAN_FUNC

#define USE_LINE_IN_DETECT_FUNC
#define AUX_DETECT_USE_P07
#endif


#ifdef JK2092_AMFM_BK_V001

#define IIC_GPIO_USE_P01_P00
#define DISP_LCD_GPIO_DRV		//GPIO直接驱动LCD
#define BK_DISP_LCD_DRV		//GPIO直接驱动LCD

#define DISP_PAUS_STR

#define JOG_STICK_FUNC
#define ADKEY_SELECT_MODE
#define USE_ADVOLT_FOR_FUNC_MODE_SEL

#define SAVE_BAND_FREQ_INFO
//#define USE_BAT_MANAGEMENT
#define USE_VALIDSTATION_CHECK
#define USE_ADVOLT_FOR_RADIO_BAND_SEL
#define SEMI_AUTO_SCAN_FUNC
#endif

#ifdef K2092_DH_228_V001

#define IIC_GPIO_USE_P01_P00
#define DISP_LCD_GPIO_DRV		//GPIO直接驱动LCD
#define NEW_DH_LCD_MODULE
#define NO_PLAY_MODE_STR_DISP
#define DISP_PAU_STR

//#define JOG_STICK_FUNC
#define PWR_CTRL_IN_IDLE_MODE
#define SW_GAIN_HW_CTRL
#define ADKEY_SELECT_MODE
#define USE_ADVOLT_FOR_FUNC_SEL_TYPE_TWO

#define USE_BAT_MANAGEMENT
#define USE_GPIO_MEASURE_VOLT
#define CHARGER_DETECT_INDICATOR
#define DC_CHARGER_DET_SHARE_IIC
//#define DC_CHARGER_DET_USE_P10

#define KEY_100_ENABLE
#define MULTI_BAND_KT_0915_IN_USE
#define SAVE_BAND_FREQ_INFO

#define USE_VALIDSTATION_CHECK
#define SEMI_AUTO_SCAN_FUNC
#define USE_KEY_FOR_BAND_SELECT

#define RTC_DISP_IN_IDLE_MODE
#define USE_RTC_ALARM_FUNCTION

#define ALARM_USE_MULTI_SOURCE_FUNC
//#define GPIO_SEL_BAND_INFO_CONFIG
#define USE_TIMER_POWER_OFF_FUNC

//#define USE_AUX_FUNC
//#define USE_LINE_IN_DETECT_FUNC
//#define AUX_DETECT_USE_P02

#endif

#ifdef K2092_DH_129_V001

#define IIC_GPIO_USE_P01_P00

#define DISP_LCD_GPIO_DRV		//GPIO直接驱动LCD
#define NEW_DH_LCD_MODULE
#define NO_PLAY_MODE_STR_DISP
#define DISP_PAU_STR

#define JOG_STICK_FUNC
#define FAST_STICK_TUNE_FUNC
#define JOG_STICK_PORT_P24_P25
#define ADKEY_SELECT_MODE
#define USE_ADVOLT_FOR_FUNC_SEL_TYPE_FOUR

#define PWR_CTRL_IN_IDLE_MODE
#define PWR_CTRL_WKUP

#define USE_BAT_MANAGEMENT
#define USE_GPIO_MEASURE_VOLT
#define CHARGER_DETECT_INDICATOR
#define DC_CHARGER_DET_SHARE_IIC
//#define DC_CHARGER_DET_USE_P10

#define MULTI_BAND_KT_0915_IN_USE
#define SAVE_BAND_FREQ_INFO
#define SEMI_AUTO_SCAN_FUNC
#define USE_VALIDSTATION_CHECK
#define USE_KEY_FOR_BAND_SELECT


#define KEY_100_ENABLE

#define RTC_DISP_IN_IDLE_MODE
#define USE_RTC_ALARM_FUNCTION

//#define GPIO_SEL_BAND_INFO_CONFIG
#define ALARM_USE_MULTI_SOURCE_FUNC
#define USE_TIMER_POWER_OFF_FUNC

#endif














