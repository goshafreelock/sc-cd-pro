/*--------------------------------------------------------------------------*/
/**@file    fm_receive.c
   @brief   FM接收任务模块
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#include "Custom_config.h"
#include "fm_rev.h"
#include "KT_AMFMdrv.h"
#include "fat_memory.h"


extern xd_u16 cfilenum;
extern xd_u8 curr_menu;
extern xd_u8  return_cnt;
extern bit aux_online;
extern xd_u8 work_mode;
extern void usb_in_check_nopower(void);
extern u8 xdata last_work_mode;
extern bool alarm_on;

/** FM收音当前搜索到的台总数*/
xd_u8 all_channl;
/** 当前频率*/
xd_u16 frequency;
/** FM收音当前所在的台号*/
xd_u8 fre_channl;
xd_u8 fre_point[10]={0};    ///< FM收音搜索到的台的缓存
extern _idata u16 dac_cnt;
extern bit key_voice_disable;
#ifdef ADKEY_SELECT_MODE
extern bool mode_switch_protect_bit;
#endif
#ifdef FAST_STICK_TUNE_FUNC
extern xd_u8 fast_step_cnt;
#endif
xd_u8 freq_step =0;


xd_u8 sw_fm_mod=0,cur_sw_fm_band=0;
xd_u16 REG_MAX_FREQ=0,REG_MIN_FREQ=0;
xd_u8 sw_fm_pos=0;

extern void KT_AMFMSetMode(xd_u8 AMFM_MODE);
extern xd_u8 KT_FMTune(xd_u16 Frequency);
extern xd_u8 KT_AMTune(xd_u16 Frequency);
#ifdef USE_VALIDSTATION_CHECK
extern xd_u8 KT_FMValidStation(xd_u16 Frequency);
extern xd_u8 KT_AMValidStation(xd_u16 Frequency);
extern xd_u8 KT_SMValidStation(xd_u16 Frequency);
extern void KT_Mute_Ctrl(bool m_f);
void load_band_info(void);
#endif
extern xd_u8 KT_AMFMWakeUp(void); //0->Fail 1->Success
extern void KT_AMFMStandby(void);					//0->Fail 1->Success
extern xd_u8 KT_AMFMPreInit(void);		
extern xd_u16 KT_AMGetFreq(void);

#if defined(PWR_CTRL_WKUP)
extern void wkup_pin_ctrl(bool dir);
#endif

#if defined(USE_TIMER_POWER_OFF_FUNC)
extern bool timer_setting_enable;
#endif

extern Str_Band  Current_Band;
extern bool IR_key_det;

#ifdef GPIO_SEL_BAND_INFO_CONFIG
static bool gpio_sel_band_info_config=0;
void scan_gpio_band_info_config()
{
	EA =0;
	gpio_sel_band_info_config=0;
	P0DIR &= ~(BIT(2)); 
	P04=1; 
	P0DIR |= BIT(2);
	P0PU |= BIT(2);
	_nop_();
	_nop_();
	_nop_();

	if(P02){
		gpio_sel_band_info_config=1;
	}

	P0DIR &= ~(BIT(2)); 
	EA =1;
}
bool get_band_info_config()
{
	return gpio_sel_band_info_config;
}
#endif

#if defined(JK2092_AMFM_BK_V001)||defined(AM_FM_BAND_ONLY)
FREQ_RAGE _code radio_freq_tab[MAX_BAND]=
{
	8750,	10800,
	520,		1630,
};
#else
FREQ_RAGE _code radio_freq_tab[MAX_BAND]=
{
	8750,	10800,
	520,		1630,
	3200,	5200,
	5210,	7300,
	7310,	9400,
	9410,	11500,
	11510,	13800,
	13810,	15800,
	15810,	17900,
	17910,	22000,
	22010,	23200,
	23010,	25000,
};
#endif

#ifdef SAVE_BAND_FREQ_INFO

