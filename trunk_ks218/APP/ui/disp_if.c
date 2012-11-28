#include "Custom_config.h"
#include "config.h"
#include "disp_if.h"
#include "lcd.h"
#include "resource.h"
#include "decode.h"
#include "mp3mode.h"
#include "led.h"
#include "lcdsegment.h"
#include "my_printf.h"
#include "mcu_master.h"

extern xd_u8 LCDBuff[LCDPAGE][LCDCOLUMN];
extern xd_u8 LED_BUFF[5];
extern xd_u16 lcd_buff[5];
extern _xdata coordinate LCDsite;
extern u8 _idata music_vol;
extern FSAPIMSG _pdata fs_msg;
extern xd_u16 given_file_number;
extern xd_u8 play_mode;
extern xd_u8 eq_mode;
extern xd_u16 cfilenum;
extern u8 device_active;
//extern _code u8 _code * chars_table_l;
extern xd_u8 all_channl;
extern xd_u16 frequency;
extern xd_u8 fre_channl,play_status;
extern _xdata u8 fre_preset[];
extern MAD_DECODE_INFO _pdata mad_decode_dsc;

extern xd_u8 sel_work_mode;
extern bool gpio_sel_func;

xd_u8 return_cnt;
xd_u8 curr_menu;
extern u16 filenameCnt;
xd_u16 filenameNum;
extern xd_u8 cur_sw_fm_band,rtc_setting,rtc_set,alm_set;
extern  _xdata u8 filename_buff[100];
extern bool alm_sw;
extern xd_u8 station_save_pos,station_sel_pos;
extern _xdata SYS_WORK_MODE  work_mode;

extern bool usb_sd_dev_toc;
extern xd_u16 play_dir_tatol;

xd_u8 disp_icon_timer=0;
#ifdef SW_VER_DISP
xd_u8 sw_ver_disp=0;
#endif
#ifdef USE_PROG_PLAY_MODE
extern  xd_u8 prog_total_num,prog_cur_num;
extern bool prog_icon_bit,play_prog_mode,cd_exchange_disp;
#endif

#ifdef USE_CD_MCU_MASTER_FUNC	
extern bool toc_flag;
extern TOC_TIME cur_time;
extern xd_u8 cd_play_status;
#endif

#ifdef USE_USB_PROG_PLAY_MODE
extern bool usb_play_prog_mode,usb_prog_icon_bit,exchange_disp;
extern xd_u16 usb_prog_total_num,usb_prog_cur_num;
#endif

bool  full_timer_spark=0;

extern bool mode_switch_protect_bit;

void Disp_Playmode_icon();

void Disp_dir_num(void);

#ifdef RADIO_ST_INDICATOR
extern bool radio_st_ind;
#endif

#ifdef USE_USB_SD_DECODE_FUNC	       
extern bool folder_mode_select;
#endif

#if defined(USE_BAT_MANAGEMENT)
extern void Bat_icon_chk(void);
#endif

u8 _code playmodestr[4][7] =
{
    {"ALL"},
    {"ONE"},
    {"FoLdER"},
    {"RANdOM"}
};

void disp_clr_buf()
{
	init_disp_buf();
}
void disp_init_if(void)
{
	init_disp();
	disp_clr_buf();
	//disp_icon_timer=0;
}
xd_u8 backlight_timer=0;
void set_brightness_fade_out(void)
{
    if (backlight_timer >0)
    {
    
        backlight_timer--;
	 return;
    }
    LCD_BACKLIGHT_OFF();
}

void set_brightness_all_on(void)
{
    LCD_BACKLIGHT_ON();
    backlight_timer = 60;
}

