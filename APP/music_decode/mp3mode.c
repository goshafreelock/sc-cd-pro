/*--------------------------------------------------------------------------*/
/**@file     mp3mode.c
   @brief    ��������ģ��
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/

/*mp3mode.c*/

#include "mp3mode.h"
#include "fat_memory.h"
#include "config.h"
#include "voice_time.h"
extern u8 _xdata buffer[1024];              //<����BUFF
extern u8 _xdata win_buffer[512];           //<�ļ�ϵͳ��BUFF
extern FSAPIMSG _pdata fs_msg;              //<�����ļ��ļ�ϵͳ��Ϣ�Ľṹ�����
extern _xdata u8 filename_buff[];
extern bool get_filename( u8 _xdata *p );
extern MAD_DECODE_INFO _pdata mad_decode_dsc;
extern xd_u8 return_cnt;
extern xd_u16 cfilenum;
extern xd_u8 work_mode;
extern xd_u8 curr_menu,disp_scenario;
extern u8 device_active;
extern u8 decode_cmd;

extern s8 _idata ff_fr_step;  ///<��� ���˲��� ����Ϊ���� ����Ϊ���

//bool play_status;       ///< ����״̬ 1:����;0:��ͣ;
xd_u8 play_status;			///< ����״̬ 0����ͣ��1�����ţ�2�����/���ˣ�
xd_u8 given_device;        ///< ָ����Ҫ��ȡ���豸 1 SD ; 2 USB; 0:��֪���豸���������Ƿ�ֵ
/** ��Чģʽ */
xd_u8 eq_mode;
/** ѭ��ģʽ */
xd_u8 play_mode;
/** ָ���ҵ����ļ��� */
xd_u16 given_file_number;
/** ��ǰ���벥�ŵ��ļ��ĸ�ʽ */
u8 music_type;
/** �ļ���Ϣ������ʾ�ļ���ֵ */
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
extern volatile u8 bDACTimeOut;			//�ȴ�OBUF ���timeout
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

/*----------------------------------------------------------------------------*/
/**@brief MUSIC������Ϣ��ʼ��
   @param ��
   @return  ��
   @note void music_info_init(void)
*/
/*----------------------------------------------------------------------------*/
void music_info_init(void)
{

    play_mode = read_info(MEM_PLAY_MODE);
    if (play_mode > REPEAT_RANDOM)
    {
        play_mode = REPEAT_ALL;
    }

    if (given_device == 0)		  //�豸����ʱ��given_deviceΪ0��
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
/**@brief   ֹͣ����
   @param   ��
   @return  ��
   @note   stop_decode
*/
/*----------------------------------------------------------------------------*/
void stop_decode(void)
{
    play_status = MUSIC_PAUSE;
	main_vol_set(0, CHANGE_VOL_NO_MEM);//digital_fade_out();
    read_usb_remain_data();
    disable_decode_isr();
    disable_softint();
    cfilenum = 0;
}

/*----------------------------------------------------------------------------*/
/**@brief  ��ʼ����
   @param  ��
   @return ��
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

    if(disp_scenario== DISP_NORMAL)
    	Disp_Con(DISP_FILENUM);	
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
    set_eq(eq_mode);                             //������ÿ�׸�������ǰ��ʼ��EQ
    music_type = musice_format_check(buffer);

#if EQ_WORK_USE == DSP_EQ
    sysclock_div2(0);
    sd_speed_init(1, 100);
#else
	if (music_type == 2) 							//wav
	{
    		sysclock_div2(0);
    		sd_speed_init(1, 100);
	}
	else
	{
		sysclock_div2(1);
    		sd_speed_init(0, 50);
	}
#endif
    get_filetag(buffer);
    my_memset(win_buffer, 0, 512);              //����ǰ���������buffer
  	write_info(MEM_ACTIVE_DEV,device_active);	
//breakpoint
	load_playpoint();
	write_playpoint_info(device_active);
    update_playpoint(&playpoint_time);		//������¶ϵ���ȣ���д��洢��
	write_dev_playtime(device_active);			 //���¶ϵ���Ϣ
    	playpoint_filenum = 0;
//breakpoint
	flush_low_msg();
    	play_status = MUSIC_PLAY;
    	decode_cmd = 0;
    	decode_user_exit(0);				  //������������ͨ������ǿ��ֹͣ����,ÿ�β���ǰ�������
    	enable_softint();					   
    	enable_decode_isr();
    if (2 == music_type)                   //wav�ļ���
    {
      delay_10ms(5);
    }
    	cfilenum = 0;
    	return 1;
}

/*----------------------------------------------------------------------------*/
/**@brief  ���벥����ѭ��,��������Ϣ
   @param  ��
   @return ��
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

        switch (key)
        {
        case INFO_NEXT_SYS_MODE:
		return;
		
        case INIT_PLAY:                                 //��ʼ���벥��
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
            	break;

        case SEL_GIVEN_DEVICE_GIVEN_FILE:              ///<��ȡָ���豸��ָ���ļ�
		get_music_file2();
            	break;

        case INFO_NEXT_FIL | KEY_SHORT_UP:
	    	if((disp_scenario == DISP_RTC_SCEN)&&(rtc_setting==1)){
			goto _HOT_KEY_HDLR;
	    	}			
		get_music_file1(GET_NEXT_FILE);
            	break;

        case INFO_PREV_FIL | KEY_SHORT_UP:
	    	if((disp_scenario == DISP_RTC_SCEN)&&(rtc_setting==1)){
			goto _HOT_KEY_HDLR;
	    	}			
		get_music_file1(GET_PREV_FILE);
            	break;
        case INFO_NEXT_FIL | KEY_HOLD:
			
	    	if((disp_scenario == DISP_RTC_SCEN)&&(rtc_setting==1)){
			goto _HOT_KEY_HDLR;
	    	}

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
            if(music_type == 2)    //wav�ļ���֧�ֶϵ���䣬�ʿ��Կ���
            { 
                playpoint_flag =0;
            }
			
	    	if((disp_scenario == DISP_RTC_SCEN)&&(rtc_setting==1)){
			goto _HOT_KEY_HDLR;
	    	}
			
		if(!playpoint_flag) 				//��ȡ�ϵ���Ϣ��֧�ֿ���
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
		get_music_file3();
            	break;

        case DECODE_MSG_FILE_END:               ////*���ļ���������Ϣ�ŵ�������Ϣ�д�������ֹ����ѭ��������������޷��ֶ��л���һ��,��������������޷�����һ��,ÿ�׸������ٲ���Լ5S*/
		if (get_music_play_time() < 5)
		{
			file_end_time = 7 -  (u8)get_music_play_time();
            	}
		else
		{	
			get_music_file1(GET_PLAY_FILE);
		}
		break;

        case INFO_NEXTMODE:                     ///<��һ��ģʽ
		//work_mode = SYS_IDLE;
            //return;
             	Disp_Con(DISP_RTC);
		disp_scenario = DISP_RTC_SCEN;
		break;
        case INFO_PLAY | KEY_SHORT_UP :
			
#if defined(USE_TIMER_POWER_OFF_FUNC)
		if(timer_setting_enable){
			timer_pwr_setting();
			break;
		}
#endif		
	     if(get_device_online_status()==0){
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
			play_status = MUSIC_PAUSE;
			if(music_type == 1)
				disable_decodeint();
			Disp_Con(DISP_PAUSE);
                	mad_control(MAD_PAUSE,0);
                	read_usb_remain_data();
			write_playtime(&playpoint_time);	   	//��ͣʱ����ϵ���Ϣ��EEPROM��
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

#if defined(USE_TIMER_POWER_OFF_FUNC)
	       timer_pwr_off_hdlr();
#endif

            set_brightness_fade_out();
	     update_playpoint(&playpoint_time);		//������¶ϵ���ȣ���д��洢��
	     
            if (file_end_time)
            {
			file_end_time--;
                	if (file_end_time == 0)
			{
                    		get_music_file1(GET_PLAY_FILE);		
			}
            }
#if FF_FR_MUSIC	   ///��ÿһ�ο�����֮�󣬲����������ҵ�����
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
                	if (decode_cmd == 0) 					///<������˽�����1S����������
		  	{
				play_status = MUSIC_PLAY;
		   	}
            }
#endif

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
		
            if (DISP_PLAY == curr_menu)
            {
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
                    if (!play_status)
                    {
                        if (curr_menu != DISP_PAUSE)
                            Disp_Con(DISP_PAUSE);
                    }
                    else
                    {
                        if (curr_menu != DISP_PLAY)
                            Disp_Con(DISP_PLAY);
                    }
                }
            }
