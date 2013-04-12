/*--------------------------------------------------------------------------*/
/**@file     mp3mode.c
   @brief    解码任务模块
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/

/*mp3mode.c*/
#include "Custom_config.h"

#include "mp3mode.h"
#include "fat_memory.h"
#include "config.h"
#include "voice_time.h"
extern u8 _xdata buffer[1024];              //<解码BUFF
extern u8 _xdata win_buffer[512];           //<文件系统读BUFF
extern FSAPIMSG _pdata fs_msg;              //<带有文件文件系统信息的结构体变量
extern _xdata u8 filename_buff[];
extern bool get_filename( u8 _xdata *p );
extern MAD_DECODE_INFO _pdata mad_decode_dsc;
extern xd_u8 return_cnt;
extern xd_u16 cfilenum;
extern xd_u8 work_mode;
extern xd_u8 curr_menu,disp_scenario;
extern u8 device_active;
extern u8 decode_cmd;
extern xd_u8 my_music_vol;
extern bool adkey_detect;

extern s8 _idata ff_fr_step;  ///<快进 快退步径 负数为快退 正数为快进

//bool play_status;       ///< 播放状态 1:播放;0:暂停;
xd_u8 play_status=MUSIC_STOP;			///< 播放状态 0：暂停；1：播放；2：快进/快退；
xd_u8 given_device;        ///< 指定需要获取的设备 1 SD ; 2 USB; 0:不知道设备；其他：非法值
/** 音效模式 */
xd_u8 eq_mode;
/** 循环模式 */
xd_u8 play_mode;
/** 指定找到的文件号 */
xd_u16 given_file_number;
/** 当前解码播放的文件的格式 */
u8 music_type;
/** 文件信息滚动显示的计数值 */
xd_u16 filenameCnt;
bool playpoint_flag;


#ifdef USE_USB_SD_DECODE_FUNC	       

extern u16 playpoint_filenum;
extern PLAYPOINT_TIME playpoint_time;
extern u8 _idata music_vol;
extern FSINFO _xdata fs_info;
extern u8 device_online;
extern volatile bool bObufMute;
extern void DAC_auto_mute(void);
extern bool dac_stop_mute;
extern volatile u8 bDACTimeOut;			//等待OBUF 清空timeout
#ifdef ADKEY_SELECT_MODE
extern bool mode_switch_protect_bit;
#endif
#if defined(USE_TIMER_POWER_OFF_FUNC)
extern bool timer_setting_enable;
#endif

#if defined(PWR_CTRL_WKUP)
extern void wkup_pin_ctrl(bool dir);
#endif

extern void rtc_disp_hdlr(void);
extern xd_u8 rtc_setting,rtc_set,rtc_set_cnt;

extern bool repeat_off_flag;

extern xd_u8 rew_play_timer,disp_play_filenum_timer;
bool folder_select=0,folder_mode_select=0;

#ifdef USE_USB_PROG_PLAY_MODE
extern bool prog_disp_srn;
bool usb_play_prog_mode=0,usb_prog_icon_bit=0,usb_prog_play=0;
xd_u8 usb_prog_total_num=0,usb_prog_cur_num=0;
xd_u8 usb_play_prog_index=0;
xd_u8 usb_prog_tab[20]={0};
extern xd_u8 prog_exit_timer;

extern u8 ReadLFSR();

