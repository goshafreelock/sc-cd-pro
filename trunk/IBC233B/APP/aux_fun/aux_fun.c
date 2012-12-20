/*--------------------------------------------------------------------------*/
/**@file     aux_fun.c
   @brief    AUX����ģ��
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#include "Custom_config.h"
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

#if defined(BLUE_TOOTH_UART_FUNC)
#include "blue_tooth.h"
extern xd_u8 rev_bluetooth_status;

extern bool bt_frame_rev_finished;

extern void blue_tooth_uart_init();
extern void blue_tooth_uart_release();
extern void promt_bt_cmd(AT_PROMPT_CMD cmd);
extern u8 bluetooth_cmd_parse(void);
#endif

/*----------------------------------------------------------------------------*/
/**@brief  AUX��Ϣ����
   @param  ��
   @return ��
   @note   void deal_aux( void )
*/
/*----------------------------------------------------------------------------*/
void deal_aux( void )
{
    u8 key=0;
#if defined(BLUE_TOOTH_UART_FUNC)
    u8 bt_vol_timer=6;
    u8 cmd_key=0;
    blue_tooth_uart_init();
#if 0
  delay_10ms(20);

    promt_bt_cmd(BT_VOL_P);
	
    while(bt_vol_timer>0){

		if(bt_frame_rev_finished){

			bt_frame_rev_finished=0;
			
			//cmd_key = bluetooth_cmd_parse();
			//if(cmd_key==BT_ACK){
			//	promt_bt_cmd(BT_VOL_P);
				bt_vol_timer--;
			//}	  

		}

		delay_10ms(100);
    		promt_bt_cmd(BT_VOL_P);
		
		if(key++>250){
			bt_vol_timer=0;
			break;
		}
    }	

#endif
	promt_bt_cmd(BT_DISPAIR);			
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
		
        case INFO_250_MS :
		
		if(rev_bluetooth_status==BT_DISCONECT){
		}
		else if((rev_bluetooth_status==BT_CONECTING)){
		}
		else if(rev_bluetooth_status==BT_CONECTED){
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
                if (DISP_AUX != curr_menu)
                    Disp_Con(DISP_AUX);
            }
            break;
#if RTC_ENABLE 
		case INFO_ALM_BELL :					   ///<��������Ϣ����������ѭ��
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
/**@brief  AUX�������
   @param  ��
   @return ��
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

	AUX_PWR_GPIO_ON();		
	sysclock_div2(1);
    	flush_low_msg();
    	Disp_Con(DISP_AUX);

	//delay_10ms(220);
		
	set_max_vol(MAX_ANALOG_VOL, MAX_DIGITAL_VOL);			//����AUXģʽ����������
    	deal_aux();

#if defined(BLUE_TOOTH_UART_FUNC)
	blue_tooth_uart_release();
#endif

    	Mute_Ext_PA(MUTE);
		
	main_vol_set(0, CHANGE_VOL_NO_MEM);

	AUX_PWR_GPIO_OFF();				
	
}
#endif