void disp_active(void)
{
    if (device_active == BIT(USB_DISK))
    {
        disp_icon(ICON_USB);
    }
    else if (device_active == BIT(SDMMC))
    {
        disp_icon(ICON_SD);
    }
     disp_icon(ICON_MP3);
}
void Disp_Null()
{

}
void Disp_Num(void)
{
#ifdef USE_CD_MCU_MASTER_FUNC			
	if(work_mode == SYS_MCU_CD){

		if(cfilenum>0)
		printf_num(cfilenum,2,2);
	}
	else
#endif
	printf_num(cfilenum,0,4);

}
void Disp_prog_num(void)
{
#ifdef USE_PROG_PLAY_MODE
#if defined(LCD_DISP_THREE_DIGIT)
	if(work_mode == SYS_MCU_CD){


		if(cd_exchange_disp){

			printf_char('P',1);
			printf_num(prog_total_num,2,2);

		}
		else{

			full_timer_spark=0;

			if(prog_total_num==20){
				full_timer_spark=1;				
		    		printf_str("FUL",1);
			}
			else{
				printf_num(prog_cur_num,2,2);
			}
		}
	}
#else	
	if(work_mode == SYS_MCU_CD){
		
		printf_num(prog_total_num,0,2);

		full_timer_spark=0;

		if(prog_total_num==20){

			full_timer_spark=1;				
	    		printf_str("FUL",1);
		}
		else{
			printf_num(prog_cur_num,2,2);
		}
	}
#endif	
#endif

#ifdef USE_USB_PROG_PLAY_MODE
#if defined(LCD_DISP_THREE_DIGIT)

	if(work_mode <= SYS_MP3DECODE_SD){

		if(exchange_disp){

			printf_char('P',1);
			printf_num(usb_prog_total_num,2,2);

		}
		else{			

			full_timer_spark=0;

			if(usb_prog_total_num==USB_PROG_MAX){

				full_timer_spark=1;				
		    		printf_str("FUL",1);
			}
			else{
				printf_num(usb_prog_cur_num,1,3);
			}
		}
	}
#else
	if(work_mode <= SYS_MP3DECODE_SD){
		
		printf_num(usb_prog_total_num,0,2);

		full_timer_spark=0;

		if(usb_prog_total_num==20){

			full_timer_spark=1;
	    		printf_str("FUL",1);
		}
		else{			
			printf_num(usb_prog_cur_num,2,2);
		}
	}
#endif
#endif
	
	disp_icon(ICON_PROG);	
}
void Disp_Filenum(void)
{

#if defined(LCD_DISP_THREE_DIGIT)
	if(work_mode == SYS_MCU_CD){
		
    		printf_num(given_file_number,2,2);

		if((toc_flag)&&(cd_play_status == MUSIC_PLAY)){

			disp_icon(ICON_PLAY);					
		}			
	}
	else{	
    		printf_num(given_file_number,1,3);
	}
#else

	if(given_file_number>999)
    	printf_num(given_file_number,0,4);
	else if(given_file_number>99)
    	printf_num(given_file_number,1,3);
	else 
    	printf_num(given_file_number,2,2);

#endif	
    	disp_active();
    Disp_Playmode_icon();

}
void Disp_Nofile(void)
{
#ifdef MCU_CD_728_LCD_MODULE
    	printf_str("No",1);
#elif defined(LCD_DISP_THREE_DIGIT)
    	printf_str("no",1);
#else
    	printf_str("NO",0);
#endif
}