bool get_prog_song_num(u8 get_Mode)
{

#if 0
	if(get_Mode==GET_PREV_FILE){

		if(usb_play_prog_index>0){
			usb_play_prog_index--;
		}
		else{
			usb_play_prog_index=usb_prog_total_num-2;
		}
	}
	else{			
		usb_play_prog_index++;
	
		if(usb_play_prog_index>(usb_prog_total_num-2)){

			usb_play_prog_index =0;

			if(get_Mode==GET_PLAY_FILE){
				repeat_off_flag =1;
				return 0;		
			}			
		}
	}

#else
	repeat_off_flag =0;

	if(play_mode == REPEAT_ALL){

		if(get_Mode==GET_PREV_FILE){

			if(usb_play_prog_index>0){
				usb_play_prog_index--;
			}
			else{
				usb_play_prog_index=usb_prog_total_num-2;
			}
		}
		else{			
			usb_play_prog_index++;
	
			if(usb_play_prog_index>=usb_prog_total_num){
				usb_play_prog_index =0;
			}
		}
	}
	else if(play_mode == REPEAT_ONE){


	}
	else if(play_mode == REPEAT_OFF){

		if(get_Mode==GET_PREV_FILE){

			if(usb_play_prog_index>0){
				usb_play_prog_index--;
			}
			else{
				usb_play_prog_index=usb_prog_total_num-2;
			}
		}
		else{

			usb_play_prog_index++;
	
			if(usb_play_prog_index>=usb_prog_total_num){
				usb_play_prog_index =0;
			}			
		}
		
			
		if(get_Mode==GET_PLAY_FILE){
			repeat_off_flag =1;
			return 0;		
		}			
		
	}
	else if(play_mode == REPEAT_RANDOM){
		
	        CRCREG = T3CNTH;
	        CRCREG = T3CNTL;

	        usb_play_prog_index = ReadLFSR();
	        usb_play_prog_index = usb_play_prog_index % (usb_prog_total_num-1)+ 1;
	}	
#endif
	given_file_number =usb_prog_tab[usb_play_prog_index];

#ifdef MP3_UART_ENABLE
    printf(" ---> 1111	get_prog_song_num	IDX%x    FILE NUM %x  \r\n",(u16)usb_play_prog_index,(u16)usb_prog_total_num);
#endif
	
#ifdef MP3_UART_ENABLE
    printf(" ---> 2222	get_prog_song_num	%x \r\n",(u16)given_file_number);
#endif

	return 1;

}
void usb_prog_play_init()
{
	if(!usb_play_prog_mode){
		
		usb_play_prog_mode=1;
		usb_prog_icon_bit=1;	
		prog_exit_timer=14;	

		usb_play_prog_index=0;
		usb_prog_total_num=1;
		usb_prog_cur_num=0;
		prog_disp_srn=1;
		my_memset(&usb_prog_tab[0], 0x0, 20);
		Disp_Con(DISP_PROG_FILENUM);
	}
}
void usb_prog_mode_cls()
{
	if(usb_play_prog_mode||usb_prog_icon_bit){

		usb_play_prog_mode=0;
		usb_prog_icon_bit=0;	
		prog_disp_srn=1;

		usb_prog_total_num=1;
		usb_prog_cur_num=0;
		prog_exit_timer=0;	
		
		my_memset(&usb_prog_tab[0], 0x0, 20);
		Disp_Con(DISP_STOP);
	}
}
void usb_prog_hdlr(u8 key)
{
	switch(key)
	{
	        case INFO_MODE | KEY_SHORT_UP :

			prog_exit_timer=PROG_EXIT_TIMER;	
			if((usb_prog_cur_num!=0)&&(usb_prog_total_num<20)){
				usb_prog_tab[usb_prog_total_num-1]=usb_prog_cur_num;
				usb_prog_cur_num=0;
				usb_prog_total_num++;	
				prog_disp_srn=1;

#ifdef MP3_UART_ENABLE
    printf(" ---> usb_prog_hdlr	%x \r\n",(u16)usb_prog_total_num);
#endif
				Disp_Con(DISP_PROG_FILENUM);
				
			}
			break;		
	        case INFO_NEXT_FIL | KEY_SHORT_UP:
			prog_exit_timer=PROG_EXIT_TIMER;	
			usb_prog_cur_num++;
			if(usb_prog_cur_num>fs_msg.fileTotal){
				usb_prog_cur_num=1;
			}
			prog_disp_srn=0;			
			Disp_Con(DISP_PROG_FILENUM);
			break;
	        case INFO_PREV_FIL | KEY_SHORT_UP:

			prog_exit_timer=PROG_EXIT_TIMER;	
			usb_prog_cur_num--;
				
			if((usb_prog_cur_num==0)||(usb_prog_cur_num>fs_msg.fileTotal)){
				usb_prog_cur_num=fs_msg.fileTotal;
			}			
			prog_disp_srn=0;						
			Disp_Con(DISP_PROG_FILENUM);			
			break;
	        case INFO_POWER| KEY_SHORT_UP:
			usb_play_prog_mode=0;		
			prog_disp_srn=0;						
			break;
		 case MSG_USB_DISK_OUT: 
		 	usb_prog_mode_cls();
			 Remov_Func_From_List(USB_DEV);
		        break;			
		 case INFO_HALF_SECOND:
		 	
		 	 if(prog_exit_timer>0){
			 	prog_exit_timer--;
			 	if(prog_exit_timer==0){

		 			usb_prog_mode_cls();
				}
		 	 }
		        break;			
	}
}


