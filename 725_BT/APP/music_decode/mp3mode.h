/*--------------------------------------------------------------------------*/
/**@file     mp3mode.h
   @brief    ��������
   @details
   @author
   @date   2010-12-13
   @note
*/
/*----------------------------------------------------------------------------*/
#include "Custom_config.h"

#ifndef _MP3_MODE_
#define _MP3_MODE_

#ifdef __C51__
#include "config.h"
#include "uart.h"
#include "decode.h"
#include "filesystem.h"

#include "device.h"
#include "key.h"
#include "disp_if.h"
#include "eq.h"
#include "iic.h"
#include "get_music_file.h"
#include "id3.h"
#include "dac.h"
#include "rtc_mode.h"

#define FAST_FARWORD_STEP	5

#ifdef USE_CD_MCU_MASTER_FUNC			
typedef enum
{
    REPEAT_ONE,
    REPEAT_ALL,
    //REPEAT_RANDOM,
    REPEAT_OFF,    
    REPEAT_FOLDER,    
    REPEAT_INTRO,  
    REPEAT_RANDOM,

    REPEAT_HEAD=REPEAT_ONE,
    REPEAT_END=REPEAT_OFF,

	
}PLAY_MODE;
#else
typedef enum
{
    REPEAT_ALL = 0,
    REPEAT_ONE,
    REPEAT_FOLDER,    
    REPEAT_RANDOM,

}PLAY_MODE;
#endif

enum 							//config command
{
    MUSIC_PAUSE = 0,				//������ͣ
	MUSIC_PLAY,					//���ڲ�����
	MUSIC_FF_FR,					//�������
	MUSIC_STOP,					   //����ֹͣ
};

void decode_play(void);
void music_info_init(void);
void stop_decode(void);
#endif

//#define MAX_VOL 30

#endif
