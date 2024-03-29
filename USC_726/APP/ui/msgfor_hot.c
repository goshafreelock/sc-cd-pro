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
#include "KT_AMFMdrv.h"

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
xd_u8 my_music_vol=0;
xd_u8 radio_force_preset=0;
extern xd_u8 curr_menu;

extern bool sys_pwr_flag;
extern bool cd_open_detect,fm_reset_enable;
extern xd_u8 fm_reset_cnt;

extern xd_u8 cur_sw_fm_band;

extern void rtc_disp_hdlr(void);
extern void curr_date_minus();
extern void curr_date_plus();
extern void set_date_sec();
extern void alm_time_plus();
extern void alm_time_minus();
extern void set_alm_sec(void);
extern void radio_preset_init();

void aux_channel_crosstalk_improve(u8 ch_num)
{
	//aux_ch_reg=ch_num;
	//printf("-------->%aux_ch_reg  %d \r\n",(u16)aux_ch_reg);

#ifdef UART_ENABLE
		if(ch_num == DAC_AMUX0){
			sys_printf(" AUX0_SELECTED_P24_P25");
			return ;
		}
		else{
			sys_printf(" AUX1_SELECTED_P26_P27");
		}
#endif


		if(ch_num == DAC_AMUX0){
			DACCON0 &=~0x40;
		     	P2DIR |=( (BIT(4))|(BIT(5)));
		    	P2PU &=~( (BIT(4))|(BIT(5)));
		    	//P2 &=~( (BIT(6))|(BIT(7)));
		}
		else if(ch_num == DAC_AMUX1){

			DACCON0 &=~0x80;
		     	P2DIR |=( (BIT(6))|(BIT(7)));
		    	P2PU &=~( (BIT(6))|(BIT(7)));
		    	//P2 &=~( (BIT(6))|(BIT(7)));
		}
    		dac_out_select(ch_num);	//4

		if(ch_num == DAC_AMUX0){	//4AUX0_P24_P25

			DACCON0|=0x80;
		     	P2DIR &=~( (BIT(6))|(BIT(7)));
		    	P2PU |=( (BIT(6))|(BIT(7)));
		    	P2 &=~( (BIT(6))|(BIT(7)));
		}
		else if(ch_num == DAC_AMUX1){//4AUX0_P26_P27
		
			DACCON0|=0x40;
		     	P2DIR &=~( (BIT(4))|(BIT(5)));
		    	P2PU |=( (BIT(4))|(BIT(5)));
		    	P2 &=~( (BIT(4))|(BIT(5)));
		}

}
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
	if(work_mode == SYS_AUX){

		if(vol==1){
			vol= 3;
		}
		else if(vol==2){
			vol= 5;
		}
		else if(vol==3){
			vol= 6;
		}
		else if((vol==4)||(vol==5)){
			vol= 8;
		}
		else if((vol==6)||(vol==7)){
			vol= 9;
		}		
		else if((vol==8)||(vol==9)){
			vol= 10;
		}
		
		music_vol=vol;
		main_vol_set(0, SET_USE_CURRENT_VOL);		
	}
	else
	{
		music_vol=vol;
		main_vol_set(0, SET_USE_CURRENT_VOL);
	}
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
#if defined(USE_MANUAL_IDLE_FUNC)||defined(IPONE_INDEPENDENT_MODE)
#define MAX_FUNC_LIST 		IDLE+1
#else
#define MAX_FUNC_LIST 		IDLE
#endif
static xd_u16 Sys_Func_List=0;

