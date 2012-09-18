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

#ifdef USE_CD_MCU_MASTER_FUNC			

#include "iic_drv.h"

bool mcu_master_tranceive_tick=0;

xd_u8 cd_play_status=0;

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

xd_u8 fast_fr_release_cnt=0;
xd_u8 send_buf=0;
xd_u8 rev_buf[10]={0};

void mcu_master_info_hdlr()
{
	static u8 info_dispatch_div=0;
	if(fast_fr_release_cnt>0){

		fast_fr_release_cnt--;
		if(fast_fr_release_cnt==0){
			send_buf = FFR_OFF_CMD;
		}
	}

	info_dispatch_div++;
	if(info_dispatch_div==5){

		if(rev_buf[0]&0x01){

		}

	}
}
void mcu_master_rev()
{
	u8 rev_loop=0,rev_reg=0;
	
	EA =0;
	iic_drv_start();
	iic_drv_sendbyte(MASTER_READ_CMD);
	while(rev_loop<10){

    		rev_reg = iic_drv_revbyte_io();
    		if(drv_rev_ack()==0){
			rev_buf[rev_loop]=rev_reg;
			//printf("--------------------%x  ----rev  %x \r\n",(u16)rev_loop,(u16)rev_reg);
		}
		else{
			break;
		}

		rev_loop++;
	}
	iic_drv_stop();	
	EA =1;

    	//printf("---111-mcu_master_rev   %x \r\n",(u16)ack1);
    	//printf("------222-mcu_master_rev   %x \r\n",(u16)ack2);		
}
void mcu_master_send()
{
	bool ack1,ack2;
	if(send_buf!=0){
		EA =0;
		iic_drv_start();
		ack1=iic_drv_sendbyte(MASTER_SEND_CMD);
		ack2=iic_drv_sendbyte(send_buf);
		iic_drv_stop();	
		EA =1;
#if 0		
	    	printf("---111-drv_rev_ack   %x \r\n",(u16)ack1);
	    	printf("---222-drv_rev_ack   %x \r\n",(u16)ack2);		
#endif			
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
		//suspend_sdmmc();
  	dac_out_select(DAC_AMUX1);
		
	key = get_msg();
		
	if(dac_cnt > 20)
	{
		dac_sw(0);
	}
	if(mcu_master_tranceive_tick){
		mcu_master_tranceive_tick =0;
		mcu_master_info_hdlr();
		mcu_master_rev();
		mcu_master_send();
	}
	switch (key)
	{

	        case INFO_NEXT_SYS_MODE:
			return;
	        case INFO_PLAY | KEY_SHORT_UP :
			if(cd_play_status== MUSIC_PLAY){
				cd_play_status=MUSIC_PAUSE;
				send_buf=PAUSE_CMD;
			}
			else if(cd_play_status == MUSIC_PAUSE){

					cd_play_status=MUSIC_PLAY;
				send_buf=PLAY_RESUME_CMD;
			}

			printf("---- -INFO_PLAY  %x \r\n",(u16)send_buf);
			
			break;
	        case INFO_STOP| KEY_SHORT_UP :
			if(cd_play_status!= MUSIC_STOP){
				cd_play_status=MUSIC_STOP;			
				send_buf=STOP_CMD;
			}
			break;
	        case INFO_NEXT_FIL | KEY_SHORT_UP:
			send_buf=NEXT_FILE_CMD;
			break;
	        case INFO_PREV_FIL | KEY_SHORT_UP:
			send_buf=PREV_FILE_CMD;
			break;			
	        case INFO_NEXT_FIL | KEY_HOLD:
			fast_fr_release_cnt=60;
			send_buf=FAST_F_CMD;
			break;
	        case INFO_PREV_FIL | KEY_HOLD:
			fast_fr_release_cnt=60;			
			send_buf=FAST_R_CMD;
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
	                if (DISP_SCAN_DISK != curr_menu)
	                    Disp_Con(DISP_SCAN_DISK);
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
/**@brief  mcu_main_hdlr功能入口
   @param  无
   @return 无
   @note   void mcu_main_hdlr(void)
*/
/*----------------------------------------------------------------------------*/
void mcu_main_hdlr(void)
{

    CD_PWR_GPIO_CTRL_INIT();
    CD_PWR_GPIO_ON();
    sysclock_div2(1);
    flush_low_msg();
    Disp_Con(DISP_SCAN_DISK);
    set_max_vol(MAX_ANALOG_VOL, MAX_DIGITAL_VOL);			//设置AUX模式的音量上限
    mcu_hdlr();
    main_vol_set(0, CHANGE_VOL_NO_MEM);
    CD_PWR_GPIO_OFF();	
}
#endif