void Disp_waiting(void)
{
    	printf_str(".....",0);
}
void Disp_Nodevice(void)
{
    	if(work_mode == SYS_MP3DECODE_USB){
        	disp_icon(ICON_USB);
	}	
    	else if(work_mode == SYS_MP3DECODE_SD){
        	//disp_icon(ICON_SD);
	}
    	else if(work_mode == SYS_MCU_CD){
        	disp_icon(ICON_CD);
	}
#ifdef MCU_CD_728_LCD_MODULE
    	printf_str("Nod",1);
#elif defined(LCD_DISP_THREE_DIGIT)
    	printf_str("no",2);
#else
    	printf_str("NO d",0);
#endif

	disp_clr_icon(ICON_PLAY);					

}
void Disp_Vol(void)
{
	printf_char('V',1);
	printf_num(music_vol,2,2);
}
void Diap_Playmode(void)
{
    printf_str(&playmodestr[play_mode][0],0);
}
void Disp_Playmode_icon()
{
	disp_clr_icon(ICON_REP_ALL);
	disp_clr_icon(ICON_REP_1);
	disp_clr_icon(ICON_REP_RDM);
	disp_clr_icon(ICON_REP_FOD);

	if(work_mode == SYS_MCU_CD){

		if(prog_icon_bit||play_prog_mode){
				return ;	
		}
	}
	if(work_mode <SYS_MCU_CD){

		if(!usb_sd_dev_toc)return;
	}
	
	if(play_mode==REPEAT_ALL){
#ifdef LCD_DISP_THREE_DIGIT
	    disp_icon(ICON_REP_1);
#endif	
	    disp_icon(ICON_REP_ALL);
	}
	else if(play_mode == REPEAT_ONE){
	    disp_icon(ICON_REP_1);
	}
	else if(play_mode == REPEAT_RANDOM){
	    disp_icon(ICON_REP_RDM);
	}	
	else if(play_mode == REPEAT_FOLDER){
	    disp_icon(ICON_REP_1);		
	    disp_icon(ICON_REP_FOD);
	}
}
#ifdef PLAY_DISP_FILE_NUM_DIR_ONLY
static xd_u8 mp3_disp_timer=0;
void reset_disp_timer()
{
	mp3_disp_timer=0;
}
void disp_file_time(void)
{
#ifdef USE_CD_MCU_MASTER_FUNC

    if(work_mode ==SYS_MCU_CD){

    		printf_num(given_file_number,2,2);
    }
    else
#endif
    {
	mp3_disp_timer++;
	if(mp3_disp_timer<5){

    		printf_num(given_file_number,1,3);
	}
	else if(mp3_disp_timer<10){

		Disp_dir_num();
	}
	else{

		if(mp3_disp_timer<=10){
    			printf_num(given_file_number,1,3);
			
			mp3_disp_timer=0;
		}
	}
    }
	
    disp_icon(ICON_PLAY);
    disp_icon(ICON_COL);
    disp_active();
    Disp_Playmode_icon();
}

#else

void disp_file_time(void)
{
    u16 sec;
    u16 min;
    u32 file_play_time;

#ifdef USE_CD_MCU_MASTER_FUNC

    if(work_mode ==SYS_MCU_CD){
		
	    printf_num(cur_time.SEC,2,2);
	    printf_num(cur_time.MIN,0,2);

	    if(cd_play_status==MUSIC_PLAY){	

	    	disp_clr_icon(ICON_PAUSE);
	    	disp_icon(ICON_PLAY);
	    }
	    else  if(cd_play_status==MUSIC_PAUSE){	

	    	disp_clr_icon(ICON_PLAY);

	    	disp_icon(ICON_PAUSE);
	    }
    }
    else
#endif
    {
	    file_play_time = get_music_play_time();
	    sec = file_play_time % 60;
	    min = (file_play_time/60) % 60;
		
	    printf_num(sec,2,2);
	    printf_num(min,0,2);
    }
#if defined(MCU_CD_728_LCD_MODULE)
    disp_clr_flash_icon(ICON_PLAY);
#endif

    disp_icon(ICON_PLAY);
    //disp_icon(ICON_COL);
    disp_active();
    Disp_Playmode_icon();
	
}
#endif

void Disp_Pause(void)
{
    disp_active();
#ifdef DISP_PAUS_STR
    printf_str("PAUS",0);
#elif defined(DISP_PAU_STR)
    printf_str("PAU",1);
#else
    disp_file_time();
#endif
    disp_clr_icon(ICON_PLAY);	
    disp_icon(ICON_PAUSE);
}
#ifdef PLAY_DISP_FILE_NUM_DIR_ONLY
xd_u8 disp_timer=0;
void Disp_Stop(void)
{

#ifdef USE_CD_MCU_MASTER_FUNC
	if(work_mode == SYS_MCU_CD){
		
#if defined(MCU_CD_728_LCD_MODULE)

#elif defined(LCD_DISP_THREE_DIGIT)

#else			
		printf_char('C',0);
		printf_char('d',1);
#endif
		printf_num(cfilenum,2,2);
	}	
	else
#endif
	{
	
		disp_timer++;
		if(disp_timer<5){

			printf_num(fs_msg.fileTotal,1,3);
		}
		else if(disp_timer<=10){

			//Disp_dir_num();
			printf_char('F',1);
			printf_num((play_dir_tatol+1),2,2);			
		}
		else{

			if(disp_timer>10){
				printf_num(fs_msg.fileTotal,1,3);
				disp_timer=0;
			}
		}	
    		//printf_str("STOP",0);
	}
    disp_active();

}