void Add_Func_To_List(DEV_ON_LIST Dev)
{
	Sys_Func_List |=BIT(Dev);
	//printf(" --->Add_Func_To_List %x \r\n",(u16)Sys_Func_List);

}
void Remov_Func_From_List(DEV_ON_LIST Dev)
{
	Sys_Func_List &=~(BIT(Dev));

	//printf(" --->Remov_Func_From_List %x \r\n",(u16)Sys_Func_List);
	
}
void Set_Curr_Func(SYS_WORK_MODE curr_mod)
{
	work_mode = curr_mod;	
	Sys_Func_List =((curr_mod<<8)|(Sys_Func_List&0x00FF));	
	//printf(" --->Set_Curr_Func %x \r\n",(u16)Sys_Func_List);
	
}
void Init_Func_List()
{
	//sys_printf("Init_Func_List");
	Sys_Func_List=0;
#ifdef USE_USB_SD_DECODE_FUNC	       
	//if((get_device_online_status()&0x02))
#ifndef REMOVE_USE_MODE	
		Add_Func_To_List(USB_DEV);
#endif
#ifndef NO_SD_DECODE_FUNC
	if((get_device_online_status()&0x01))
		Add_Func_To_List(SD_DEV);
#endif	
#endif
#ifdef USE_CD_MCU_MASTER_FUNC
		Add_Func_To_List(CD_DEV);
#endif		

#ifdef USE_RADIO_FUNC
       //if(fm_chip_online_chk()){	

		Add_Func_To_List(FM_DEV);
#ifdef AM_RADIO_FUNC
		Add_Func_To_List(AM_DEV);
#endif
 	    	//enter_fm_rev();  
		KT_AMFMStandby();
	//}
#endif

#ifndef NOT_USE_LINE_IN_FUNC

#ifdef USE_LINE_IN_DETECT_FUNC
#ifdef AUX_DETECT_FUNC_WITH_MANUAL_SEL
	Add_Func_To_List(AUX_DEV);
#else
	delay_10ms(20);
	if(aux_online)
		Add_Func_To_List(AUX_DEV);
#endif	
#else	
#ifndef DONT_MOUNT_AUX_FUNC
		Add_Func_To_List(AUX_DEV);
#endif
#endif
#endif

#if defined(IPONE_INDEPENDENT_MODE)
		Add_Func_To_List(IPH_DEV);
#endif
#ifdef USE_RTC_FUNCTION
		Add_Func_To_List(RTC_DEV);
#endif

#ifdef USE_MANUAL_IDLE_FUNC
		Add_Func_To_List(IDLE);
#endif

#if defined(IPONE_DOCK_FUNC_ENABLE)
#if defined(IPHONE_DETECT_IN_USE)
	if(iphone_status){
		Set_Curr_Func(SYS_IPH);
	}
#endif
#endif

	//sys_printf("Init_Func_List   end....");

}
static SYS_WORK_MODE Next_Func()
{
	u8 i;
	//printf(" -111-->Sys_Func_List %x \r\n",(u16)Sys_Func_List);
	//printf(" -222-->CURR FUNC %x \r\n",(u16)((Sys_Func_List&0xFF00)));
	
	if((Sys_Func_List&0x00FF)>0)
	{
		for(i=(((Sys_Func_List&0xFF00)>>8)+1);i<MAX_FUNC_LIST;i++){
			//printf(" --%x->Next_Func --->BIT  %x \r\n",(u16)i,(u16)(BIT(i)));
			if((Sys_Func_List&(BIT(i)))>0){
				return i;
			}
		}				
		for(i=0;i<=((Sys_Func_List&0xFF00)>>8);i++)
			if((Sys_Func_List&(BIT(i)))>0){
				return i;
		}
	}
	
	return SYS_IDLE;
}

extern xd_u16 usb_erp_timer,aux_erp_timer,erp_timer;

