/*--------------------------------------------------------------------------*/
/**@file     rtc_mode.c
   @brief    RTC
   @details
   @author
   @date   2011-3-7
   @note    AC209N
*/
/*----------------------------------------------------------------------------*/
#include "config.h"
#include "fat_memory.h"
#if RTC_ENABLE
#include "rtc_mode.h"
#include "rtc_cpu.h"
#include "rtc_fun.h"
#include "voice_time.h"
extern u16 cfilenum;
extern xd_u8 curr_menu;
extern xd_u8  return_cnt;
extern bit aux_online;
extern xd_u8 work_mode,rtc_setting;
extern bit key_voice_disable;
//extern void disp_rtc(void);
extern u8 _idata music_vol;
extern u8 power_down_cnt;

extern xd_u8 given_device,disp_scenario;
RTC_TIME curr_date;
ALM_TIME curr_alm;
xd_u8 rtc_mode; //_at_ 0x71;			///<0：正常显示；1：设置时间模式；2：设置闹钟模式
xd_u8 rtc_set; //_at_ 0x72;			///<RTC设置界面的方位选择计数器
xd_u8 alm_set;// _at_ 0x73;
xd_u8 alm_flag;
bool alm_sw;
xd_u8 rtc_return_cnt;
#if defined(PWR_CTRL_WKUP)
extern void wkup_pin_ctrl(bool dir);
#endif

void rtc_disp_hdlr(void);

void fond_alm_clk(void);
extern bool voicetime_on;
extern u8 xdata report_nowtime;
extern u8 xdata last_work_mode;
bool alarm_on;
/*----------------------------------------------------------------------------*/
/**@brief 检测闹钟
   @param 	无
   @return  无
   @note void check_alm(void)
*/
/*----------------------------------------------------------------------------*/
void check_alm(void)
{
    u8 rtc_reg;
    rtc_reg = read_rtc_reg();
    if (rtc_reg & BIT(7))
    {
        alm_flag = 1;
	 put_msg_lifo(INFO_ALM_BELL);
    }	
}

/*----------------------------------------------------------------------------*/
/**@brief 获取现在时间
   @param 	void
   @return  void
   @note void get_curr_date(void)
*/
/*----------------------------------------------------------------------------*/
void get_curr_date(void)
{
    sec_date(&curr_date,read_rtc_sec());
}
/*----------------------------------------------------------------------------*/
/**@brief 闹钟跟随当前日期
   @param 	void
   @return  void
   @note void fond_alm_clk(void)
*/
/*----------------------------------------------------------------------------*/

void fond_alm_clk(void)
{
    u8 i;
    RTC_TIME temp_date;
    get_curr_date();
    //memcpy(&temp_date,&curr_date,7);
    for (i = 0; i < sizeof(RTC_TIME); i++)
    {
        ((u8 *)(&temp_date))[i] =  ((u8 *)(&curr_date))[i];
    }
    if ((curr_alm.hour<curr_date.hour)||((curr_alm.hour==curr_date.hour)&&(curr_alm.minutes<=curr_date.minutes)))
        temp_date.day+=1;
    temp_date.second=0;
    temp_date.hour=curr_alm.hour;
    temp_date.minutes=curr_alm.minutes;
    send_alm_sec(date_sec(&temp_date,0));
}

/*----------------------------------------------------------------------------*/
/**@brief 设置闹钟时间
   @param 	void
   @return  void
   @note void set_alm_sec(void)
*/
/*----------------------------------------------------------------------------*/
void set_alm_sec(void)
{

    if (alm_sw)	enable_alm();
    else   disable_alm();
    fond_alm_clk();
    write_info(MEM_ALM_HOUR,curr_alm.hour);
    write_info(MEM_ALM_MIN,curr_alm.minutes);
}


