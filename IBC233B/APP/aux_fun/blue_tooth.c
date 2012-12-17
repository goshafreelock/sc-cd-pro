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
#include "uart.h"

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

u8 _code BT_REV_CMD[5][7] =
{
	0xAA ,0x00 ,0x03 ,0x01 ,0x02 ,0x00 ,0xFA,				//Power on
  	0xAA ,0x00 ,0x03 ,0x01 ,0x06 ,0x10 ,0xE6,				//|| A connect A2DP
  	0xAA ,0x00 ,0x03 ,0x01 ,0x0B ,0x00 ,0xF1,				//|| A connect AVRCP		
  	0xAA ,0x00 ,0x02 ,0x00 ,0x02 ,0xFC,0x00,				//|| ACK
};

u8 _code BT_AT_CMD[5][7] =
{
	0xAA ,0x00 ,0x03 ,0x02 ,0x00 ,0x32 ,0xC9,				//PLAY
  	0xAA ,0x00 ,0x03 ,0x02 ,0x00 ,0x34 ,0xC7,				// NEXT
  	0xAA ,0x00 ,0x03 ,0x02 ,0x00 ,0x35 ,0xC6,				//PREV	
};
/*----------------------------------------------------------------------------*/
/**@brief  AUX消息处理
   @param  无
   @return 无
   @note   void deal_aux( void )
*/
/*----------------------------------------------------------------------------*/
void Blue_tooth_hdlr( void )
{
    u8 key;
#if defined(BLUE_TOOTH_UART_FUNC)
    blue_tooth_uart_init();
#endif
    Mute_Ext_PA(UNMUTE);

    while (1)
    {
        	dac_out_select(DAC_AMUX0);
		//suspend_sdmmc();

		key = get_msg();
		
		if(dac_cnt > 20)
		{
			dac_sw(0);
		}

	switch (key)
        {
        case INFO_NEXT_SYS_MODE:
		return;
		
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
       Mute_Ext_PA(MUTE);

	BT_PWR_GPIO_ON();
	sysclock_div2(1);
    	flush_low_msg();
    	Disp_Con(DISP_BT);
	delay_10ms(180);		
	set_max_vol(MAX_ANALOG_VOL, MAX_DIGITAL_VOL);			//设置AUX模式的音量上限
    	Blue_tooth_hdlr();

       Mute_Ext_PA(MUTE);
		
	main_vol_set(0, CHANGE_VOL_NO_MEM);
	BT_PWR_GPIO_OFF();
	
}
#endif

