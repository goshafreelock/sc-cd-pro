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
#if defined(BLUE_TOOTH_UART_FUNC)
extern bool bt_frame_rev_finished;
xd_u8 rev_bluetooth_status=0;

extern void blue_tooth_uart_init();
extern void blue_tooth_uart_release();
extern void promt_bt_cmd(AT_PROMPT_CMD cmd);
extern u8 bluetooth_cmd_parse(void);
#endif

xd_u8 bt_play_status=BT_STA_STOP;

xd_u8 spark_timer=0;
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

    dac_out_select(DAC_AMUX0);
    delay_10ms(20);	
    Mute_Ext_PA(UNMUTE);

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
        case INFO_STOP | KEY_SHORT_UP:
		bt_play_status=BT_STA_STOP;
		Mute_Ext_PA(MUTE);
		promt_bt_cmd(BT_STOP);			
		break;
        case INFO_PLAY| KEY_LONG:
		promt_bt_cmd(BT_DISPAIR);			
		break;
        case INFO_PLAY| KEY_SHORT_UP:
		if(bt_play_status==BT_STA_PLAY){
			bt_play_status=BT_STA_PAUSE;
		}
		else if(bt_play_status<=BT_STA_PAUSE){
			bt_play_status=BT_STA_PLAY;
		}
		promt_bt_cmd(BT_PLAY);			
		break;
        case INFO_NEXT_FIL| KEY_SHORT_UP:
		promt_bt_cmd(BT_NEXT);						
		break;	
        case INFO_PREV_FIL| KEY_SHORT_UP:
		promt_bt_cmd(BT_PREV);									
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
			
		spark_timer++;		
		if(rev_bluetooth_status==BT_POWER_ON){


			Mute_Ext_PA(MUTE);

			if(spark_timer%4==0){
	             		Disp_Con(DISP_BT);
			}
			else{
	            		Disp_Con(DISP_NULL);
			}
		}
		else if((rev_bluetooth_status==BT_CONECTING)||(rev_bluetooth_status==BT_DISCONECT)||
				(rev_bluetooth_status==BT_PAIR_MODE)||(rev_bluetooth_status==BT_PAIR_MODE)
		){

			if(spark_timer%2==0){
	                   	Disp_Con(DISP_BT);
			}
			else{
	                    	Disp_Con(DISP_NULL);
			}
		}
		else if(rev_bluetooth_status==BT_CONECTED){

			spark_timer=0;
	              if(DISP_NULL == curr_menu){
				 Disp_Con(DISP_BT);
	              }

  			 Mute_Ext_PA(UNMUTE);

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

       Mute_Ext_PA(MUTE);

	BT_PWR_GPIO_ON();
	sysclock_div2(1);
    	flush_low_msg();
    	Disp_Con(DISP_BT);
	delay_10ms(220);		
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