/*----------------------------------------------------------------------------*/
/**@brief 设置现在时间
   @param 	void
   @return  void
   @note void set_date_sec(void)
*/
/*----------------------------------------------------------------------------*/
void set_date_sec(void)
{
    u32 sec;
    sec = date_sec(&curr_date,1);
    send_rtc_sec(sec);
}
/*----------------------------------------------------------------------------*/
/**@brief 日期超限校验
   @param 	void
   @return  void
   @note void chk_date_err(void)
*/
/*----------------------------------------------------------------------------*/
void chk_date_err(void)
{
    if (curr_date.year>2135)
    {
        send_rtc_sec(0);
        sec_date(&curr_date,0);
        fond_alm_clk();
    }
}
/*----------------------------------------------------------------------------*/
/**@brief RTC 初始化
   @param 	void
   @return  void
   @note void init_rtc(void)
*/
/*----------------------------------------------------------------------------*/
bool init_rtc(void)
{
    u8 rtc_con;
    bool alm_wkup;

    curr_alm.hour = read_info(MEM_ALM_HOUR);
    curr_alm.minutes = read_info(MEM_ALM_MIN);
    if (curr_alm.hour > 23)
    {
        curr_alm.hour = 0;
    }
    if (curr_alm.minutes > 59)
    {
        curr_alm.minutes = 0;
    }
    alm_sw = 0;
    rtc_con=read_rtc_reg();			 //读出IRTCON = 0x05；32KHz enable,32KHz output

    alm_wkup = 0;

    //printf("%u\n",(u16)rtc_con);
    if (rtc_con & BIT(6))   ///<RTC是否完全掉电
    {
        	send_rtc_sec(0);
		reset_rtc_ram();
    }
    else
    {
        if (rtc_con & (1<<1))    ///<RTC是否开闹钟
        {
            alm_sw = 1;
        }
        if (rtc_con & BIT(7))   ///<闹钟唤醒
        {
            alm_wkup=1;
            alm_flag = 1;
			//put_msg_lifo(INFO_ALM_BELL);
        }
    }
    get_curr_date();
    chk_date_err();
    fond_alm_clk();

    rtc_con &= ~(BIT(6)); 				 //清零PDGLG
    write_rtc_reg(rtc_con);
    return alm_wkup;
}
/*----------------------------------------------------------------------------*/
/**@brief 关闹钟闹铃
   @param 	void
   @return  void
   @note void close_alm_bell(void)
*/
/*----------------------------------------------------------------------------*/
void close_alm_bell(void)
{
	if (alm_flag)
	{
//	    sys_clock_pll();
	    set_alm_sec();
	    alm_flag = 0;
	    main_vol_set(0, SET_USE_CURRENT_VOL);
		delay_10ms(40);				//等待数字音量调节延时，避免声音突变
	    key_voice_disable = 0;
		rtc_mode = RTC_PLAY_MODE;
		return_cnt = RETURN_TIME - 1;
#ifdef PWR_CTRL_IN_IDLE_MODE

#if defined(PWR_CTRL_WKUP)
	wkup_pin_ctrl(0);
#else
	PWR_CTRL_GPIO_INIT();
	PWR_CTRL_DIS();
#endif

#endif
        	Disp_Con(DISP_RTC);
	}
}

/*----------------------------------------------------------------------------*/
/**@brief 打开闹钟
   @param 	void
   @return  bool 1：有响铃；0：无响铃
   @note bool alm_bell(void)
*/
/*----------------------------------------------------------------------------*/
bool alm_bell(void)
{
    if (alm_flag>=60)
    {
        close_alm_bell();
    }
    else if (alm_flag)
    {
		rtc_mode = ALM_UP_MODE;
        	alm_flag++;
        	key_voice_disable = 1;
//        sys_clock_pll();
		dac_sw(1);								//闹钟时打开DAC EN0/1
        	main_vol_set(30, CHANGE_VOL_NO_MEM);
		analog_vol_set(max_analog_vol);
		if (alm_flag > 3)						//去除因调用时间而导致时间不均匀的第一声闹铃
        	write_dsp(-2, 6, 0x12);
        	Disp_Con(DISP_ALM_UP);
	       return 1;//break;
    }
    return 0;
}
/*----------------------------------------------------------------------------*/
/**@brief 闹钟 模式响应的消息
   @param 	void
   @return  返回级数
   @note u8 alm_bell_key(void)
*/
/*----------------------------------------------------------------------------*/
u8 alm_bell_key(u8 key)
{
    switch (key)
    {
    	case MSG_USB_DISK_OUT: 
    	case MSG_SDMMC_OUT:
	case MSG_AUX_IN :
	case MSG_AUX_OUT :
    	case MSG_SDMMC_IN :
    	case MSG_USB_DISK_IN  : 
		break;
    	case INFO_PLAY | KEY_SHORT_UP :
    	case INFO_MODE | KEY_SHORT_UP :
   	case INFO_NEXT_FIL| KEY_SHORT_UP :
    	case INFO_PREV_FIL| KEY_SHORT_UP :
    	case INFO_EQ_UP| KEY_SHORT_UP :
    	case INFO_PLAY_MODE| KEY_SHORT_UP :
    	case INFO_VOL_MINUS| KEY_SHORT_UP :
    	case INFO_VOL_PLUS| KEY_SHORT_UP :
		return 1;
    }
	return 0;	
}