#else
void Disp_Stop(void)
{
#ifdef USE_CD_MCU_MASTER_FUNC
	if(work_mode == SYS_MCU_CD){
#if defined(MCU_CD_728_LCD_MODULE)

#else			
		printf_char('C',0);
		printf_char('d',1);
#endif
		printf_num(cfilenum,2,2);
	}	
	else
#endif
	{
		if(fs_msg.fileTotal>999)
			printf_num(fs_msg.fileTotal,0,4);
		else if(fs_msg.fileTotal>99)
			printf_num(fs_msg.fileTotal,1,3);
		else
			printf_num(fs_msg.fileTotal,2,2);
    		//printf_str("STOP",0);
	}
    disp_active();

}
#endif
void Disp_Play(void)
{
    disp_active();
    disp_file_time();
    disp_icon(ICON_PLAY);	
    disp_clr_icon(ICON_PAUSE);	
}
void Disp_Hello(void)
{
#ifdef WELCOME_DISP_BAR_BAR
    //printf_str("----",0);
#elif defined(WELCOME_DISP_ON_STR)
    printf_str(" ON",0);
#else
    printf_str(" HI",0);
#endif
    set_brightness_all_on();	
}
void Disp_Eq(void)
{
    printf_char('E',1);
    printf_char('q',2);
    printf_num(eq_mode,3,1);
}
void Diap_usbslave(void)
{
    printf_str("U-d",1);
}
void Disp_scan_disk(void)
{
#if defined(LCD_DISP_THREE_DIGIT)
    	printf_str("---",1);
#else
    	printf_str(" Lod",0);
#endif
}
void Disp_scan_toc(void)
{

    	if(work_mode == SYS_MP3DECODE_USB){
        	disp_icon(ICON_USB);
	}	
    	else if(work_mode == SYS_MP3DECODE_SD){
        	disp_icon(ICON_SD);
	}
    	else if(work_mode == SYS_MP3DECODE_SD){
        	disp_icon(ICON_CD);
	}
		
#ifdef LCD_DISP_THREE_DIGIT
    	printf_str("---",1);
#else
    	printf_str("----",0);
#endif
}
void disp_error(void)
{
    printf_str(" ERR",0);
}
void disp_open(void)
{

#ifdef SW_VER_DISP
	if(sw_ver_disp>0){
    		printf_char('V',1);
    		printf_num(SW_VER_DISP,2,2);
		return;
	}
#endif
#ifdef LCD_MODULE_WITHOUT_F_DIGIT
	printf_str("OPN",1);
#elif defined(LCD_DISP_THREE_DIGIT)
	printf_str("OP ",1);
#else
	printf_str("OPEN",0);
#endif
}
void Disp_dir_num(void)
{
	//dispNum((u8)((fs_msg.dirTotal/1000)%10),3);
	disp_active();
	printf_char('F',1);
	printf_num((fs_msg.dirTotal+1),2,2);
}
void Disp_Power_up(void)
{

}
void Disp_Power_off(void)
{
	disp_clr_buf();
}
#ifdef USE_TIMER_POWER_OFF_FUNC
extern xd_u8 timer_disp;
void Disp_Timer_OFF(void)
{

   if(timer_disp == 0){
	    printf_str(" OFF",0);
   }
   else{	
   	
    	printf_num(timer_disp,2,2);
   }
}
#endif 