#if ( USE_RTCRAM == MEMORY_STYLE  )
			save_playpoint(2);	   	//2*0.5 = 1s 1s��¼һ�β��Ž���,
#endif
		break;

        case INFO_PLAY_MODE :
		play_mode++;
            	if (play_mode > REPEAT_ONE)
            	{
                	play_mode = REPEAT_ALL;
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
	 case INFO_ALM_BELL :					///<��������Ϣ����������ѭ��
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
/**@brief   ���Ž���ģʽ���
   @param   ��
   @return  ��
   @note    decode_play
*/
/*----------------------------------------------------------------------------*/
void decode_play(void)
{
#ifdef UART_ENABLE
	sys_printf(" SYS GO IN DECODE MODE");
#endif
	rtc_setting=0;
	disp_scenario = DISP_NORMAL;
	Disp_Con(DISP_SCAN_DISK);
	sysclock_div2(1);
    	sd_speed_init(0, 50);
	decodeclock_div2(DECODE_CLK_DIV2);				//decoder��Ƶ�����Լ��ٹ���
    	music_info_init();
    	dsp_hi_pro();
    	decodeint_hi_pro();
    	device_active = 0;
    	put_msg_lifo(SEL_GIVEN_DEVICE_GIVEN_FILE);
	set_max_vol(MAX_ANALOG_VOL-DECODE_ANALOG_VOL_CUT, MAX_DIGITAL_VOL);			//����Musicģʽ����������
    //suspend_sdmmc();
	music_play();

#ifdef ADKEY_SELECT_MODE
    	mode_switch_protect_bit=1;
#endif
	disp_scenario = DISP_NORMAL;
	stop_decode();
#if(MEMORY_STYLE != USE_DEVICE)
	usb_suspend();			//Entered Suspend mode
#endif
	write_playtime(&playpoint_time);				//��¼�ϵ���Ϣ��EEPROM��
	main_vol_set(0, CHANGE_VOL_NO_MEM);
}
#else
void stop_decode(void)
{
}
#endif
