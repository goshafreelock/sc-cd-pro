/*--------------------------------------------------------------------------*/
/**@file     config.h
   @brief    һЩ��������������
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
	DIS_SD_CMD_CHK = 0,	///<��������SD������״̬������£���ͣSD���߼��
	EN_SD_CMD_CHK,		///<��������SD������״̬������£��ָ�SD���߼��
	SET_SD_H_CHK,		///<��������SD������״̬������£�SD������ʱ���100MSһ��
	SET_SD_L_CHK,		///<��������SD������״̬������£�SD������ʱ���1Sһ��
}Sd_api_cmd;

#if 1
typedef enum
{

    SYS_FMREV,
		
#ifdef USE_CD_MCU_MASTER_FUNC
    SYS_MCU_CD,
#endif

#ifdef USE_BLUE_TOOTH_FUNC			
    SYS_BLUE_TOOTH,
#endif
    
    SYS_AUX,

#ifdef AM_RADIO_FUNC
    SYS_AMREV,
#endif

#if 1//def USE_USB_SD_DECODE_FUNC	       
    SYS_MP3DECODE_USB,
    SYS_MP3DECODE_SD,
#endif
    

#if defined(IPONE_INDEPENDENT_MODE)	
    SYS_IPH,
#endif    

    SYS_RTC,
    SYS_IDLE,
    SYS_USB_DEVICE,
}SYS_WORK_MODE;
#else
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
#endif
									   

#define OSC_CLOCK		32768L		///<ϵͳ����ѡ��
#define OTP_MUSIC_ENABLE 		0			///<�Ƿ���Ҫʹ�ÿ�������

/*0: DECODE EQ, 1: DSP EQ*/
#define DECODE_EQ       0
#define DSP_EQ          1
#define EQ_WORK_USE     DECODE_EQ
/**/

#define USB_PROG_MAX		99

#define  LED_DISP  1
#define  LCD_DISP  0

/*��ʱ��ʾʱ��*/
#define RETURN_TIME      12

/*sd���ģʽ**/
#define SDMMC_CMD_MODE   0	  ///<ʹ��SD card �����ѯģʽ
#define SDMMC_CLK_MODE   1	  ///<ʹ��SD clk ��ѯģʽ
#define SDMMC_GPIO_MODE  0	  ///<ʹ�ö�����GPIO��SD���߲�ѯģʽ

#define FF_FR_MUSIC                      1			//�ڿ������ʱ���Ƿ���Ҫ��������
#define RTC_ENABLE                       0		    //<�Ƿ���Ҫʹ��RTC����(���Ƿ�ʹ��RTC�ڲ���RAM�޹�)
#define VOICE_TIME_ENABLE          0          //������ʱ����ʹ�ܣ�     1����  0���ر�


#define FILE_ENCRYPTION                  0          //�ļ����ܹ���ʹ��;      1����  0���ر�
#define DEVICE_ENCRYPTION                0          //�豸���ܹ���ʹ�ܣ�     1����  0���ر�
#define FAT_MEMORY                       0          //�豸���书��ʹ�ܣ�     1����  0���ر�
#endif
