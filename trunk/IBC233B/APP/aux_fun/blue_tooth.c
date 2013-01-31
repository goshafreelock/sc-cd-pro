/*--------------------------------------------------------------------------*/
/**@file     blue_tooth.c
   @brief    blue_tooth 任务模块
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#include "Custom_config.h"


#ifdef USE_BLUE_TOOTH_FUNC			
#include "blue_tooth.h"
#include "fat_memory.h"
#include "voice_time.h"
#include "decode_otp.h"

extern xd_u16 cfilenum;
extern xd_u8 curr_menu;
extern xd_u8  return_cnt;
extern bit aux_online;
extern _xdata SYS_WORK_MODE  work_mode;
extern _idata u16 dac_cnt;
extern bool key_voice_disable;
extern void chk_date_err(void);
extern u8 xdata last_work_mode;
extern bool alarm_on;
extern xd_u8 my_music_vol;
extern bool IR_key_det,adkey_detect;

static bool wait_for_dev_connect=0;
static bool promt_dev_disconnect=0;
static bool force_pairing_mode_enable=0;

#if defined(BLUE_TOOTH_UART_FUNC)
extern bool bt_frame_rev_finished;
xd_u8 rev_bluetooth_status=0;

extern void blue_tooth_uart_init();
extern void blue_tooth_uart_release();
extern void promt_bt_cmd(AT_PROMPT_CMD cmd);
extern u8 bluetooth_cmd_parse(void);
#ifdef BLUETOOTH_CONFIG_DEV_NAME
extern void config_dev_name(void);
#endif
#endif

xd_u8 bt_pwr_off_timer=0,bt_pwr_on_timer=0,retry_timer=0;
void bt_disconnect_power_hldr()
{
	
	if(bt_pwr_on_timer>0){
		bt_pwr_on_timer--;
		if(bt_pwr_on_timer == 0){

    			set_delay_mute();
			BT_PWR_GPIO_ON();
		}
	}
	else{
		
		if((rev_bluetooth_status==BT_DISCONECT_A2DP)||(rev_bluetooth_status==BT_DISCONECT_AVRCP)||(force_pairing_mode_enable)){
		
			bt_pwr_off_timer++;
			
			if(bt_pwr_off_timer>=(2*60)){

				if(retry_timer>0){
					retry_timer--;
				
			  		Mute_Ext_PA(MUTE);

					bt_pwr_off_timer=0;
					bt_pwr_on_timer=3;
					BT_PWR_GPIO_OFF();
				}
				else{
					
					bt_pwr_off_timer=0;
					bt_pwr_on_timer =0;
				}
			}
		}
		else{
			bt_pwr_off_timer=0;
			bt_pwr_on_timer =0;
		}
	}
}
xd_u8 bt_play_status=BT_STA_STOP;
xd_u8 spark_timer=0;
static xd_u8 activate_beep=0,ind_src_voice=0;
void activate_beep_ind(u8 ind_src)
{
	activate_beep=2;
	ind_src_voice=ind_src;
}
void bluetooth_standby_beep()
{
  	if(activate_beep>0){

		activate_beep--;
		if(activate_beep==0){
			
		if(ind_src_voice==BT_POWER_ON){
			
#if OTP_MUSIC_ENABLE
			Mute_Ext_PA(UNMUTE);
    			decode_opt_music_file();
			set_sys_vol(my_music_vol);

#else
			decode_voicetime_file();
#endif
			}
			else if((ind_src_voice==BT_CONECTED_A2DP)||(ind_src_voice==BT_CONECTED_AVRCP)){
				
				dac_sw(1);								//闹钟时打开DAC EN0/1
		        	main_vol_set(20, CHANGE_VOL_NO_MEM);
				analog_vol_set(MAX_ANALOG_VOL);
				set_sys_vol(20);

		        	write_dsp(-2, 22, 0x10);
				Mute_Ext_PA(UNMUTE);
				delay_10ms(60);
		        	//write_dsp(2, 22, 0x10);
				//delay_10ms(20);
				set_sys_vol(my_music_vol);
			}
			ind_src_voice = 0xFF;
			set_sys_vol(my_music_vol);
  		}
  	}
}
/*----------------------------------------------------------------------------*/
/**@brief  AUX消息处理
   @param  无
   @return 无
   @note   void deal_aux( void )
*/
/*----------------------------------------------------------------------------*/
void Blue_tooth_hdlr( void )
{
    u8 key=0;
#if defined(BLUE_TOOTH_UART_FUNC)
    u8 cmd_key=0;
    u8 bt_vol_timer=3;

    blue_tooth_uart_init();

#if 0	
    delay_10ms(20);
	
    promt_bt_cmd(BT_VOL_M);

    while(bt_vol_timer>0){

		if(bt_frame_rev_finished){

			bt_frame_rev_finished=0;
			
			cmd_key = bluetooth_cmd_parse();
			if(cmd_key==BT_ACK){
				promt_bt_cmd(BT_VOL_M);
				bt_vol_timer--;
			}	  

		}

		delay_10ms(20);
		if(key++>250){
			bt_vol_timer=0;
			break;
		}
    }	
#endif

#endif

	bt_pwr_off_timer=0;
	bt_pwr_on_timer =0;
	retry_timer=0;
	force_pairing_mode_enable=0;

	promt_dev_disconnect=0;
	wait_for_dev_connect=1;
    //dac_out_select(DAC_AMUX0);
    //delay_10ms(20);	
    //Mute_Ext_PA(UNMUTE);
    activate_beep_ind(BT_POWER_ON);
    set_sys_vol(my_music_vol);	
    set_delay_mute();
	
#ifdef BLUETOOTH_CONFIG_DEV_NAME
    config_dev_name();
#endif

    while (1)
    {
        	dac_out_select(DAC_AMUX0);
		//suspend_sdmmc();

		key = get_msg();

#if defined(BLUE_TOOTH_UART_FUNC)

		if(bt_frame_rev_finished){

			bt_frame_rev_finished=0;
			cmd_key = bluetooth_cmd_parse();

			//printf("bluetooth_cmd_parse    cmd_key %x \r\n",(u16)cmd_key);
			
			if(cmd_key<BT_ACK){
				rev_bluetooth_status = cmd_key;
			}
			else if(cmd_key==BT_ACK){

#if 1
				if(bt_play_status==(BT_STA_PLAY|BT_STA_MASK)){
		  			 Mute_Ext_PA(UNMUTE);
					bt_play_status=BT_STA_PLAY;
				}
#endif				
			}
			else if(cmd_key ==BT_DEV_ISSUE_PLAY){
					bt_play_status=BT_STA_PLAY;
		  			 Mute_Ext_PA(UNMUTE);
			}
		}
#endif		
		if(dac_cnt > 20)
		{
			dac_sw(0);
		}

	switch (key)
        {
        case INFO_NEXT_SYS_MODE:
		return;
        case INFO_STOP | KEY_SHORT_UP:
#if defined(BLUE_TOOTH_UART_FUNC)					
		if((rev_bluetooth_status==BT_CONECTED_A2DP)||(rev_bluetooth_status==BT_CONECTED_AVRCP)){
			bt_play_status=BT_STA_STOP;
			Mute_Ext_PA(MUTE);
			promt_bt_cmd(BT_STOP);			
		}
#endif		
		break;
        case INFO_PLAY| KEY_LONG:
#if defined(BLUE_TOOTH_UART_FUNC)			
		promt_bt_cmd(BT_FAST_PAIRING_MODE);		
#endif
		force_pairing_mode_enable=1;
		rev_bluetooth_status=BT_DISCONECT_A2DP;
		break;
        case INFO_PLAY| KEY_SHORT_UP:

#if defined(BLUE_TOOTH_UART_FUNC)			
		if((rev_bluetooth_status==BT_CONECTED_A2DP)||(rev_bluetooth_status==BT_CONECTED_AVRCP)){

#if 1			
			if(bt_play_status==BT_STA_PLAY){
				//Mute_Ext_PA(MUTE);
				bt_play_status=BT_STA_PAUSE;
			}
			else if(bt_play_status!=BT_STA_PLAY){
				bt_play_status=BT_STA_PLAY|BT_STA_MASK;
			}
#endif			
			promt_bt_cmd(BT_PLAY);		

		}
#endif		
		break;
        case INFO_NEXT_FIL| KEY_SHORT_UP:
#if defined(BLUE_TOOTH_UART_FUNC)			
		promt_bt_cmd(BT_NEXT);			
#endif
		set_sys_vol(my_music_vol);

		break;	
        case INFO_PREV_FIL| KEY_SHORT_UP:

#if defined(BLUE_TOOTH_UART_FUNC)			
		promt_bt_cmd(BT_PREV);			
#endif
		set_sys_vol(my_music_vol);

		break;	
        case INFO_NEXT_FIL| KEY_HOLD:
		//promt_bt_cmd(BT_VOL_P);									
		break;	
        case INFO_PREV_FIL| KEY_HOLD:
		//promt_bt_cmd(BT_VOL_M);									
		break;	
#if 0		
        case INFO_VOL_PLUS| KEY_SHORT_UP:
		promt_bt_cmd(BT_VOL_P);						
		break;	
        case INFO_VOL_MINUS| KEY_SHORT_UP:
		promt_bt_cmd(BT_VOL_M);									
		break;	
#endif		
        case INFO_250_MS :

#if defined(BLUE_TOOTH_UART_FUNC)			

		spark_timer++;

		if(rev_bluetooth_status==BT_POWER_ON){


			Mute_Ext_PA(MUTE);

	              if(DISP_VOL== curr_menu)break;

			if(spark_timer%4==0){
	             		Disp_Con(DISP_BT);
			}
			else{
	            		Disp_Con(DISP_NULL);
			}

			wait_for_dev_connect = 1;
		}
		else if((rev_bluetooth_status==BT_PAIR_MODE)||(rev_bluetooth_status==BT_PAIR_MODE)){

	              if(DISP_VOL== curr_menu)break;

			if(force_pairing_mode_enable){

				if(spark_timer%2==0){				  	
				//if(spark_timer%2==0){
		                   	Disp_Con(DISP_BT);
				}
				else{
		                    	Disp_Con(DISP_NULL);
				}
			}
			else{

				if(spark_timer%4==0){				  	
				//if(spark_timer%2==0){
		                   	Disp_Con(DISP_BT);
				}
				else{
		                    	Disp_Con(DISP_NULL);
				}
			}
			wait_for_dev_connect = 1;
		}		
		else if((rev_bluetooth_status==BT_DISCONECT_A2DP)||(rev_bluetooth_status==BT_DISCONECT_AVRCP)){

	              if(DISP_VOL== curr_menu)break;
				  	
			if(spark_timer%2==0){
	                   	Disp_Con(DISP_BT);
			}
			else{
	                    	Disp_Con(DISP_NULL);
			}

			wait_for_dev_connect =1;
			
			if(promt_dev_disconnect==0){
				
				promt_dev_disconnect = 1;
#if defined(BLUE_TOOTH_UART_FUNC)			
				promt_bt_cmd(BT_FAST_PAIRING_MODE);		
#endif			
			}
		}
		else if((rev_bluetooth_status==BT_CONECTED_A2DP)||(rev_bluetooth_status==BT_CONECTED_AVRCP)){

			spark_timer=0;
	              if(DISP_NULL == curr_menu){
				 Disp_Con(DISP_BT);
	              }
  			 Mute_Ext_PA(UNMUTE);
			 retry_timer=3;

			force_pairing_mode_enable=0;
			 
			 if(wait_for_dev_connect){
				wait_for_dev_connect =0;
				promt_dev_disconnect = 0;
				activate_beep_ind(BT_CONECTED_AVRCP);
			 }
		}	
#endif		
		break;			
        case INFO_HALF_SECOND :

	     if(adkey_detect){
    			adkey_detect=0;
			set_sys_vol(my_music_vol);
	     }
		 
	     bt_disconnect_power_hldr();
	     ///delay_mute_handler();
    	     bluetooth_standby_beep();

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
            if (return_cnt < RETURN_TIME)
            {
                return_cnt++;
            }

            if (RETURN_TIME == return_cnt)
            {
                if (DISP_DWORD_NUMBER == curr_menu)
                {
                    cfilenum = 0;
                }
                if (DISP_BT != curr_menu)
                    Disp_Con(DISP_BT);
            }
            break;
#if RTC_ENABLE 
		case INFO_ALM_BELL :					   ///<闹钟响消息，进入闹钟循环
			dac_out_select(DAC_DECODE);
			alm_bell_mode();
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
            if (!ap_handle_hotkey(key))
                return;
        }
    }
}

/*----------------------------------------------------------------------------*/
/**@brief  AUX功能入口
   @param  无
   @return 无
   @note   void aux_function(void)
*/
/*----------------------------------------------------------------------------*/
void Blue_tooth_main(void)
{
#ifdef USE_LINE_IN_DETECT_FUNC
	    if (!aux_online)
	    {
	     	 Set_Curr_Func(SYS_MP3DECODE_USB);
	        return;
	    }
#endif	
	bt_play_status=BT_STA_STOP;
       rev_bluetooth_status=BT_POWER_ON;
	   	
       Mute_Ext_PA(MUTE);

	BT_PWR_GPIO_ON();
	sysclock_div2(1);
    	flush_low_msg();
    	Disp_Con(DISP_BT);
	delay_10ms(60);		
	set_max_vol(MAX_ANALOG_VOL, MAX_DIGITAL_VOL);			//设置AUX模式的音量上限
    	Blue_tooth_hdlr();

#if defined(BLUE_TOOTH_UART_FUNC)
	blue_tooth_uart_release();
#endif

       Mute_Ext_PA(MUTE);
		
	main_vol_set(0, CHANGE_VOL_NO_MEM);
	//BT_PWR_GPIO_OFF();
	
}
#endif