void Disp_freq(void )
{
	u16 freq=0;
	
	if(cur_sw_fm_band==0){
		freq =frequency/10;
	 	disp_icon(ICON_FM_MHZ);		
	}
	else if(cur_sw_fm_band==1){
		freq =frequency;
	 	disp_icon(ICON_AM_KHZ);
	}
	else{
		freq =frequency/10;
	 	disp_icon(ICON_SW);		

	}
	
    if(freq > 999)
    {
        printf_num(freq,0,4);
    }	
    else if(freq > 99)
    {
        printf_num(freq,1,3);
    }
    else{

        printf_num(freq,2,2);
    }

}
void Disp_Aux(void )
{
    printf_str("AUX",1);
}
void Disp_cur_band(void)
{
   if(cur_sw_fm_band ==0)
   {
    	printf_str("FN7",1);
   }
   else  if(cur_sw_fm_band ==1){

    	printf_str("AN7",1);
   }
   else{
    	printf_str("bd",1);
    	printf_num((cur_sw_fm_band-1),3,1);
   }
}
void Disp_station_ch(void)
{
    	printf_str("P",1);
    	printf_num((station_save_pos+1),2,2);
	disp_icon(ICON_PROG);		
}
void Disp_sel_station_ch(void)
{
    	printf_str("P",1);
    	printf_num((station_sel_pos+1),2,2);
}
void Disp_USB_str()
{
    	printf_str("USB",1);
	disp_icon(ICON_USB);	
}
void Disp_SD_str()
{
	disp_clr_icon(ICON_USB);	
    	printf_str("Sd",1);
}
void Disp_CD_str()
{
	disp_clr_icon(ICON_USB);	
    	printf_str("Cd",1);
}
#if RTC_ENABLE
xd_u8  clock_points=0;
extern RTC_TIME curr_date;
void Disp_curr_time(void)
{
#ifdef DISP_ACTIVE_DEVICE_WHEN_TIME_DISPLAY
    if(play_status == MUSIC_PLAY){
    	disp_active();
	Disp_Playmode_icon();
    }
#endif	
    printf_num(curr_date.minutes,2,2);
    printf_num(curr_date.hour,0,2);

    if(clock_points==1){

		clock_points =0;
		printf_char(':',0xFF);		
    }
    else{
		
		clock_points =1;
		
		if(rtc_setting>0){

			if(rtc_set==3){
    				clr_lcd_digit(0);
    				clr_lcd_digit(1);
			}
			else if(rtc_set==4){
    				clr_lcd_digit(2);
    				clr_lcd_digit(3);			
			}
			printf_char(':',0xFF);		
    		}
   }
}
extern ALM_TIME curr_alm;
void Disp_alarm_time(void)
{

    if(clock_points==1){
		
	    clock_points =0;
	    if(alm_set==1){

	    }
	    else{
		    	printf_num(curr_alm.minutes,2,2);
			printf_num(curr_alm.hour,0,2);			
			printf_char(':',0xFF);	
	    }
    }
    else{

	    clock_points =1;

	    if(alm_set==1){

			if(alm_sw){
			    	printf_str("ON",1);
			}
			else{
			    	printf_str("OFF",1);
			}
	    }
	    else{
		    	printf_num(curr_alm.minutes,2,2);
			printf_num(curr_alm.hour,0,2);			
			printf_char(':',0xFF);		

			if(alm_set==2){
			  	clr_lcd_digit(0);
			    	clr_lcd_digit(1);
			}
			else if(alm_set==3){
			    	clr_lcd_digit(2);
			  	clr_lcd_digit(3);			
			}			
	   }
   }
}

void Disp_alarm_up(void)
{
    if(clock_points==1){
		
	    	clock_points =0;
    		printf_num(curr_date.minutes,2,2);
    		printf_num(curr_date.hour,0,2);			
		printf_char(':',0xFF);	
    }
    else{
	    	clock_points =1;
   }
}
#endif