xd_u8 erp2_test_mode_timer=0;
xd_u8 erp2_test_enable=0;
xd_u8 erp_2_test_mode_window=0;
void enable_erp_2_test_window(void)
{
	erp_2_test_mode_window=20;		// 10 sec window
}
void erp_2_test_mode_enable()
{
#if 1
	erp2_test_enable=2;
#else
	if(erp_2_test_mode_window >0){
		erp_2_test_mode_window=6;
		erp2_test_enable=2;
	}
#endif	
	if(work_mode == SYS_AUX){
		
		if((WKUPPND&BIT(7))>0){
			erp2_test_mode_timer =0;
			erp2_test_enable =0;
		}
	}
}
void erp_2_test_mode_handlr(void)
{
#if 0
	if(erp_2_test_mode_window>0){

		erp_2_test_mode_window--;
	}
	else{
		return ;
	}
#endif	
	
	if(erp2_test_enable>0){

		erp2_test_enable--;
		
		erp2_test_mode_timer++;
		
		if(erp2_test_mode_timer>=20){
			
			erp2_test_mode_timer =0;
			usb_erp_timer=0x1FFF;
			aux_erp_timer=0x1FFF;
			erp_timer=0x1FFF;
			
#ifdef UART_ENABLE
    			printf("------->- erp_2_test_mode_handlr   \r\n");
#endif
			
		}
		else{

			usb_erp_timer=0;
			aux_erp_timer=0;
			erp_timer=0;
		}
	}
	else{
		erp2_test_mode_timer=0;
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
    SYS_WORK_MODE mode_reg=0;
	
#if 0
	if(key!= 0xff)
	    	printf("------->-ap_handle_hotkey   %x \r\n",(u16)key);
#endif

    switch (key)
    {
 
#if 0//def USE_USB_SD_DECODE_FUNC	       
   
    case MSG_USB_DISK_OUT: 
	 Remov_Func_From_List(USB_DEV);
        break;
    case MSG_SDMMC_OUT:
	 Remov_Func_From_List(SD_DEV);
        break;

    case MSG_SDMMC_IN :

#ifdef NO_MEDIA_DEV_HOT_PLUG_SEL_FUNC
        if (work_mode > SYS_MP3DECODE_SD){
		break;
	 }
#endif		

        disp_scenario = DISP_NORMAL;
	 Add_Func_To_List(SD_DEV);
        given_device = BIT(SDMMC);
        put_msg_lifo(SEL_GIVEN_DEVICE_GIVEN_FILE);
        if ((work_mode != SYS_MP3DECODE_SD) && (work_mode != SYS_USB_DEVICE))
        {
	     Set_Curr_Func(SYS_MP3DECODE_SD);
            return 0;
        }

        break;

    case MSG_USB_DISK_IN  :
		
#ifdef NO_MEDIA_DEV_HOT_PLUG_SEL_FUNC
        if (work_mode > SYS_MP3DECODE_SD){
		break;
	 }
#endif		

        disp_scenario = DISP_NORMAL;
	 Add_Func_To_List(USB_DEV);
        given_device = BIT(USB_DISK);
        put_msg_lifo(SEL_GIVEN_DEVICE_GIVEN_FILE);
        if (work_mode != SYS_MP3DECODE_USB)
        {
	     Set_Curr_Func(SYS_MP3DECODE_USB);
            return 0;
        }
        break;
#endif

#ifdef CUSTOMED_KEY_FORCED_INIT_PRESET
	case INFO_PLAY |KEY_HOLD:
		if(cd_open_detect){
			
			fm_reset_enable = 1;
			
			if(fm_reset_cnt>=19){
				fm_reset_enable =0;
				fm_reset_cnt=0;
				radio_preset_init();
			}
		}
		break;
#endif

#ifdef USE_LINE_IN_DETECT_FUNC
    case MSG_AUX_IN :
        if (work_mode != SYS_USB_DEVICE){
            last_work_mode= work_mode;
	     Set_Curr_Func(AUX_DEV);
            return 0;
        }
        break;

    case MSG_AUX_OUT :

	Remov_Func_From_List(AUX_DEV);	
        if (SYS_AUX == work_mode){
	     Set_Curr_Func(last_work_mode);
            return 0;
        }
        break;
#endif		
    case INFO_STOP| KEY_HOLD:
		erp_2_test_mode_enable();
		break;
#ifdef MODE_KEY_SEL_FUNC
#ifdef USE_POWER_KEY_SHORT_FOR_MODE
    case INFO_POWER | KEY_SHORT_UP :	
#else
    case INFO_MODE | KEY_SHORT_UP:
#endif
	       //printf(" ----->Next_Func work_mode :%x -- \r\n",(u16)work_mode);

		mode_reg =Next_Func();
	       //printf(" ----->Next_Func mode_reg:%x -- \r\n",(u16)mode_reg);

		if(work_mode ==mode_reg){
			break;
		}
		else{
			work_mode = mode_reg;
		}
	        //printf(" ----->INFO_MODE %x -- \r\n",(u16)work_mode);

		Set_Curr_Func(work_mode);
	   
		if(work_mode==SYS_MP3DECODE_USB){
			
			given_device = BIT(USB_DISK);
			given_file_number = 1;
			put_msg_lifo(SEL_GIVEN_DEVICE_GIVEN_FILE);
		}
		else if(work_mode==SYS_MP3DECODE_SD){

			given_device = BIT(SDMMC);
			given_file_number = 1;
			put_msg_lifo(SEL_GIVEN_DEVICE_GIVEN_FILE);
		}
#ifdef USE_CD_MCU_MASTER_FUNC
		else if(work_mode==SYS_MCU_CD){
			
	 	}		
#endif
		else if(work_mode==SYS_FMREV){
			cur_sw_fm_band=0;
	 	}
#ifdef AM_RADIO_FUNC	
		else if(work_mode==SYS_AMREV){

#ifdef USE_SYS_MODE_RECOVER
#ifdef UART_ENABLE
    			printf("------->- MEM_SYSMODE saved   work_mode:%d   \r\n",(u16)work_mode);
#endif	

              	write_info(MEM_SYSMODE,work_mode);
#endif	
			if(cur_sw_fm_band==0){
				cur_sw_fm_band=1;
				put_msg_lifo(INFO_NEXT_FM_MODE); 
				break;
			}			
	 	}
#endif		
	 	else if(work_mode==SYS_AUX){

		}
#if defined(IPONE_INDEPENDENT_MODE)
	 	else if(work_mode==SYS_IPH){

		}
#endif
		else if(work_mode==SYS_RTC){
		}
		else if(work_mode==SYS_IDLE){
			//put_msg_lifo(MSG_SYS_IDLE);
		}

      		put_msg_lifo(INFO_NEXT_SYS_MODE);	   
	
	break;
#else
    case INFO_MODE | KEY_SHORT_UP:
        if (work_mode == SYS_USB_DEVICE)
            break;
#ifdef RTC_DISP_IN_IDLE_MODE
        if (work_mode == SYS_FMREV)
            break;
#else
        if (work_mode > SYS_MP3DECODE_SD)
            break;
#endif		
		set_brightness_all_on();

#ifdef RTC_DISP_IN_IDLE_MODE

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

		        if (work_mode == SYS_IDLE)
		            break;		
			disp_scenario = DISP_NORMAL;
	              Disp_Con(DISP_PLAY);
		}
#endif				
		
		break;
#ifdef RTC_DISP_IN_IDLE_MODE
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
        my_music_vol += 2;
    case INFO_VOL_MINUS:
    case INFO_VOL_MINUS | KEY_HOLD :
        //break;
		if (my_music_vol)
            my_music_vol--;
#if 1

	if(my_music_vol > MAX_MAIN_VOL)
		  my_music_vol = MAX_MAIN_VOL;

	if(my_music_vol ==0){

    		Mute_Ext_PA(MUTE);
	}
	else{
    		Mute_Ext_PA(UNMUTE);

	}
	
	dac_mute_control(0, 1);	
	set_sys_vol(my_music_vol);
	
	//write_info(MEM_VOL, music_vol);

	//write_info(MEM_VOL, music_vol);
	
       // printf(" -------> vol %d   \r\n",(u16)my_music_vol);
       // printf(" -------> vol %d   \r\n",(u16)music_vol);

#else
	music_vol=my_music_vol;

        if (music_vol > MAX_MAIN_VOL)
        {
            music_vol = MAX_MAIN_VOL;
        }
        //printf(" -------> vol %d   \r\n",(u16)music_vol);
        //write_info(MEM_VOL, music_vol);
        /**/
	       if ((work_mode < SYS_MP3DECODE_SD)&&(play_status == MUSIC_PAUSE))		  				//暂停播放时不调整主音量
		{
			if (DAC_DECODE == dac_channel)
			{
			    	if (music_vol > MAX_MAIN_VOL)
			        	music_vol = MAX_MAIN_VOL;
			}
			else
			{
				if (music_vol > MAX_MAIN_VOL)
			       	music_vol = MAX_MAIN_VOL;
			}
			Disp_Con(DISP_VOL);
			write_info(MEM_VOL, music_vol);
			main_vol_set(0, CHANGE_VOL_NO_MEM);
			break;
	        }
		dac_mute_control(0, 1);
		write_info(MEM_VOL,main_vol_set(0, SET_USE_CURRENT_VOL));
#endif		
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
#if 0
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
        if ((work_mode < SYS_MP3DECODE_SD) || (work_mode == SYS_FMREV))
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

		if(work_mode < SYS_MP3DECODE_SD){

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
	break;
#ifdef RTC_DISP_FUNC_WHEN_PLAY   
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
	
    }
    return 1;
}