void save_radio_freq(u16 radio_freq,u8 ch)
{
	xd_u8 freq_reg=0;
	ch=ch<<1;
	freq_reg =(u8)radio_freq&(0x00FF);
	write_info(MEM_FM_CHANNL +ch , freq_reg);

	freq_reg =(u8)(radio_freq>>8);
	write_info(MEM_FM_CHANNL +ch+1, freq_reg);
}
u16 read_radio_freq(u8 ch)
{
	xd_u16 freq_reg=0;
	
	ch=ch<<1;
	freq_reg = read_info(MEM_FM_CHANNL +ch+1);
	freq_reg=freq_reg<<8;
	freq_reg |= read_info(MEM_FM_CHANNL +ch);

	return freq_reg;	
}
#endif
void set_radio_freq(u8 mode)
{
   
    set_brightness_all_on();
#if 0
	if(vol_tune_bit){
		vol_tune_bit = 0;
		
	    if(cur_sw_fm_band==0){
			
			freq_step =10; 
	    }
	    else if(cur_sw_fm_band==1){
			freq_step = 1;
	    }
	    else{
			freq_step = 5;
	    }	
	}
	else{		
		freq_step = 10;
	}
#elif defined(FAST_STICK_TUNE_FUNC)

    if(fast_step_cnt==0){
	fast_step_cnt=1;
    }
    if(cur_sw_fm_band==0){
		
		freq_step =10*fast_step_cnt; 
    }
    else if(cur_sw_fm_band==1){

		freq_step = 1*fast_step_cnt;
    }
    else{
		freq_step = 5*fast_step_cnt;
    }
#else
    if(cur_sw_fm_band==0){
		
		freq_step =10; 
    }
    else if(cur_sw_fm_band==1){

#ifdef GPIO_SEL_BAND_INFO_CONFIG
		if(get_band_info_config()){
			freq_step = 10;
		}
		else 
#endif			
		{
			freq_step = 1;
		}
    }
    else{
		freq_step = 5;
    }	
#endif

    if (mode == FM_FRE_INC)
    {
        frequency=frequency+freq_step;
    }
    else if (mode == FM_FRE_DEC)
    {
        frequency=frequency-freq_step;
    }
	
    if (frequency > REG_MAX_FREQ)
        frequency = REG_MAX_FREQ;
	
    if (frequency < REG_MIN_FREQ)
        frequency =REG_MIN_FREQ;

    if(cur_sw_fm_band==0){

		KT_FMTune(frequency);
    }
    else{

		KT_AMTune(frequency);
    }
    Disp_Con(DISP_FREQ);			

#ifdef SAVE_BAND_FREQ_INFO	
    save_radio_freq(frequency,cur_sw_fm_band);
#endif
}
void radio_band_hdlr()
{
	REG_MAX_FREQ = radio_freq_tab[cur_sw_fm_band].MAX_FREQ;
	REG_MIN_FREQ = radio_freq_tab[cur_sw_fm_band].MIN_FREQ;
#ifdef SAVE_BAND_FREQ_INFO	
	frequency = read_radio_freq(cur_sw_fm_band);
#endif

#ifdef USE_VALIDSTATION_CHECK
	load_band_info();
#endif

	KT_AMFMSetMode(cur_sw_fm_band);	

    	set_radio_freq(FM_CUR_FRE);
}
#ifdef SEMI_AUTO_SCAN_FUNC