/*----------------------------------------------------------------------------*/
/**@brief 闹钟 模式主循环
   @param 	void
   @return  返回级数目录
   @note u8 alm_bell_mode(void)
*/
/*----------------------------------------------------------------------------*/
u8 alm_bell_mode(void)
{
	u8 key;
	u8 flag;
	alm_flag = 2;

#ifdef PWR_CTRL_IN_IDLE_MODE

#if defined(PWR_CTRL_WKUP)
	wkup_pin_ctrl(1);
#else
	PWR_CTRL_GPIO_INIT();
	PWR_CTRL_EN();
#endif

#endif
	dac_mute_control(0,0);
    	flush_low_msg();
	sysclock_div2(1);
	set_max_vol(MAX_ANALOG_VOL, MAX_DIGITAL_VOL);			//设置RTC模式的音量上限
    	dac_out_select(DAC_DECODE);
    	fond_alm_clk();
	
	while(1)
	{
#if ((USB_DEVICE_CFG & USB_MASS) == USB_MASS)
        if(SYS_USB_DEVICE == work_mode)
		{
		    UFI_massstorage();
		}
#endif
		key = get_msg();

		switch (key)
		{
			case INFO_HALF_SECOND :
#if ((USE_DEVICE == MEMORY_STYLE)&&(FAT_MEMORY))         
            updata_fat_memory();
#endif
				if(alm_bell())
				{
					break;
				}
				return 0;

			default :
			flag = alm_bell_key(key);
            		if (flag)
            		{
                		close_alm_bell();
				if(flag == 2)
					return 1;
				if(flag == 1)
					return 0;
            		}
		}
	}	
}
void rtc_disp_hdlr(void)
{
    	if(disp_scenario == DISP_ALM_SCEN){

		Disp_Con(DISP_ALARM);
	}
	else{
		
	  	if(rtc_setting==0){	     
			get_curr_date();
	       }
		Disp_Con(DISP_RTC);
	}
}
/*----------------------------------------------------------------------------*/
/**@brief RTC 模式主循环
   @param 	void
   @return  void
   @note void deal_rtc(void)
*/
/*----------------------------------------------------------------------------*/
void deal_rtc(void)
{
    u8 key,flag;

    while (1)
    {
        key = get_msg();
		
        switch (key)
        {
        case INFO_PLAY | KEY_LONG:				//RTC闹钟设置模式
			if (rtc_mode != ALM_UP_MODE)
			{
            	rtc_mode++;
				if (rtc_mode == RTC_SET_MODE)
				{
					rtc_return_cnt = 0;
				}
            	if (rtc_mode>ALM_SET_MODE)
            	{
                	rtc_mode = RTC_PLAY_MODE;
            	}
            	rtc_set=0;
            	alm_set=0;
            	Disp_Con(DISP_RTC);
			}
            break;

        case INFO_PLAY | KEY_SHORT_UP:
            if (rtc_mode == RTC_SET_MODE)
            {
               	rtc_return_cnt = 0;
			    rtc_set++;
                if (rtc_set>5)
                {
                    rtc_set = 0;
                }
				Disp_Con(DISP_RTC);
            }
            else if (rtc_mode == ALM_SET_MODE)
            {
                alm_set++;
                if (alm_set>2)
                {
                    alm_set=0;
                }
				Disp_Con(DISP_RTC);
            } 
            break;

        case INFO_VOL_PLUS :
        case INFO_VOL_PLUS | KEY_HOLD :			  //RTC 设置模式下的时间选择（增加）
            if (rtc_mode==RTC_SET_MODE)
            {
                rtc_return_cnt = 0;
				curr_date_plus();
                set_date_sec();
                fond_alm_clk();
				Disp_Con(DISP_RTC);
            }
            else if (rtc_mode==ALM_SET_MODE)
            {
                alm_time_plus();
                set_alm_sec();
				Disp_Con(DISP_RTC);
            }
            
            break;

        case INFO_VOL_MINUS :
        case INFO_VOL_MINUS | KEY_HOLD :		   //RTC 设置模式下的时间选择（减少）
            if (rtc_mode==RTC_SET_MODE)
            {
                rtc_return_cnt = 0;
				curr_date_minus();
                set_date_sec();
                fond_alm_clk();
				Disp_Con(DISP_RTC);
            }
            else if (rtc_mode==ALM_SET_MODE)
            {
                alm_time_minus();
                set_alm_sec();
				Disp_Con(DISP_RTC);
            }

            break;

        case INFO_HALF_SECOND :
#if ((USE_DEVICE == MEMORY_STYLE)&&(FAT_MEMORY))          
             updata_fat_memory();
#endif            
#if VOICE_TIME_ENABLE
            if(report_nowtime == 1)            //报完当前时间，则退回之前模式
            {               
                report_nowtime =0;
                decode_voicetime_file(); 
                work_mode = last_work_mode;
                delay_10ms(50);
                return;
            }
            if((curr_date.minutes == 0)&&(voicetime_on ==0))       //每隔1小时报时
            {
               voicetime_on = 1;
               decode_voicetime_file();  
//               main_vol_set(0, SET_USE_CURRENT_VOL);
            } 
            else if((curr_date.minutes != 0)&&(voicetime_on ==1)) 
            {
               voicetime_on = 0;
            }  
#endif  
			if(alm_flag)
				break;
            power_down_cnt++;
            if (power_down_cnt > 120)
            {
                //sys_power_down();
            }

            if (DISP_RTC == curr_menu)
            {
                if ((rtc_mode<2 && (!alm_flag)))					//除闹钟模式外一直更新now_time
                {
                    get_curr_date();
                    chk_date_err();
                    Disp_Con(DISP_RTC);
                }
            }

            if (return_cnt < (RETURN_TIME))
            {
                return_cnt++;
            }
            if ((RETURN_TIME) == return_cnt)
            {
                if ((DISP_RTC != curr_menu) || (0 != rtc_mode))
                {
                    rtc_mode = 0;
                    get_curr_date();
                    Disp_Con(DISP_RTC);
                }
            }
			if ((rtc_return_cnt < RETURN_TIME)&&(rtc_mode == RTC_SET_MODE))
			{
				rtc_return_cnt++;
			}
			if ((RETURN_TIME == rtc_return_cnt)&&(rtc_mode == RTC_SET_MODE))
			{
				rtc_mode = 0;
				get_curr_date();
				Disp_Con(DISP_RTC);
			}
            break;
		case INFO_ALM_BELL :				 ///<闹钟响消息，进入闹钟循环
#if VOICE_TIME_ENABLE
             get_curr_date();
             Disp_Con(DISP_ALM_UP);
             flag = decode_voicetime_file();
             if( 2 == flag)                    //闹钟响之前先报时，再响闹
             {   
                 alarm_on =0;
                 return;
             }
             else if(1 == flag)
             {
                if(alarm_on)
                {
                  alarm_on =0;
                  work_mode = last_work_mode;  
                  return;	
                }
                else
                {
                   alarm_on =0;
                   break;
                }
             }
#endif 
            if(music_vol == 0)
            {
               dac_mute_control(0,0);
               dac_unmute();
            }
            flag = alm_bell_mode();
            if(music_vol == 0)
            {
               dac_mute();
               dac_mute_control(1,0);
            }
            if(2 == flag)
            {
               alarm_on =0;
               return;
            }
            else if((1 == flag)||(0 == flag))
            {
              if(alarm_on)
              {
                  alarm_on =0;
                  work_mode = last_work_mode;  
                  return;	
               }
               else
               {
                 alarm_on =0;
                 break;	
               }	   
            }
        default :
            if (!ap_handle_hotkey(key))
                return;
        }
    }
}

/*----------------------------------------------------------------------------*/
/**@brief RTC 模式
   @param 	void
   @return  void
   @note void rtc_function(void)
*/
/*----------------------------------------------------------------------------*/
void rtc_function(void)
{
    rtc_mode = RTC_PLAY_MODE;
    rtc_set = 0;
    alm_set = 0;
    flush_low_msg();

	sysclock_div2(1);
	set_max_vol(MAX_ANALOG_VOL, MAX_DIGITAL_VOL);			//设置RTC模式的音量上限
    dac_out_select(DAC_DECODE);
	if (alm_flag)	 //闹钟响时默认进入闹钟模式
    {
		rtc_mode = ALM_UP_MODE;
        	Disp_Con(DISP_ALM_UP);
		put_msg_lifo(INFO_ALM_BELL);
        	goto _deal_rtc0;
    }
    else
    {
        	get_curr_date();
        	Disp_Con(DISP_RTC);
    }
_deal_rtc0:
	
    deal_rtc();
	main_vol_set(0, CHANGE_VOL_NO_MEM);
}
#endif
