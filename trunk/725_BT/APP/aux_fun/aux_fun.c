/*--------------------------------------------------------------------------*/
/**@file     aux_fun.c
   @brief    AUX任务模块
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#include "aux_fun.h"
#include "fat_memory.h"
#include "voice_time.h"

#ifdef USE_AUX_FUNC			

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
extern bool adkey_detect;
xd_u8 unmute_delay_timer=0;

#if defined(BLUE_TOOTH_UART_FUNC)
#include "blue_tooth.h"
extern bool bt_frame_rev_finished;
extern xd_u8 rev_bluetooth_status;

extern void blue_tooth_uart_init();
extern void blue_tooth_uart_release();
extern void promt_bt_cmd(AT_PROMPT_CMD cmd);
extern u8 bluetooth_cmd_parse(void);
#endif

#ifdef USE_ERP_2_HDLR
xd_u16 aux_erp_timer=0;
void aux_erp_2_timer_hdlr()
{
    	WKUPPND |= BIT(6);  //开VPP的上拉

	erp_2_test_mode_handlr();

	if((WKUPPND&BIT(7))==0){

		aux_erp_timer++;
		if(aux_erp_timer>=(10*60*2)){

    			//CD_PWR_GPIO_OFF();
		    	Disp_Con(DISP_POWER_OFF);
#ifdef USE_POWER_KEY				
			sys_power_down();
#endif
		}
	}
	else{
		
		aux_erp_timer=0;
		
	}
}
#endif

/*----------------------------------------------------------------------------*/
/**@brief  AUX消息处理
   @param  无
   @return 无
   @note   void deal_aux( void )
*/
/*----------------------------------------------------------------------------*/
void deal_aux( void )
{
    u8 cmd_key,key;
	
#if defined(BLUE_TOOTH_UART_FUNC)
    blue_tooth_uart_init();
    promt_bt_cmd(BT_DISPAIR);			
    unmute_delay_timer=6;
#endif

    aux_channel_crosstalk_improve(DAC_AMUX0);
   // delay_10ms(60);	
    //Mute_Ext_PA(UNMUTE);
    set_sys_vol(0);	

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
        case INFO_NEXT_FIL| KEY_HOLD:
		flush_all_msg();								
		break;	
        case INFO_PREV_FIL| KEY_HOLD:
		flush_all_msg();									
		break;			
        case INFO_250_MS :
		
		if((rev_bluetooth_status==BT_DISCONECT_A2DP)||(rev_bluetooth_status==BT_DISCONECT_AVRCP)){
		}
		else if((rev_bluetooth_status==BT_CONECTED_A2DP)||(rev_bluetooth_status==BT_CONECTED_AVRCP)){
			promt_bt_cmd(BT_DISPAIR);			
		}	
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
#ifdef USE_ERP_2_HDLR
		aux_erp_2_timer_hdlr();
#endif
	    if(adkey_detect){
	   	    adkey_detect=0;
	   	    set_sys_vol(my_music_vol);
	    }
            set_brightness_fade_out();
            if (return_cnt < RETURN_TIME)
            {
                return_cnt++;
            }

	     if(unmute_delay_timer>0){
			unmute_delay_timer--;
			if(unmute_delay_timer==0){
	   	    		set_sys_vol(my_music_vol);
				delay_10ms(10);
				Mute_Ext_PA(UNMUTE);
			}
	     }
		 
            if (RETURN_TIME == return_cnt)
            {
                if (DISP_DWORD_NUMBER == curr_menu)
                {
                    cfilenum = 0;
                }
                if (DISP_AUX != curr_menu)
                    Disp_Con(DISP_AUX);
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
void aux_function(void)
{
#ifdef USE_LINE_IN_DETECT_FUNC
	    if (!aux_online)
	    {
	     	 Set_Curr_Func(SYS_MP3DECODE_USB);
	        return;
	    }
#endif	
	Mute_Ext_PA(MUTE);
       aux_channel_crosstalk_improve(DAC_AMUX1);
	BT_PWR_GPIO_ON();
	AUX_PWR_GPIO_ON();		
	sysclock_div2(1);
    	flush_low_msg();
    	Disp_Con(DISP_AUX);
	set_max_vol(MAX_ANALOG_VOL, MAX_DIGITAL_VOL);			//设置AUX模式的音量上限
    	deal_aux();

	Mute_Ext_PA(MUTE);
	
#if defined(BLUE_TOOTH_UART_FUNC)
	blue_tooth_uart_release();
#endif
	
	AUX_PWR_GPIO_OFF();				
	BT_PWR_GPIO_OFF();
	
	main_vol_set(0, CHANGE_VOL_NO_MEM);
}
#endif