bool radio_get_validstation(u16 freq)
{
    if(cur_sw_fm_band==0){
	return KT_FMValidStation(freq);
    }
    else if(cur_sw_fm_band==1){
	return KT_AMValidStation(freq);
    }
#ifdef MULTI_BAND_KT_0915_IN_USE	
    else{
	return KT_SMValidStation(freq);
    }
#endif
}
#define SEMI_AUTO_SCAN_KEY_UP		INFO_NEXT_FIL | KEY_LONG
#define SEMI_AUTO_SCAN_KEY_DOWN	INFO_PREV_FIL | KEY_LONG
void semi_auto_scan(u8 scan_dir)
{
    xd_u16 fre_old=frequency;
    u8 key=0;

    flush_low_msg();

   dac_mute_control(1,1);		
    do   
    {
	 key = get_msg();	
	 
        if ((key==(INFO_NEXT_FIL|KEY_SHORT_UP))||
		( key==(INFO_PREV_FIL | KEY_SHORT_UP))||
		( key==(INFO_PLAY |KEY_SHORT_UP))
	    )
        {
            break;
        }
		
	if(( key==INFO_NEXT_SYS_MODE)){
		put_msg_fifo(INFO_NEXT_SYS_MODE);
            	break;
	}
	
	if(( key==INFO_NEXT_FM_MODE))
	{
		put_msg_fifo(INFO_NEXT_FM_MODE);
            	break;
	}
	
	if(scan_dir == SEARCH_UP)
	{
		frequency=frequency+ Current_Band.Seek_Step;
          	if(frequency>REG_MAX_FREQ)frequency = REG_MIN_FREQ;
	}
	else
	{
		frequency=frequency- Current_Band.Seek_Step;
          	if(frequency<REG_MIN_FREQ)frequency =REG_MAX_FREQ;
	}

           Disp_Con(DISP_FREQ);
	
 	if(fre_old ==frequency)
	{
		break;
	}        

	if(radio_get_validstation(frequency))
        {
            Disp_Con(DISP_FREQ);
   	     //dac_mute_control(0,1);		
	     break;
        }

    }while(1);

   set_radio_freq(FM_CUR_FRE);

   dac_mute_control(0,1);		

}
#endif
/*----------------------------------------------------------------------------*/
/**@brief  FM任务函数
   @param  无
   @return 无
   @note   void fm_hdlr( void )
*/
/*----------------------------------------------------------------------------*/
void fm_hdlr( void )
{
    u8 key;	

#ifdef PWR_CTRL_IN_IDLE_MODE

#if defined(PWR_CTRL_WKUP)
	wkup_pin_ctrl(1);
#else
	PWR_CTRL_GPIO_INIT();
	PWR_CTRL_EN();
#endif

#endif

    set_radio_freq(FM_CUR_FRE);
#ifdef ADKEY_SELECT_MODE
    mode_switch_protect_bit=0;
#endif	
    while (1)
    {

	dac_out_select(DAC_AMUX1);

	key = get_msg();

	if(dac_cnt >20){
		dac_sw(0);
	}
        switch (key)
        {
#ifdef UART_ENABLE        
       case INFO_VOL_PLUS:
		KT_AMGetFreq();
	 break;        
#endif	 
        case INFO_NEXT_SYS_MODE:
		return;
        case INFO_NEXT_FM_MODE:
		radio_band_hdlr();			
		break;
#if defined(USE_ADVOLT_FOR_FUNC_SEL_TYPE_FOUR)	
	case INFO_BAND_SEL:
		if(sw_fm_pos==SW_MODE){
			cur_sw_fm_band = read_info(MEM_BAND_SEL);
			if(cur_sw_fm_band>MAX_BAND)cur_sw_fm_band=0;
		}
		goto __BAND_SEL_HANDLE;
		break;
#endif		
#ifdef USE_KEY_FOR_BAND_SELECT
        case INFO_PLAY | KEY_SHORT_UP :
		if(IR_key_det){
			break;
		}

#if defined(USE_TIMER_POWER_OFF_FUNC)
		if(timer_setting_enable){
			timer_pwr_setting();
			break;
		}
#endif		
#if defined(USE_ADVOLT_FOR_FUNC_SEL_TYPE_FOUR)	
		if(sw_fm_pos!=SW_MODE){
			break;
		}
#endif
    	case INFO_EQ_DOWN| KEY_SHORT_UP :
			
#if defined(USE_ADVOLT_FOR_FUNC_SEL_TYPE_FOUR)		
		if(sw_fm_pos==SW_MODE){

			cur_sw_fm_band++;
			if(cur_sw_fm_band>SW8_MODE){
				cur_sw_fm_band = SW_MODE;	
			}

			write_info(MEM_BAND_SEL, cur_sw_fm_band);	
		}

__BAND_SEL_HANDLE:		
#else
		cur_sw_fm_band++;
		if(cur_sw_fm_band>SW8_MODE){
			cur_sw_fm_band = FM_MODE;	
		}
		write_info(MEM_BAND_SEL, cur_sw_fm_band);
#endif		
	       Disp_Con(DISP_BAND_NUM);
		delay_10ms(50);
		flush_low_msg();
		put_msg_fifo(INFO_NEXT_FM_MODE);	
	   
            break;
#endif			
			
#ifdef SEMI_AUTO_SCAN_FUNC
        case SEMI_AUTO_SCAN_KEY_UP:			
		semi_auto_scan(SEARCH_UP);
		break;

        case SEMI_AUTO_SCAN_KEY_DOWN:			
		semi_auto_scan(SEARCH_DN);
		break;	
#endif
        case INFO_FRE_UP | KEY_SHORT_UP:
        case INFO_NEXT_FIL | KEY_SHORT_UP:			
             	set_radio_freq(FM_FRE_INC);
		break;
        case INFO_FRE_DOWN | KEY_SHORT_UP:
        case INFO_PREV_FIL | KEY_SHORT_UP:		
             	set_radio_freq(FM_FRE_DEC);
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

#if defined(USE_TIMER_POWER_OFF_FUNC)	
			timer_setting_enable=0;
#endif            
	            if (DISP_DWORD_NUMBER == curr_menu)			//数字输入模式
	            {
	            		if(cur_sw_fm_band !=1){
					cfilenum = cfilenum*10;
				}
				if ((cfilenum <= REG_MAX_FREQ)&&(cfilenum >=REG_MIN_FREQ)){
			        	frequency =cfilenum;
				    	set_radio_freq(FM_CUR_FRE);
				}
				else{
	    				Disp_Con(DISP_ERROR);
				}
				cfilenum =0;				
	            }	
	 	     else if (DISP_FREQ != curr_menu){
			 	
	                    Disp_Con(DISP_FREQ);
	           }
            }
            break;
    case MSG_USB_DISK_IN  :
    case MSG_SDMMC_IN :
		break;
	
#ifdef USE_RTC_ALARM_FUNCTION
    		case INFO_ALM_BELL:
		    	sys_clock_pll();							
			alm_bell_mode();
             		set_radio_freq(FM_FRE_INC);			
			sysclock_div2(1);
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
/**@brief  调频收音工作模式入口
   @param  无
   @return 无
   @note   void fm_radio(void)
*/
/*----------------------------------------------------------------------------*/
void fm_radio(void)
{
#ifdef UART_ENABLE
	sys_printf(" SYS GO IN FM MODE");
#endif

       if (KT_AMFMWakeUp()==0)
       {
    		Disp_Con(DISP_ERROR);
		delay_10ms(50);
	}
	else{

#ifdef GPIO_SEL_BAND_INFO_CONFIG
		scan_gpio_band_info_config();
#endif		
		sysclock_div2(1);
#if SDMMC_CMD_MODE
		sd_chk_ctl(SET_SD_L_CHK);
#endif

#if defined(USE_ADVOLT_FOR_FUNC_SEL_TYPE_FOUR)	
		if(sw_fm_pos==SW_MODE){
			cur_sw_fm_band = read_info(MEM_BAND_SEL);
			if(cur_sw_fm_band>MAX_BAND)cur_sw_fm_band=0;
		}
#else

#ifdef USE_KEY_FOR_BAND_SELECT
		cur_sw_fm_band = read_info(MEM_BAND_SEL);
		if(cur_sw_fm_band>MAX_BAND)cur_sw_fm_band=0;
#endif

#endif
	     	radio_band_hdlr();   
	    	flush_low_msg();

	    	Disp_Con(DISP_FREQ);
		set_max_vol(MAX_ANALOG_VOL, MAX_DIGITAL_VOL);			//设置FM模式的音量上限
	    	fm_hdlr();

#ifdef ADKEY_SELECT_MODE
	    	mode_switch_protect_bit=1;
#endif			
		main_vol_set(0, CHANGE_VOL_NO_MEM);

	    	KT_AMFMStandby();
#if SDMMC_CMD_MODE
		sd_chk_ctl(SET_SD_H_CHK);
#endif

	}
	
}


