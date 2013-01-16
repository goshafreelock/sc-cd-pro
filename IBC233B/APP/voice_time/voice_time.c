 /*--------------------------------------------------------------------------*/
/**@file      decode_otp.c
   @brief     语音报时
   @details
   @author
   @date   2012-7-6
   @note
*/
/*----------------------------------------------------------------------------*/

#include "voice_time.h"
#include "mp3mode.h"
#include "decode.h"
#include "srom.h"
#include "voicetime_reource.h"
#include "rtc_mode.h"

#if VOICE_TIME_ENABLE
//SROM_MC(CONST_MyMusic) // Create external declarations for the CODE Class

extern u8 _idata music_vol;
extern void f_open_dummy_file(u32 fsize); //打开一个虚拟的文件
extern void stop_decode(void);
extern u16 f_read (u8 _xdata *buffer, u16 len);
extern u8  f_open(u8 _pdata *ptr, u16 *) ;
extern u8 alm_bell_key(u8 key);
extern void get_curr_date(void);

extern u8 _xdata buffer[1024];              //<解码BUFF
extern u8 _xdata win_buffer[512];           //<文件系统读BUFF

extern FIL _pdata fat_file; 
extern u16 f_read (u8 _xdata *buffer, u16 len);
extern u8 music_type;
extern xd_u8 eq_mode;
extern xd_u8 play_status;
extern u8 decode_cmd;
extern u8 code_otp_read(u8 _xdata *buf, u16 lbah, u16 lbal);
u16  otp_music_addr;
extern RTC_TIME curr_date;
u16 xdata play_buf[8];
u8 xdata play_len;
extern bool voicetime_on;
/*********************************************************************************
 * void set_time_buf(void)
 * Description : 提取需要播放的音乐文件序号
 * Arguments   : 
 * Returns     :  
 * Notes       : 
 *********************************************************************************/
void get_time_tobuf(void)
{
   u8 time;
   play_len =0;
   get_curr_date();
  // my_memset((u8 _xdata *)play_buf,0,8);
  // play_buf[play_len++]= BPF_XZSJ_MP3;
    if(0 == curr_date.hour)
    {
        play_buf[play_len++]= music_file_0; 
        play_buf[play_len++]= music_file_dian;
    }
    else
    {
       
        if(curr_date.hour >= 10)
        {   
            time = curr_date.hour /10;
            if( 2 == time)
            {
                play_buf[play_len++]= (u16)vocicetime_num[time];//time; //2 1   
            }
            play_buf[play_len++]= music_file_shi; 
        }
    
        time = curr_date.hour %10;
        if(0 != time)
        { 
            play_buf[play_len++]= (u16)vocicetime_num[time];//time;   
        } 
        play_buf[play_len++]= music_file_dian;
    }

    if(0 == curr_date.minutes)
    {
       // play_buf[i++]= music_file_0; 
       // play_buf[i++]= music_file_fen;
    }
    else
    {
        
        if(curr_date.minutes >= 10)
        {
            time = curr_date.minutes /10;
            if( 1 != time)
            {
                play_buf[play_len++]= (u16)vocicetime_num[time];//time; //5 4 3 2 1   
            }
            play_buf[play_len++]= music_file_shi; 
        }
        else
        {
             play_buf[play_len++]= music_file_0; 
        }
        time = curr_date.minutes %10;
        if(0 != time)
        { 
            play_buf[play_len++]= (u16)vocicetime_num[time];//time;    
        }
        play_buf[play_len++]= music_file_fen;
    }
}


u8 decode_voicetime_file()
{
	u8 key; 
   	u8 cnt =0;
    	u8 flag;
    	get_time_tobuf();    //获取需要报时的文件

_play_voicetime:
    	voicetime_on = 1;
    	dsp_hi_pro();
    	decodeint_hi_pro();
	f_init(win_buffer, (u16)code_otp_read, 1);
	f_open((u8 *)&fat_file, 0);
    	f_open_dummy_file(0x200);    
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

       	switch (key)
		{    
          case DECODE_MSG_DISK_ERR:
          case DECODE_MSG_FILE_END:
                otp_music_addr = play_buf[cnt];                                   
                cnt++;           
                delay_10ms(20); 
                        
                if(cnt > play_len)
                {
                   cnt= 0;
                   otp_music_addr =0;
                   stop_decode();
    		       return 0;  
                } 
                else
                {
                  goto _play_voicetime; 
                } 	 

		   default :
#if 0		   	
			flag = alm_bell_key(key);
            if(flag)
            {   
                          
               otp_music_addr =0;
               stop_decode();  
               close_alm_bell();                       
              return flag;
            }
#endif	
		break;
		}
	}
}


#endif




