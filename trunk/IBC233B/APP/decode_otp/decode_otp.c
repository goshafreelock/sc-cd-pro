/*--------------------------------------------------------------------------*/
/**@file      decode_otp.c
   @brief     音乐库
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/

#include "decode_otp.h"
#if OTP_MUSIC_ENABLE

#include "mp3mode.h"
#include "decode.h"
#include "srom.h"

SROM_MC(CONST_MyMusic) // Create external declarations for the CODE Class

extern u8 _idata  music_vol;
extern void f_open_dummy_file(u32 fsize); //打开一个虚拟的文件
extern void stop_decode(void);
extern u16 f_read (u8 _xdata *buffer, u16 len);
extern u8  f_open(u8 _pdata *ptr, u16 *) ;

extern u8 _xdata buffer[1024];              //<解码BUFF
extern _code unsigned char music_file[];
extern u8 _xdata win_buffer[512];           //<文件系统读BUFF

//u8 _pdata fat_file[2] _at_ 0x29; 
extern FIL _pdata fat_file; 
extern u16 f_read (u8 _xdata *buffer, u16 len);
extern u8 music_type;
extern xd_u8 eq_mode;
extern xd_u8 play_status;
extern xd_u8 decode_cmd;

extern u8 code_otp_read(u8 _xdata *buf, u16 lbah, u16 lbal);


void decode_opt_music_file(void)
{
	u8 key;

    dsp_hi_pro();
    decodeint_hi_pro();

	f_init(win_buffer, (u16)code_otp_read, 1);
	f_open((u8 *)&fat_file, 0);
	f_open_dummy_file((u16)&_CONST_MyMusic_L_);

    mad_control(0,(u16)buffer);
    set_eq(0);                             //必须在每首歌曲播放前初始化EQ
    music_type = musice_format_check(buffer);
	set_max_vol(MAX_ANALOG_VOL, MAX_DIGITAL_VOL);
    my_memset(win_buffer, 0, 512);              //解码前，必须清除buffer
	main_vol_set(0, SET_USE_CURRENT_VOL);
    flush_low_msg();
    play_status = MUSIC_PLAY;
    decode_cmd = 0;
    decode_user_exit(0);
	enable_softint();
    enable_decode_isr();
	set_device(0);
    while(1)
	{
	    key = get_msg();
		if((DECODE_MSG_DISK_ERR == key) || (DECODE_MSG_FILE_END == key))
		{
		    stop_decode();
			return;
		}
	}
}

#endif
