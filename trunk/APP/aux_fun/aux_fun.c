/*--------------------------------------------------------------------------*/
/**@file     aux_fun.c
   @brief    AUX����ģ��
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

/*----------------------------------------------------------------------------*/
/**@brief  AUX��Ϣ����
   @param  ��
   @return ��
   @note   void deal_aux( void )
*/
/*----------------------------------------------------------------------------*/
void deal_aux( void )
{
    u8 key;

    while (1)
    {
        	dac_out_select(DAC_AMUX1);
		//suspend_sdmmc();

		key = get_msg();
		
		if(dac_cnt > 20)
		{
			dac_sw(0);
		}

	switch (key)
        {
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
    if (!aux_online)
    {
        work_mode = SYS_RTC;//SYS_MP3DECODE;
        return;
    }
	sysclock_div2(1);
    flush_low_msg();
    Disp_Con(DISP_AUX);
	set_max_vol(MAX_ANALOG_VOL, MAX_DIGITAL_VOL);			//����AUXģʽ����������
    deal_aux();
	main_vol_set(0, CHANGE_VOL_NO_MEM);
}
#endif

