/*--------------------------------------------------------------------------*/
/**@file     mcu_master.c
   @brief    mcu_master任务模块
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#include "Custom_config.h"
#include "mcu_master.h"
#include "fat_memory.h"
#include "voice_time.h"

#ifdef CD_MCU_MASTER_MODE			

#include "iic_drv.h"

bool mcu_master_tranceive_tick=0;
extern xd_u16 cfilenum;
extern xd_u8 curr_menu;
extern xd_u8  return_cnt;
extern bit aux_online;
extern xd_u8 work_mode;
extern _idata u16 dac_cnt;
extern bool key_voice_disable;
extern void chk_date_err(void);
extern u8 xdata last_work_mode;
extern bool alarm_on;

xd_u8 send_buf=0;
xd_u8 rev_buf[10]={0};
void mcu_master_info_hdlr()
{

}
void mcu_master_rev()
{
	u8 rev_loop=0,rev_reg=0;
	
	EA =0;
	iic_drv_start();
	iic_drv_sendbyte(0xAA);
	while(rev_loop++<10){

    		rev_reg = iic_drv_revbyte();
    		if(drv_rev_ack()==0){
			rev_buf[rev_loop]=rev_reg;
		}
		else{
			break;
		}
	}
	iic_drv_stop();
	EA =1;
}
void mcu_master_send()
{
	if(send_buf!=0){
		EA =0;
		iic_drv_start();
		iic_drv_sendbyte(0x55);
		iic_drv_sendbyte(send_buf);
		iic_drv_stop();
		EA =1;
		send_buf=0;
	}
}
/*----------------------------------------------------------------------------*/
/**@brief  mcu_hdlr 消息处理
   @param  无
   @return 无
   @note   void mcu_hdlr( void )
*/
/*----------------------------------------------------------------------------*/
void mcu_hdlr( void )
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
void mcu_main_hdlr(void)
{

    sysclock_div2(1);
    flush_low_msg();
    Disp_Con(DISP_AUX);
    set_max_vol(MAX_ANALOG_VOL, MAX_DIGITAL_VOL);			//设置AUX模式的音量上限
    mcu_hdlr();
    main_vol_set(0, CHANGE_VOL_NO_MEM);
}
#endif

