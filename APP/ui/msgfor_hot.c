/*--------------------------------------------------------------------------*/
/**@file    msgfor_hot.c
   @brief   HOT 消息处理模块
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#include "Custom_config.h"

#include "msgfor_hot.h"
#include "rtc_mode.h"
#include "config.h"
#include "mp3mode.h"
extern u8 _idata music_vol;
extern xd_u8 given_device;
extern xd_u16 given_file_number;
extern xd_u8 work_mode;
extern xd_u16 cfilenum;
extern xd_u8 eq_mode;
extern xd_u8 play_status;

extern xd_u8 rtc_mode;
extern xd_u8 alm_flag,rtc_set,alm_set;
extern bool alm_sw;
extern xd_u8 return_cnt;
xd_u8 report_nowtime;
xd_u8 last_work_mode;
bool voicetime_on=0;
extern RTC_TIME curr_date;
extern u8 device_active;

xd_u8 rtc_set_cnt=0;
xd_u8 disp_scenario=DISP_NORMAL;
xd_u8 rtc_setting=0;
bool sys_mute_flag=0;
xd_u8 key_100_flag=0;
xd_u8 my_sys_vol=0;

extern bool sys_pwr_flag;
extern xd_u8 cur_sw_fm_band;

extern void rtc_disp_hdlr(void);
extern void curr_date_minus();
extern void curr_date_plus();
extern void set_date_sec();
extern void alm_time_plus();
extern void alm_time_minus();
extern void set_alm_sec(void);


void Mute_Ext_PA(bool M_Type)
{
	AMP_MUTE_PORT_INIT();
	if(M_Type){
		AMP_MUTE_ENABLE();
	}
	else{
		AMP_MUTE_DISABLE();
	}
}
#if defined(USE_TIMER_POWER_OFF_FUNC)
bool timer_setting_enable=0;
xd_u8 timer_pwr_idx=0,timer_disp=0;
xd_u16 timer_pwr_cnt=0;
void timer_pwr_setting()
{
	timer_pwr_idx++;
	if(timer_pwr_idx>6)timer_pwr_idx=0;
	timer_pwr_cnt =0;
	timer_setting_enable=1;
	
	timer_disp=timer_pwr_idx*10;
	Disp_Con(DISP_PWR_TIMER);
	//delay_10ms(200);
}
void timer_pwr_off_hdlr()
{
	if(timer_pwr_idx>0){
		timer_pwr_cnt++;
	//printf( "---->timer_pwr_off_hdlr %x \r\n",(u16)timer_pwr_cnt);

		if(timer_pwr_cnt>=(timer_disp*2*60)){
			timer_pwr_idx =0;
			timer_pwr_cnt =0;
			//printf( "---->INFO_TIMER_OFF %x \r\n",(u16)timer_pwr_cnt);
            		//put_msg_lifo(INFO_CH_DOWN|KEY_SHORT_UP);
 			sys_pwr_flag =1;
       		set_brightness_all_on();
			work_mode = SYS_IDLE;
			put_msg_fifo(INFO_POWER | KEY_SHORT_UP);		
		}	
	}
}
#endif

void set_sys_vol(u8 vol)
{
	music_vol=vol;
	if (DAC_DECODE == dac_channel)
	{
		if (music_vol > max_digital_vol)
		        music_vol = max_digital_vol;
	}
	else	{

		if (music_vol > max_analog_vol)
		      	music_vol = max_analog_vol;
	}
	write_info(MEM_VOL, music_vol);
	main_vol_set(0, CHANGE_VOL_NO_MEM);
}
void rtc_setting_exit(void)
{
	//printf("--->rtc_setting_exit %x \r\n",(u16)rtc_set_cnt);
	if(rtc_set_cnt>0){

		rtc_set_cnt--;
		if(rtc_set_cnt==0){
			
			rtc_setting = 0;
#ifdef USE_RTC_ALARM_FUNCTION			
			 if(disp_scenario == DISP_ALM_SCEN){
				disp_scenario = DISP_RTC_SCEN;
			}
#endif			 
		}
	}

}
/*----------------------------------------------------------------------------*/
/**@brief   几个任务都会用到的消息集中处理的函数
   @param   key： 需要处理的消息
   @return  0：需要切换工作模式;
   @note    unsigned char I2C_sendbyte(unsigned char I2Cdata)
*/
/*----------------------------------------------------------------------------*/
u8 ap_handle_hotkey(u8 key)
{
    switch (key)
    {
#if 1   
#ifdef USE_RTC_ALARM_FUNCTION
        case INFO_PLAY | KEY_LONG:
		if(work_mode == SYS_IDLE){
			
			if(alm_sw){
				alm_sw=0;
			}
		}
		else{
#if defined(USE_TIMER_POWER_OFF_FUNC)
		timer_pwr_setting();
#endif		
		}
		break;
        case INFO_PLAY | KEY_SHORT_UP:
			
		if(work_mode == SYS_IDLE){
			
			if(disp_scenario == DISP_RTC_SCEN){
				
 	               	rtc_set_cnt=30;
				alm_set =3;
				disp_scenario = DISP_ALM_SCEN;
			}
			else{

				alm_set--;
				if(alm_set==0){
		                	rtc_set_cnt=0;						
					disp_scenario = DISP_RTC_SCEN;
				}
			}
			Disp_Con(DISP_ALARM);
		}
		
		break;
#endif 
        case INFO_PREV_FIL | KEY_SHORT_UP:
        case INFO_PREV_FIL | KEY_HOLD:
	      	if ((disp_scenario == DISP_RTC_SCEN)&&(rtc_setting!=0))
            	{
                	rtc_set_cnt=30;
                	curr_date_minus();
                	set_date_sec();
			Disp_Con(DISP_RTC);
            	}
#ifdef USE_RTC_ALARM_FUNCTION
	      else if (disp_scenario == DISP_ALM_SCEN){

                	rtc_set_cnt=30;
                	alm_time_minus();
                	set_alm_sec();
			Disp_Con(DISP_ALARM);

	      	}
#endif
    		break;
        case INFO_NEXT_FIL | KEY_SHORT_UP:
        case INFO_NEXT_FIL | KEY_HOLD:

	      	if ((disp_scenario == DISP_RTC_SCEN)&&(rtc_setting!=0))
            	{
                	rtc_set_cnt=30;
                	curr_date_plus();
                	set_date_sec();
			Disp_Con(DISP_RTC);
            	}
#ifdef USE_RTC_ALARM_FUNCTION
	      else if (disp_scenario == DISP_ALM_SCEN){
                	rtc_set_cnt=30;

                	alm_time_plus();
                	set_alm_sec();
			Disp_Con(DISP_ALARM);
	      	}
#endif
			
		break;
#endif
    
    case MSG_USB_DISK_OUT: 
        //Disp_Con(DISP_USBOUT);
        break;
    case MSG_SDMMC_OUT:
        //Disp_Con(DISP_SDOUT);
        break;

#ifdef USE_LINE_IN_DETECT_FUNC
    case MSG_AUX_IN :
        if (work_mode != SYS_USBDEVICE)
        {
            last_work_mode= work_mode;
            work_mode = SYS_AUX;
            return 0;
        }
        break;

    case MSG_AUX_OUT :
        if (SYS_AUX == work_mode)
        {
            work_mode = last_work_mode;
            return 0;
        }
        break;
#endif		
        //case MSG_USB_PC_OUT:
    case MSG_SDMMC_IN :
       // password_start(0);
        disp_scenario = DISP_NORMAL;
        given_device = BIT(SDMMC);
        put_msg_lifo(SEL_GIVEN_DEVICE_GIVEN_FILE);
        if ((work_mode != SYS_MP3DECODE) && (work_mode != SYS_USBDEVICE))
        {
            work_mode = SYS_MP3DECODE;
            return 0;
        }
        break;

    case MSG_USB_DISK_IN  :
       // password_start(0);
        disp_scenario = DISP_NORMAL;
        given_device = BIT(USB_DISK);
        put_msg_lifo(SEL_GIVEN_DEVICE_GIVEN_FILE);
        if (work_mode != SYS_MP3DECODE)
        {
            work_mode = SYS_MP3DECODE;
            return 0;
        }
        break;
#ifdef MODE_KEY_SEL_FUNC
#ifdef USE_POWER_KEY_SHORT_FOR_MODE
    case INFO_POWER | KEY_SHORT_UP :	
#else
    case INFO_MODE | KEY_SHORT_UP:
#endif
	if(work_mode==SYS_MCU_CD){
		
		work_mode = SYS_FMREV;
		cur_sw_fm_band=0;
	}
	else if(work_mode==SYS_FMREV){

		if(cur_sw_fm_band==0){

			cur_sw_fm_band=1;
			put_msg_lifo(INFO_NEXT_FM_MODE); 
			break;
		}
		else{
			work_mode = SYS_MCU_CD;
		}
	}	
	put_msg_lifo(INFO_NEXT_SYS_MODE); 
	
	break;
#else
    case INFO_MODE | KEY_SHORT_UP:
        if (work_mode == SYS_USBDEVICE)
            break;
#ifdef RTC_DISP_IN_IDLE_MODE
        if (work_mode == SYS_FMREV)
            break;
#else
        if (work_mode != SYS_MP3DECODE)
            break;
#endif		
		set_brightness_all_on();

		if(rtc_setting!=0){
			
                	rtc_set_cnt=30;	

			rtc_set++;
			
			if(rtc_set>4){
				rtc_set = 3;
	                	rtc_set_cnt=0;	
				rtc_setting = 0x00;
			}
			break;
		}
		
		if(disp_scenario == DISP_NORMAL){

				disp_scenario = DISP_RTC_SCEN;
				rtc_disp_hdlr();
		}
		else{
#ifdef RTC_DISP_IN_IDLE_MODE
		        if (work_mode == SYS_IDLE)
		            break;		
#endif				
			disp_scenario = DISP_NORMAL;
	              Disp_Con(DISP_PLAY);
		}
		break;
#if 1		
    case INFO_MODE | KEY_LONG:
	       //set_brightness_all_on();
		if(disp_scenario == DISP_RTC_SCEN){

			if(rtc_setting==0){
				rtc_setting = 0x01;
				rtc_set = 3;
	                	rtc_set_cnt=30;
			}
		}
	break;
#endif	
#endif	

    case INFO_VOL_PLUS:
    case INFO_VOL_PLUS | KEY_HOLD :
        music_vol += 2;
    case INFO_VOL_MINUS:
    case INFO_VOL_MINUS | KEY_HOLD :
        //break;
		if (music_vol)
            music_vol--;

//        if (music_vol > MAX_MAIN_VOL)
//        {
//            music_vol = MAX_MAIN_VOL;
//        }
        //printf(" vol %d\n",(u16)main_vol);
        //write_info(MEM_VOL, music_vol);
        /**/
        if ((work_mode == SYS_MP3DECODE)&&(play_status == MUSIC_PAUSE))		  				//暂停播放时不调整主音量
		{
			if (DAC_DECODE == dac_channel)
			{
		    	if (music_vol > max_digital_vol)
		        	music_vol = max_digital_vol;
			}
			else
			{
				if (music_vol > max_analog_vol)
		        	music_vol = max_analog_vol;
			}
			Disp_Con(DISP_VOL);
			write_info(MEM_VOL, music_vol);
			main_vol_set(0, CHANGE_VOL_NO_MEM);
			break;
        }
		dac_mute_control(0, 1);
		write_info(MEM_VOL,main_vol_set(0, SET_USE_CURRENT_VOL));
		Disp_Con(DISP_VOL);
        
        break;
		
#ifdef USE_POWER_KEY
    case INFO_POWER | KEY_LONG:	
	    	Disp_Con(DISP_POWER_OFF);
		sys_power_down();
		break;
#if 0		
    case INFO_POWER | KEY_SHORT_UP :	
		
		if(work_mode == SYS_MCU_CD){
			work_mode = SYS_FMREV;
			cur_sw_fm_band=0;
			put_msg_fifo(INFO_NEXT_FM_MODE);	
		}
		else if(work_mode == SYS_FMREV){

			if(cur_sw_fm_band==0){
				cur_sw_fm_band=1;
				put_msg_fifo(INFO_NEXT_FM_MODE);	
			}
			else{
				work_mode = SYS_MCU_CD;
				return 0;
			}

		}
		break;
#endif		
#else		
    case INFO_POWER | KEY_SHORT_UP :	
		if(sys_pwr_flag==0){

			sys_pwr_flag =1;
			work_mode =  SYS_IDLE;
			//sys_mute_flag =1;
        		dac_mute_control(1,1);					//调节音量时，自动UNMUTE
		}
        	put_msg_lifo(INFO_NEXT_SYS_MODE);		
        break;
#endif

#ifdef KEY_100_ENABLE		
   case INFO_100 |KEY_SHORT_UP :

	if(work_mode < SYS_AUX){

       	cfilenum += 100;
		key_100_flag =0xFF;
       	Disp_Con(DISP_DWORD_NUMBER);
   	}
	break;
#endif		
    case INFO_0 | KEY_SHORT_UP :
    case INFO_1 | KEY_SHORT_UP :
    case INFO_2 | KEY_SHORT_UP :
    case INFO_3 | KEY_SHORT_UP :
    case INFO_4 | KEY_SHORT_UP :
    case INFO_5 | KEY_SHORT_UP :
    case INFO_6 | KEY_SHORT_UP :
    case INFO_7 | KEY_SHORT_UP :
    case INFO_8 | KEY_SHORT_UP :
    case INFO_9 | KEY_SHORT_UP :
        if ((work_mode == SYS_MP3DECODE) || (work_mode == SYS_FMREV))
        {
            key &= 0x1f;
	     return_cnt =0;
		 
	     if((cfilenum==0)&&(key==0))break;
			
#if 1//defined(KEY_100_ENABLE)||defined(KEY_10_ENABLE)
            if(cfilenum==0)
	     {
		 key_100_flag = 0x00;
	     }
	     if(key_100_flag ==0xFF){

			cfilenum += key;
			key_100_flag = 0xFE;
	     }
	     else if(key_100_flag ==0xFE){
		 	
			cfilenum += (cfilenum%10)*10-(cfilenum%10)+key;
			key_100_flag = 0x00;
	     }
	     else 
#endif		 	
	     {
	     		if((cfilenum)>6553){
				cfilenum = 0x0000;			
			}
				
			cfilenum = cfilenum *10 + key;
			key_100_flag = 0x00;			
#if 1
	            if (cfilenum > 9999)
	            {
		              cfilenum = 0;			
				cfilenum = cfilenum *10 + key;
		     }
#endif
			
	     }
            Disp_Con(DISP_DWORD_NUMBER);
        }
        break;
		
	case INFO_MUTE |KEY_SHORT_UP:
		sys_mute_flag=~sys_mute_flag;
        	dac_mute_control(sys_mute_flag,1);					//调节音量时，自动UNMUTE
	break;

#if VOICE_TIME_ENABLE    
    case INFO_EQ_UP | KEY_LONG : 
         last_work_mode = work_mode;            
         work_mode =  SYS_RTC;
         report_nowtime =1;
         return 0;
#endif  

    	case INFO_EQ_DOWN| KEY_SHORT_UP :

		if(work_mode==SYS_MP3DECODE){

		        if (device_active == BIT(USB_DISK)){

				if((get_device_online_status()&0x01)>0){

				        given_device = BIT(SDMMC);
				        //given_file_number = 1;
				        put_msg_lifo(SEL_GIVEN_DEVICE_GIVEN_FILE);
				}
			}
			else if (device_active == BIT(SDMMC)){

				if((get_device_online_status()&0x02)>0){

				        given_device = BIT(USB_DISK);
				        //given_file_number = 1;
				        put_msg_lifo(SEL_GIVEN_DEVICE_GIVEN_FILE);
				}				
			}
		}
		//else if(work_mode==SYS_FMREV){
			
			//put_msg_fifo(INFO_NEXT_FM_MODE);	

		//}

	break;	
    }
    return 1;
}