#endif

#ifdef USE_USB_ERP_2_HDLR
xd_u16 usb_erp_timer=0;
void usb_erp_2_timer_hdlr()
{
	erp_2_test_mode_handlr();
	if((play_status == MUSIC_STOP)||(play_status == MUSIC_PAUSE)){

		usb_erp_timer++;
		if(usb_erp_timer>=10*60*2){

    			CD_PWR_GPIO_OFF();
		    	Disp_Con(DISP_POWER_OFF);
#ifdef USE_POWER_KEY				
			sys_power_down();
#endif
		}
	}
	else{
		usb_erp_timer = 0;
	}
}
#endif


/*----------------------------------------------------------------------------*/
/**@brief MUSIC解码信息初始化
   @param 无
   @return  无
   @note void music_info_init(void)
*/
/*----------------------------------------------------------------------------*/
void music_info_init(void)
{
#ifdef USB_SD_DECODE_DEFAULT_REP_OFF
    	play_mode=REPEAT_OFF;
#else
    play_mode = read_info(MEM_PLAY_MODE);
    if (play_mode > REPEAT_RANDOM)
    {
        play_mode = REPEAT_ALL;
    }
#endif
    if (given_device == 0)		  //设备启动时，given_device为0；
    {
        given_device = read_info(MEM_ACTIVE_DEV);
        if ((given_device != 0x01) && (given_device != 0x02))
        {
            given_device = 0x01;
            given_file_number = 1;
        }
    }

    eq_mode = read_info(MEM_EQ_MODE);
    if (eq_mode > CLASSIC)
    {
        eq_mode = NORMAL;
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   停止解码
   @param   无
   @return  无
   @note   stop_decode
*/
/*----------------------------------------------------------------------------*/
void stop_decode(void)
{

    Mute_Ext_PA(MUTE);
    play_status = MUSIC_STOP;
	//main_vol_set(0, CHANGE_VOL_NO_MEM);//digital_fade_out();
    read_usb_remain_data();
    disable_decode_isr();
    disable_softint();
    cfilenum = 0;
#ifdef MP3_UART_ENABLE
	sys_printf("  STOP DECODE ");
#endif
	
}

/*----------------------------------------------------------------------------*/
/**@brief  开始解码
   @param  无
   @return 无
   @note   start_decode
*/
/*----------------------------------------------------------------------------*/
bool start_decode(void)
{

#ifdef PWR_CTRL_IN_IDLE_MODE

#if defined(PWR_CTRL_WKUP)
	wkup_pin_ctrl(1);
#else
	PWR_CTRL_GPIO_INIT();
	PWR_CTRL_EN();
#endif

#endif

    	if(folder_select){
		Disp_Con(DISP_DIR);
    	}
   	else
	{
		Disp_Con(DISP_FILENUM);
    	}
	
#if FILE_ENCRYPTION
    if((fs_msg.fname[8]== 'S')&&(fs_msg.fname[9]== 'M')&&(fs_msg.fname[10]== 'P'))
    {
        password_start(1);
    }
    else
    {
        password_start(0);
    }
#endif
    mad_control(0,(u16)buffer);
    set_eq(eq_mode);                             //必须在每首歌曲播放前初始化EQ
    music_type = musice_format_check(buffer);

#if EQ_WORK_USE == DSP_EQ
    sysclock_div2(0);
#ifndef NO_SD_DECODE_FUNC						
    sd_speed_init(1, 100);
#endif
#else
	if (music_type == 2) 							//wav
	{
    		sysclock_div2(0);
#ifndef NO_SD_DECODE_FUNC						
    		sd_speed_init(1, 100);
#endif
	}
	else
	{
		sysclock_div2(1);
#ifndef NO_SD_DECODE_FUNC			
    		sd_speed_init(0, 50);
#endif
	}
#endif
    	get_filetag(buffer);
    	my_memset(win_buffer, 0, 512);              //解码前，必须清除buffer
  	write_info(MEM_ACTIVE_DEV,device_active);	
//breakpoint
	load_playpoint();
	write_playpoint_info(device_active);
    	update_playpoint(&playpoint_time);		//半秒更新断点进度，不写入存储器
	write_dev_playtime(device_active);			 //更新断点信息
    	playpoint_filenum = 0;
//breakpoint
	flush_low_msg();
    	play_status = MUSIC_PLAY;
    	decode_cmd = 0;
    	decode_user_exit(0);				  //非正常歌曲可通过按键强行停止解码,每次播放前必须清除
    	enable_softint();					   
    	enable_decode_isr();
    	if (2 == music_type)                   //wav文件，
    	{
     		 delay_10ms(5);
    	}
	set_sys_vol(my_music_vol);
	Mute_Ext_PA(UNMUTE);
	
    	cfilenum = 0;
    	return 1;
}

/*----------------------------------------------------------------------------*/
/**@brief  解码播放主循环,并处理信息
   @param  无
   @return 无
   @note   void music_play(void)
*/
/*----------------------------------------------------------------------------*/
/* for get ir user code
extern u16 debug_ir;
*/

void music_play(void)
{
    u8 key;
    u8 file_end_time;

#ifdef ADKEY_SELECT_MODE
    mode_switch_protect_bit=0;
#endif	
    while (1)
    {	   		
		if (play_status == MUSIC_PLAY)
		{
			dac_out_select(DAC_DECODE);
		}
		//suspend_sdmmc();

		key = get_msg();
		
#ifdef USE_USB_PROG_PLAY_MODE
		if(usb_play_prog_mode){
			
			usb_prog_hdlr(key);
			if((key==INFO_HALF_SECOND)||(key==(INFO_MODE | KEY_SHORT_UP))||(key==(INFO_NEXT_FIL | KEY_SHORT_UP))||(key==(INFO_PREV_FIL | KEY_SHORT_UP))){
				key = 0xFF;
			}		
		}
#endif	

        switch (key)
        {
        case INFO_NEXT_SYS_MODE:
		return;
		
        case INIT_PLAY:                                 //开始解码播放
            	file_end_time = 0;
            	stop_decode();
		playpoint_flag = 0;
              if(playpoint_filenum)
		{
		    playpoint_flag = 1;
			given_file_number = playpoint_filenum;
		}
#if FILE_ENCRYPTION
            password_start(0);
#endif
            	if (!fs_getfile_bynumber(given_file_number))
            	{
			put_msg_lifo(INFO_NEXT_FIL | KEY_SHORT_UP);
                	break;
            	}

            	if (!start_decode())
            	{
			put_msg_lifo(INFO_NEXT_FIL | KEY_SHORT_UP);
            	}	
#ifdef USE_USB_PROG_PLAY_MODE
		if(usb_prog_icon_bit){
			usb_prog_play=1;
		}
#endif
				
            	break;

        case SEL_GIVEN_DEVICE_GIVEN_FILE:              ///<获取指定设备的指定文件
		get_music_file2();
            	break;
        case INFO_STOP| KEY_SHORT_UP :
#ifdef USE_USB_PROG_PLAY_MODE

		if(usb_prog_play){
				
				usb_prog_play=0;
				usb_play_prog_index =0;
				
				flush_all_msg();
				stop_decode();
				Disp_Con(DISP_STOP);
   			 //printf(" ---> usb_prog_icon_bit	%x \r\n",(u16)usb_prog_icon_bit);
				play_mode  =REPEAT_OFF;
				
				break;
		}
		usb_prog_mode_cls();
#endif

		if((get_device_online_status()==0)){
			break;
		}

		Mute_Ext_PA(MUTE);            
		disp_play_filenum_timer=6;
		play_mode  =REPEAT_OFF;

		flush_all_msg();
		stop_decode();
		Disp_Con(DISP_STOP);			
		break;
        case INFO_NEXT_FIL | KEY_SHORT_UP:
			
#ifdef USE_FOLDER_SELECT_FUNC
		if(folder_select){
			
			select_folder_file(FIND_NEXT_DIR);
		       Disp_Con(DISP_DIR);	
			break;			   
		}
#endif
		disp_play_filenum_timer=10;

		get_music_file1(GET_NEXT_FILE);
            	break;

        case INFO_PREV_FIL | KEY_SHORT_UP:
			
#ifdef USE_FOLDER_SELECT_FUNC
		if(folder_select){
			
			select_folder_file(FIND_PREV_DIR);
		       Disp_Con(DISP_DIR);	
			break;
		}
#endif		

		if(rew_play_timer>0){

			rew_play_timer=0;
                	put_msg_lifo(INIT_PLAY);
		}
		else
		{
			disp_play_filenum_timer=10;
			get_music_file1(GET_PREV_FILE);
		}
            	break;
        case INFO_NEXT_FIL | KEY_HOLD:
			
		if(play_status)
		{
			ff_fr_step = FAST_FARWORD_STEP;
			play_status = MUSIC_FF_FR;
#if (FF_FR_MUSIC)
			if(0 == decode_cmd)
#endif
			{			
				analog_vol_set(0);
				decode_cmd = 0xf0;
			}
            	}
		break;
        case INFO_PREV_FIL | KEY_HOLD:
	       if(music_type == 2)    //wav文件不支持断点记忆，故可以快退
	       { 
	             playpoint_flag =0;
	       }
			
		if(!playpoint_flag) 				//读取断点信息后不支持快退
		{
			if(play_status)
			{
				ff_fr_step = -FAST_FARWORD_STEP;
				play_status = MUSIC_FF_FR;
#if (FF_FR_MUSIC)
				if(0 == decode_cmd)
#endif					
				{
					analog_vol_set(0);
					decode_cmd = 0xf1;
				}
			}    
		}
		break;

        case DECODE_MSG_DISK_ERR:
#ifdef MP3_UART_ENABLE
			sys_printf(" DECODE_MSG_DISK_ERR");
#endif
			
		get_music_file3();
            	break;

        case DECODE_MSG_FILE_END:               ////*将文件结束的消息放到半秒消息中处理，防止单曲循环遇到错误歌曲无法手动切换下一首,或遇到错误歌曲无法到上一首,每首歌曲至少播放约5S*/
		if (get_music_play_time() < 5)
		{
			file_end_time = 7 - (u8)get_music_play_time();
            	}
		else
		{	

#ifdef MP3_UART_ENABLE
			sys_printf(" DECODE_MSG_FILE_END");
#endif
		
			get_music_file1(GET_PLAY_FILE);
		}
		break;
    	case MSG_USB_DISK_IN  :
		Disp_Con(DISP_SCAN_DISK);	
        	disp_scenario = DISP_NORMAL;
	 	//Add_Func_To_List(USB_DEV);
        	given_device = BIT(USB_DISK);
        	put_msg_lifo(SEL_GIVEN_DEVICE_GIVEN_FILE);	
	break;   
	
        case INFO_NEXTMODE:                     ///<下一个模式
		//work_mode = SYS_IDLE;
            //return;
	        stop_decode();
#ifdef NO_DEV_SHOW_NO_DEV
		usb_prog_mode_cls();
		folder_select=0;
		folder_mode_select=0;
		Disp_Con(DISP_NODEVICE);
#else
             	Disp_Con(DISP_RTC);
		disp_scenario = DISP_RTC_SCEN;
#endif		
		break;
        case INFO_PLAY | KEY_SHORT_UP :
			
#if defined(USE_TIMER_POWER_OFF_FUNC)
		if(timer_setting_enable){
			timer_pwr_setting();
			break;
		}
#endif		
		if((get_device_online_status()&0x03)==0){
			break;
	     }		
            if (DISP_DWORD_NUMBER == curr_menu)
            {
                	Disp_Con(DISP_NULL);
			put_msg_fifo(INFO_PICK_SONG | KEY_SHORT_UP);
                	break;
            }
			
            if (play_status == MUSIC_PAUSE)
            {
			Mute_Ext_PA(UNMUTE);
			play_status = MUSIC_PLAY;
			Disp_Con(DISP_PLAY);
                	mad_control(MAD_PLAY,0);
			if(music_type == 1)
				enable_decodeint();
			if(music_type == 2)
				enable_softint();
            }
            else if (play_status == MUSIC_PLAY)
            {
			Mute_Ext_PA(MUTE);            
			play_status = MUSIC_PAUSE;
			if(music_type == 1)
				disable_decodeint();
			Disp_Con(DISP_PAUSE);
                	mad_control(MAD_PAUSE,0);
                	read_usb_remain_data();
			write_playtime(&playpoint_time);	   	//暂停时记忆断点信息（EEPROM）
            }
            else if (play_status == MUSIC_STOP){

			disp_play_filenum_timer=10;
			
#ifdef USE_USB_PROG_PLAY_MODE
			if(usb_play_prog_mode){

				if(usb_prog_tab[0]!=0){
					given_file_number =usb_prog_tab[0];

				}
				else{
					usb_prog_mode_cls();
				}
				usb_play_prog_mode=0;
			}				
#endif
                	put_msg_lifo(INIT_PLAY);
			//Mute_Ext_PA(UNMUTE);

	     }
            break;

        case INFO_PICK_SONG | KEY_SHORT_UP :
            given_file_number = (u16)cfilenum;           
            cfilenum = 0;
	     if ((given_file_number > 0) && (given_file_number <= fs_msg.fileTotal))
            {
                	put_msg_lifo(INIT_PLAY);
                	break;
            }
	     Disp_Con(DISP_ERROR);
            break;
       case INFO_HALF_SECOND :
	   	
#if ((USE_DEVICE == MEMORY_STYLE)&&(FAT_MEMORY))      
            updata_fat_memory();
#endif

#if defined(USE_BAT_MANAGEMENT)
	     bmt_hdlr();
#endif
#ifdef USE_USB_ERP_2_HDLR
	     usb_erp_2_timer_hdlr();
#endif

#if defined(USE_TIMER_POWER_OFF_FUNC)
	       timer_pwr_off_hdlr();
#endif

            set_brightness_fade_out();
	     update_playpoint(&playpoint_time);		//半秒更新断点进度，不写入存储器

#if 1//defined(USE_FOLDER_SELECT_FUNC)
		if(folder_select){
           	 	return_cnt++;
            		if (RETURN_TIME == return_cnt){
				return_cnt=4;
				folder_select=0;
			}
			else{
				break;
			}
		}
#endif		
	    if(adkey_detect){
	   	    adkey_detect=0;
	   	    set_sys_vol(my_music_vol);
	    }

	     if(disp_play_filenum_timer>0)disp_play_filenum_timer--;
		 
            if (file_end_time)
            {
			file_end_time--;
                	if (file_end_time == 0)
			{
                    		get_music_file1(GET_PLAY_FILE);		
			}
            }
#if FF_FR_MUSIC	   ///在每一次快进完成之后，播出半秒左右的声音
	     if ((2 == decode_cmd)&&(play_status == MUSIC_FF_FR))
	     {
			decode_cmd = 0;
			set_eq(eq_mode);
			play_status = MUSIC_PLAY;
	     }

#else
            if ((decode_cmd != 0) && (decode_cmd <= 2))		
            {
                	decode_cmd--;
			set_eq(eq_mode);
                	if (decode_cmd == 0) 					///<快进快退结束后1S，打开主音量
		  	{
				play_status = MUSIC_PLAY;
		   	}
            }
#endif
#ifdef RTC_DISP_IN_IDLE_MODE

	    if(disp_scenario == DISP_RTC_SCEN){

		    	rtc_setting_exit();

			if(curr_menu != DISP_RTC){
            			return_cnt++;
            			if (RETURN_TIME == return_cnt){
					rtc_disp_hdlr();
				}
				break;
			}
			rtc_disp_hdlr();
			break;
	     }
#endif	

#ifdef NO_DEV_SHOW_NO_DEV
		if((get_device_online_status()&0x03)==0){
			Disp_Con(DISP_NODEVICE);
			break;
		}
#endif            

            if (DISP_PLAY == curr_menu)
            {
			rew_play_timer=1;				            
                	disp_file_time();
            }

            return_cnt++;
            if (RETURN_TIME == return_cnt)
            {

#if defined(USE_TIMER_POWER_OFF_FUNC)	
			timer_setting_enable=0;
#endif

                if (DISP_DWORD_NUMBER == curr_menu)
                {
			Disp_Con(DISP_NULL);
                    	put_msg_fifo(INFO_PICK_SONG | KEY_SHORT_UP);
                }
                else
                {
                    if (play_status== MUSIC_PAUSE){
                        if (curr_menu != DISP_PAUSE)
                            Disp_Con(DISP_PAUSE);
                    }
                    else if(play_status== MUSIC_PLAY){
                        if (curr_menu != DISP_PLAY){

				if(disp_play_filenum_timer==0)
                            Disp_Con(DISP_PLAY);
				else
                            Disp_Con(curr_menu);
			   }
                    }
		      else if(play_status== MUSIC_STOP){

                        if (curr_menu != DISP_STOP)
                            Disp_Con(DISP_STOP);
			  }
                }
            }
#if 1//( USE_RTCRAM == MEMORY_STYLE  )
			save_playpoint(2);	   	//2*0.5 = 1s 1s记录一次播放进度,
#endif
		break;

	 case INFO_MODE | KEY_LONG:

		if((get_device_online_status()==0)){
			break;
		}
#if 0		
#ifdef USE_USB_PROG_PLAY_MODE
		if(play_status == MUSIC_STOP){
			usb_prog_play_init();
			break;
		}
#endif
#endif
		folder_mode_select=~folder_mode_select;
		folder_select=folder_mode_select;
		if(folder_mode_select){
#ifdef USE_USB_PROG_PLAY_MODE
			usb_prog_mode_cls();
#endif		
		       Disp_Con(DISP_DIR);	
		}
		else{
		       Disp_Con(DISP_PLAY);	
		}
		break;
	 case INFO_MODE | KEY_SHORT_UP:

		if((get_device_online_status()==0)){
			break;
		}
#ifdef USE_USB_PROG_PLAY_MODE
		if(play_status == MUSIC_STOP){
			usb_prog_play_init();
			break;
		}
#endif
        case INFO_PLAY_MODE :

		//if(usb_play_prog_mode||usb_prog_icon_bit){
		//	 break;
		//}			
		
		play_mode++;
            	if (play_mode > REPEAT_END)
            	{
                	play_mode = REPEAT_HEAD;
            	}
#ifndef NO_PLAY_MODE_STR_DISP				
            write_info(MEM_PLAY_MODE,play_mode);
            Disp_Con(DISP_PLAYMODE);
#endif			
            break;
			
	    case INFO_EQ_UP | KEY_SHORT_UP :
               
	        eq_mode++;
	        if (eq_mode > CLASSIC)
	        {
	            eq_mode = NORMAL;
	        }
	        write_info(MEM_EQ_MODE,eq_mode);
	        set_eq(eq_mode);
	        Disp_Con(DISP_EQ);
	        break;
#if 0			
    	case INFO_MODE | KEY_LONG:
		if(disp_scenario == DISP_RTC_SCEN){

			if(rtc_setting==0){
				rtc_setting = 0x01;
				rtc_set = 3;
	                	rtc_set_cnt=30;
			}
		}			
		break;
#endif		
#if RTC_ENABLE 
	 case INFO_ALM_BELL :					///<闹钟响消息，进入闹钟循环
            	if (play_status)
            	{
				/**/
			if(music_type == 1)
				disable_decodeint();
                
			mad_control(MAD_PAUSE,0);
                	read_usb_remain_data();
            	}
            
		if(alm_bell_mode())
		{
			if (play_status)
		       {
	              	mad_control(MAD_PLAY,0);
				if(music_type == 1)
					enable_decodeint();
				if(music_type == 2)
					enable_softint();			
	            	}
			return;	
		}

            	if (play_status)
            	{
                	mad_control(MAD_PLAY,0);
			if(music_type == 1)
				enable_decodeint();
			if(music_type == 2)
				enable_softint();			
            	}
#ifdef PWR_CTRL_IN_IDLE_MODE

#if defined(PWR_CTRL_WKUP)
		wkup_pin_ctrl(1);
#else
		PWR_CTRL_GPIO_INIT();
		PWR_CTRL_EN();
#endif

#endif
		break;
#endif			
        default :
_HOT_KEY_HDLR:		
            if (!ap_handle_hotkey(key))
                return;
            if ((!play_status) && (!device_check()))
            {
		if(disp_scenario == DISP_NORMAL){

		  	stop_decode();
	             	Disp_Con(DISP_RTC);
			disp_scenario = DISP_RTC_SCEN;
		  }
                play_status = MUSIC_PLAY;
                //return;
            }
        }
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   播放解码模式入口
   @param   无
   @return  无
   @note    decode_play
*/
/*----------------------------------------------------------------------------*/
void decode_play(void)
{
#ifdef MP3_UART_ENABLE
	sys_printf(" SYS GO IN DECODE MODE");
#endif
	Mute_Ext_PA(MUTE);

	disp_play_filenum_timer=10;
	folder_select=0;
	folder_mode_select=0;
	rtc_setting=0;
	disp_scenario = DISP_NORMAL;
	Disp_Con(DISP_SCAN_DISK);
	sysclock_div2(1);
#ifndef NO_SD_DECODE_FUNC	
    	sd_speed_init(0, 50);
#endif
		
	decodeclock_div2(DECODE_CLK_DIV2);				//decoder分频，可以减少功耗
    	music_info_init();
    	dsp_hi_pro();
    	decodeint_hi_pro();
    	device_active = 0;
    	put_msg_lifo(SEL_GIVEN_DEVICE_GIVEN_FILE);
	set_max_vol(MAX_ANALOG_VOL-DECODE_ANALOG_VOL_CUT, MAX_DIGITAL_VOL);			//设置Music模式的音量上限
    //suspend_sdmmc();
	music_play();

	Mute_Ext_PA(MUTE);

#ifdef ADKEY_SELECT_MODE
    	mode_switch_protect_bit=1;
#endif
	disp_scenario = DISP_NORMAL;

	folder_select=0;
	folder_mode_select=0;
	
	stop_decode();
	usb_prog_mode_cls();	
#if(MEMORY_STYLE != USE_DEVICE)
	usb_suspend();			//Entered Suspend mode
#endif
	write_playtime(&playpoint_time);				//记录断点信息（EEPROM）
	main_vol_set(0, CHANGE_VOL_NO_MEM);
}
#else
void stop_decode(void)
{
}
#endif