void custom_buf_update(void)
{
	static bool flash=0;

	
	if(work_mode ==SYS_MP3DECODE_USB){

		
		if((sel_work_mode==SYS_MCU_CD)&&(gpio_sel_func)&&(!mode_switch_protect_bit)){

			disp_clr_icon(ICON_USB);	
		  	disp_clr_buf();				
			printf_str("Cd",1);
			disp_icon_timer=0;
			flash=1;				
			
			return;
		}

	}

	if(work_mode ==SYS_MCU_CD){

		
		if((sel_work_mode==SYS_MP3DECODE_USB)&&(gpio_sel_func)&&(!mode_switch_protect_bit)){

		  	disp_clr_buf();				
		    	printf_str("USB",1);
			disp_icon(ICON_USB);	
			disp_icon_timer=0;
			flash=1;				
			
			return;
		}

	}	
	if(work_mode <=SYS_MP3DECODE_SD){
			
		if((curr_menu == DISP_SCAN_DISK)||(curr_menu == DISP_SCAN_TOC)){


			if((sel_work_mode==SYS_MCU_CD)&&(gpio_sel_func)&&(!mode_switch_protect_bit)){

				disp_clr_icon(ICON_USB);	
		    		disp_clr_buf();				
				printf_str("Cd",1);
				disp_icon_timer=0;
				flash=1;				
				return;
			}
				
			disp_icon_timer++;
			if (disp_icon_timer == 250)
			{
			        disp_icon_timer = 0;
			        flash = ~flash;
			}
			
			if(flash){

    				disp_clr_buf();				
    				printf_str("---",1);
			}
			else{
    				disp_clr_buf();
			}
			
			if(work_mode ==SYS_MP3DECODE_USB){
				 disp_icon(ICON_USB);	
			}
			return;
		
		}

	}	
#ifdef USE_RTC_ALARM_FUNCTION
	if(alm_sw){
	 	disp_icon(ICON_BELL);		
	}
	else{
	 	disp_clr_icon(ICON_BELL);		
	}
#endif
		
#ifdef USE_CD_MCU_MASTER_FUNC
	if(work_mode == SYS_MCU_CD){
		 disp_icon(ICON_CD);	
		 disp_clr_icon(ICON_USB);		
		 disp_clr_icon(ICON_SD);		
	}
#endif

	if(work_mode ==SYS_MP3DECODE_USB){


		//if(device_active==0x02)
		 disp_icon(ICON_USB);	

		if((play_status == MUSIC_PLAY)||(play_status==MUSIC_FF_FR)){
	    		disp_icon(ICON_PLAY);		
		}
		else{
	    		disp_clr_icon(ICON_PLAY);		
		}		 
	 }
	else if(work_mode ==SYS_MP3DECODE_SD){

		if((play_status == MUSIC_PLAY)||(play_status==MUSIC_FF_FR)){
	    		disp_icon(ICON_PLAY);		
		}
		else{
	    		disp_clr_icon(ICON_PLAY);		
		}
		
		 disp_clr_icon(ICON_USB);		
		 disp_clr_icon(ICON_CD);		

	}

#ifdef USE_PROG_PLAY_MODE
	if(work_mode ==SYS_MCU_CD){
		
		if(prog_icon_bit||play_prog_mode){
#ifdef PROG_MODE_FLASH_PROG_ICON
			if(cd_play_status == MUSIC_STOP){
				
				if(play_prog_mode){
				    		disp_flash_icon(ICON_PROG);

						if(full_timer_spark){


							disp_icon_timer++;
							if (disp_icon_timer == 250)
							{
							        disp_icon_timer = 0;
							        flash = ~flash;
							}							
							if(flash){
    								disp_clr_buf();								
								printf_str("FUL",1);
							}
							else{
    								disp_clr_buf();								
							}
						}							
				}
				else{
				    		disp_clr_flash_icon(ICON_PROG);
				}


				if(prog_icon_bit){
					disp_icon(ICON_PROG);
				}
				else{

					disp_clr_icon(ICON_PROG);
				}			
			}
			else
#endif
			{
				disp_icon(ICON_PROG);
			}
		}
		else{
	    		disp_clr_flash_icon(ICON_PROG);			
			disp_clr_icon(ICON_PROG);		
		}
	}
#endif

#ifdef USE_USB_PROG_PLAY_MODE
	if(work_mode <SYS_MCU_CD){
		
		if(usb_prog_icon_bit||usb_play_prog_mode){
#ifdef PROG_MODE_FLASH_PROG_ICON

			if(play_status == MUSIC_STOP){

				if(usb_play_prog_mode){
				    		disp_flash_icon(ICON_PROG);
							
						if(full_timer_spark){

							disp_icon_timer++;
							if (disp_icon_timer == 250)
							{
							        disp_icon_timer = 0;
							        flash = ~flash;
							}							
							if(flash){
    								disp_clr_buf();								
								printf_str("FUL",1);
							}
							else{
    								disp_clr_buf();								
							}
						}							
				}
				else{
				    		disp_clr_flash_icon(ICON_PROG);
				}

				if(usb_prog_icon_bit){
					disp_icon(ICON_PROG);
				}
				else{

					disp_clr_icon(ICON_PROG);
				}

			}
			else
#endif
			{
				disp_icon(ICON_PROG);
			}
		}
		else{

	    		disp_clr_flash_icon(ICON_PROG);						
			disp_clr_icon(ICON_PROG);		
		}
	}
#endif

#if 0//def USE_USB_SD_DECODE_FUNC	       
	if(work_mode <SYS_MCU_CD){

		if(folder_mode_select){
		    	disp_icon(ICON_REP_FOD);
		}
		else{
		    	disp_clr_icon(ICON_REP_FOD);
		}
	}
#endif


#ifdef FLASH_PLAY_ICON_WHEN_PAUSE
	if(((toc_flag)&&(cd_play_status == MUSIC_PAUSE)&&(work_mode == SYS_MCU_CD))||((device_active>0)&&(play_status == MUSIC_PAUSE)&&(work_mode <= SYS_MP3DECODE_SD))){
	    		disp_flash_icon(ICON_PLAY);
	}
	else{
	    		disp_clr_flash_icon(ICON_PLAY);
	}
#endif
	
#if defined(USE_BAT_MANAGEMENT)
	Bat_icon_chk();
#endif
}
/*----------------------------------------------------------------------------*/
/**@brief
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void Disp_Con(u8 LCDinterf)
{
    return_cnt = 0;
    curr_menu = LCDinterf;
	
#ifdef UART_ENABLE
	    	printf("------->-Disp_Con   %d    \r\n",(u16)LCDinterf);
#endif

    full_timer_spark=0;

    disp_clr_buf();
    switch (LCDinterf)
    {
    case DISP_NULL:
        Disp_Null();
        break;
    case DISP_HELLO:
        Disp_Hello();
        break;		
    case DISP_SCAN_DISK:
        Disp_scan_disk();
        break;	
    case DISP_SCAN_TOC:
        Disp_scan_toc();
        break;			
    case DISP_DWORD_NUMBER:
        Disp_Num();
        break;		
    case DISP_PROG_FILENUM:
        Disp_prog_num();
        break;			
    case DISP_FILENUM:
        Disp_Filenum();
        break;
    case DISP_NOFILE:
        Disp_Nofile();
        break;
    case DISP_NODEVICE:
        Disp_Nodevice();
        break;
    case DISP_PLAY:
        Disp_Play();
        break;
    case DISP_PAUSE:
        Disp_Pause();
        break;
    case DISP_STOP:
        Disp_Stop();
        break;	
    case DISP_VOL:
        Disp_Vol();
        break;
    case DISP_EQ:
        Disp_Eq();
        break;
    case DISP_DIR:					   
	 Disp_dir_num();			
	break;		
    case DISP_POWER_UP:
        Disp_Power_up();
        break;
    case DISP_POWER_OFF:
        Disp_Power_off();
        break;		
#ifdef USE_TIMER_POWER_OFF_FUNC
    	case DISP_PWR_TIMER:
	Disp_Timer_OFF();
	break;	
#endif		
    case DISP_PLAYMODE:
        Diap_Playmode();
        break;
    case DISP_WAIT:
        Disp_waiting();
        break;
    case DISP_USBDEV:
        Diap_usbslave();
        break;
    case DISP_USB:
        Disp_USB_str();
        break;
    case DISP_SD:
        Disp_SD_str();
        break;		
    case DISP_CD:
        Disp_CD_str();
        break;			
    case DISP_FREQ:
        Disp_freq();
        break;
    case DISP_SAVE_POS:
        Disp_station_ch();
        break;
    case DISP_SEL_POS:
        Disp_sel_station_ch();
        break;		
    case DISP_BAND_NUM:
        Disp_cur_band();
        break;
    case DISP_AUX:
        Disp_Aux();
        break;	
#if RTC_ENABLE		
    case DISP_RTC:
        Disp_curr_time();
        break;	
    case DISP_ALARM:
        Disp_alarm_time();
        break;	
    case DISP_ALM_UP:
        Disp_alarm_up();
        break;	
#endif		
    case DISP_ERROR:
        disp_error();
        break;		
    case DISP_OPEN:
        disp_open();
        break;			
    }
}
