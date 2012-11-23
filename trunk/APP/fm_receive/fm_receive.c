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
extern _xdata SYS_WORK_MODE  work_mode;
extern void usb_in_check_nopower(void);
extern u8 xdata last_work_mode;
extern bool alarm_on;

/** FM收音当前搜索到的台总数*/
xd_u8 all_channl;
/** 当前频率*/
xd_u16 frequency;
/** FM收音当前所在的台号*/
xd_u8 fre_channl;
//xd_u16 fre_preset[10]={0};    ///< FM收音搜索到的台的缓存
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
xd_u8 station_save_pos=0,station_sel_pos=0;

#ifdef RADIO_ST_INDICATOR
bool radio_st_ind=0;
#endif

#ifdef USE_RADIO_FUNC

extern void KT_AMFMSetMode(xd_u8 AMFM_MODE);
extern xd_u8 KT_FMTune(xd_u16 Frequency);
extern xd_u8 KT_AMTune(xd_u16 Frequency);
#ifdef SEMI_AUTO_SCAN_FUNC
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
extern bool IR_key_det,adkey_detect;

void save_radio_freq(u16 radio_freq,u8 ch)
{
	xd_u8 freq_reg=0;

	freq_reg =(u8)radio_freq&(0x00FF);
	write_info(ch, freq_reg);

	freq_reg =(u8)(radio_freq>>8);
	write_info(ch+1, freq_reg);
}
u16 read_radio_freq(u8 ch)
{
	xd_u16 freq_reg=0;

	freq_reg = read_info(ch+1);
	freq_reg=freq_reg<<8;
	freq_reg |= read_info(ch);

	return freq_reg;	
}

#ifdef GPIO_SEL_BAND_INFO_CONFIG
static xd_u8 gpio_sel_band_info_config=0;
void scan_gpio_band_info_config()
{
	EA =0;
	P0DIR &= ~(BIT(7)); 
	P07=1; 
	P0DIR |= BIT(7);
	P0PU |= BIT(7);
	_nop_();
	_nop_();

	gpio_sel_band_info_config=0;

	if(P07){
		gpio_sel_band_info_config=1;
	}

	P0DIR &= ~(BIT(7)); 
	EA =1;
}
u8 get_band_info_config()
{
	return gpio_sel_band_info_config;
}
#endif

u16 _code fm_p_freq[6]={8750,9000,9800,10600,10800,8750};
u16 _code am_eur_freq[6]={522,603,999,1404,1620,522};
u16 _code am_usa_freq[6]={520,600,1000,1400,1710,520};

void load_preset_table(u8 pre_cmd)
{
	u8 i=0;
	u16 *p;
	u8 epprom_offset=0;

	u16 fre_preset[10]={0};
	
	if(pre_cmd==GET_FM_PRESET_FROM_EPPROM){

		for(i=0;i<10;i++){
			fre_preset[i]=read_radio_freq(2*i+FM_CH_OFFSET);
		}
		return;
	}
	else if(pre_cmd==GET_AM_PRESET_FROM_EPPROM){
		for(i=0;i<10;i++){
			fre_preset[i]=read_radio_freq(2*i+AM_CH_OFFSET);
		}
		return;
	}	
	else if(pre_cmd==RESET_FM_PRESET){
		p=fm_p_freq;
		epprom_offset=FM_CH_OFFSET;
	}
	else if(pre_cmd==RESET_EUR_AM_PRESET){
		p=am_eur_freq;
		epprom_offset=AM_CH_OFFSET;
	}
	else if(pre_cmd==RESET_USA_AM_PRESET){
		p=am_usa_freq;
		epprom_offset=AM_CH_OFFSET;
	}
	
	for(i=0;i<10;i++){
		
		fre_preset[i]=*(p);
		
		if(i<5)p++;
	}

	for(i=0;i<10;i++){
#ifdef UART_ENABLE
		printf("------->-station form  TABLE  fre:%4u  at : %d \r\n",fre_preset[i],(u16)((i*2)+epprom_offset));
#endif
		save_radio_freq(fre_preset[i],(i*2)+epprom_offset);
	}
}
void radio_preset_init()
{
	u8 preset_reg=0;

	preset_reg =read_info(MEM_PRESET_REG);

	if(((preset_reg&PRESET_MASK)==PRESET_OK)&&((preset_reg&PRESET_ZONE_MASK)==gpio_sel_band_info_config)){		
#ifdef UART_ENABLE
	    	printf("------->-station form  epprom    \r\n");
#endif
		//4 load preset station form  epprom
		if(cur_sw_fm_band==0)
			load_preset_table(GET_FM_PRESET_FROM_EPPROM);
		else if(cur_sw_fm_band==1)
			load_preset_table(GET_AM_PRESET_FROM_EPPROM);
	}
	else{
#ifdef UART_ENABLE
	    	printf("------->-station form  TABLE   \r\n");
#endif		
		//4  reset FM Preset
		load_preset_table(RESET_FM_PRESET);


		//4 reset AM Preset
		if(get_band_info_config()==0)
			load_preset_table(RESET_USA_AM_PRESET);
		else
			load_preset_table(RESET_EUR_AM_PRESET);
			
		preset_reg=PRESET_OK|gpio_sel_band_info_config;
		write_info(MEM_PRESET_REG , preset_reg);

	}	
}
#if defined(AM_FM_BAND_ONLY)
FREQ_RAGE _code radio_freq_tab_EUR[MAX_BAND]=
{
	8750,	10800,
	522,		1620,
};
FREQ_RAGE _code radio_freq_tab_USA[MAX_BAND]=
{
	8750,	10800,
	520,		1710,
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

void set_radio_freq(u8 mode,bool disp_pro)
{

    set_brightness_all_on();

    freq_step = Current_Band.Tune_Step;
	
    if (mode == FM_FRE_INC)
    {
        frequency=frequency+freq_step;
		
	 if (frequency > REG_MAX_FREQ)
	     frequency = REG_MIN_FREQ;		
    }
    else if (mode == FM_FRE_DEC)
    {
        frequency=frequency-freq_step;
		
	 if (frequency < REG_MIN_FREQ)
	      frequency =REG_MAX_FREQ;		
    }
    else{
		
	    if (frequency > REG_MAX_FREQ)
	        frequency = REG_MAX_FREQ;
		
	    if (frequency < REG_MIN_FREQ)
	        frequency =REG_MIN_FREQ;
    }
	
    if(cur_sw_fm_band==0){

		KT_FMTune(frequency);
    }
    else{

		KT_AMTune(frequency);
    }

    if(disp_pro)	
	Disp_Con(DISP_FREQ);			

#if 1//def SAVE_BAND_FREQ_INFO
#ifdef UART_ENABLE
    printf("------->- set_radio_freq    fre:%4u   \r\n",frequency);
#endif
    save_radio_freq(frequency,cur_sw_fm_band*2+MEM_FREQ_BASE);
#endif


}
void radio_band_hdlr()
{
#ifdef GPIO_SEL_BAND_INFO_CONFIG

	if(get_band_info_config()==0){
		
		REG_MAX_FREQ = radio_freq_tab_USA[cur_sw_fm_band].MAX_FREQ;
		REG_MIN_FREQ = radio_freq_tab_USA[cur_sw_fm_band].MIN_FREQ;
	}
	else
#endif
	{
		REG_MAX_FREQ = radio_freq_tab_EUR[cur_sw_fm_band].MAX_FREQ;
		REG_MIN_FREQ = radio_freq_tab_EUR[cur_sw_fm_band].MIN_FREQ;
	}
	
#if 1//def SAVE_BAND_FREQ_INFO	
	frequency = read_radio_freq(cur_sw_fm_band*2+MEM_FREQ_BASE);
#ifdef UART_ENABLE
    	printf("------->- radio_band_hdlr    fre:%4u   \r\n",frequency);
#endif
#endif

	 if((frequency > REG_MAX_FREQ)||(frequency < REG_MIN_FREQ))		
	 	frequency =REG_MIN_FREQ;
		
	station_save_pos=0;
	station_sel_pos=0;

	write_info(MEM_BAND_SEL,cur_sw_fm_band);

	load_band_info();

	KT_AMFMSetMode(cur_sw_fm_band);	

    	set_radio_freq(FM_CUR_FRE,SHOW_FREQ);
}
void restore_last_radio_band()
{
	cur_sw_fm_band = read_info(MEM_BAND_SEL);
	if(cur_sw_fm_band>MW_MODE)cur_sw_fm_band=FM_MODE;
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
	return 0;
}

#define BAND_FULL_SCAN_KEY	(INFO_PLAY |KEY_LONG)
void full_band_scan_hdlr()
{
    u8 key=0;

    	flush_all_msg();
#ifdef RADIO_ST_INDICATOR
    	radio_st_ind=0;
#endif

#ifdef UART_ENABLE
	sys_printf(" RADIO ALL SCAN  MODE");
#endif

    	adkey_detect=0;

	frequency =REG_MIN_FREQ;
	station_save_pos=0;
	station_sel_pos=0;

   	dac_mute_control(1,1);		
	
	while(1)
	{
	 key = get_msg();	
	 
        if ((key==(INFO_NEXT_FIL|KEY_SHORT_UP))||
		( key==(INFO_PREV_FIL | KEY_SHORT_UP))||
		( key==BAND_FULL_SCAN_KEY)
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

	if(adkey_detect){
		adkey_detect=0;
		break;
	}
	
	 frequency=frequency+ Current_Band.Seek_Step;
        if(frequency>=REG_MAX_FREQ){

			frequency = REG_MAX_FREQ;
        		Disp_Con(DISP_FREQ);
			break;
        }

        Disp_Con(DISP_FREQ);

	 if(radio_get_validstation(frequency))
        {	
#ifdef RADIO_ST_INDICATOR
	     	radio_st_ind=1;
#endif
            	//Disp_Con(DISP_FREQ);
       	Disp_Con(DISP_SAVE_POS);

	     	if(cur_sw_fm_band==0){
			save_radio_freq(frequency,station_save_pos*2+FM_CH_OFFSET);
	     	}
	     	else if(cur_sw_fm_band==1){
			save_radio_freq(frequency,station_save_pos*2+AM_CH_OFFSET);
	     	}

		delay_10ms(120);
		station_save_pos++;   

		if(station_save_pos>Current_Band.MAX_CH){
			
			break;
		}			 
        }

    }

	if(cur_sw_fm_band==0){

		frequency = read_radio_freq(FM_CH_OFFSET);		
		write_info(MEM_FM_ALL_CH,station_save_pos);
	}
	else if(cur_sw_fm_band==1){

		frequency = read_radio_freq(AM_CH_OFFSET);		
		write_info(MEM_AM_ALL_CH,station_save_pos);
	}
	
	set_radio_freq(FM_CUR_FRE,SHOW_FREQ);

   	dac_mute_control(0,1);		

#ifdef UART_ENABLE
	sys_printf(" FINISH RADIO ALL SCAN  MODE");
#endif

}
#define SEMI_AUTO_SCAN_KEY_UP		INFO_NEXT_FIL | KEY_LONG
#define SEMI_AUTO_SCAN_KEY_DOWN	INFO_PREV_FIL | KEY_LONG
void semi_auto_scan(u8 scan_dir)
{
    xd_u16 fre_old=frequency;
    u8 key=0;

    flush_low_msg();
#ifdef RADIO_ST_INDICATOR
    radio_st_ind=0;
#endif

    adkey_detect=0;

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
	if(adkey_detect){

		adkey_detect=0;
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
	     break;
        }

    }while(1);

   set_radio_freq(FM_CUR_FRE,SHOW_FREQ);

   dac_mute_control(0,1);		

}
#endif
#ifdef FM_SAVE_STATION_MANUAL

#define MANUAL_STATION_SAVE_KEY	(INFO_MODE|KEY_LONG)

void radio_save_station_hdlr()
{
    	u8 key,timerout_cnt=0;	

	station_save_pos=0;
	Disp_Con(DISP_SAVE_POS);

	while (1)
    	{
		key = get_msg();
		switch(key)
		{
		     	case INFO_MODE| KEY_HOLD:
				timerout_cnt=0;	
				break;
        		case INFO_NEXT_FIL | KEY_SHORT_UP:

				station_save_pos++;
				if(station_save_pos> Current_Band.MAX_CH)
					station_save_pos=0;
				
	                     Disp_Con(DISP_SAVE_POS);
				timerout_cnt=0;	
				break;
        		case INFO_PREV_FIL | KEY_SHORT_UP:
				station_save_pos--;
				if(station_save_pos>Current_Band.MAX_CH)
					station_save_pos=Current_Band.MAX_CH;
				
	                     Disp_Con(DISP_SAVE_POS);
				timerout_cnt=0;				
				break;

        		//case INFO_STOP| KEY_SHORT_UP:
        		case INFO_MODE| KEY_SHORT_UP:

				if(cur_sw_fm_band==0){
					save_radio_freq(frequency,station_save_pos*2+FM_CH_OFFSET);
				}
				else if(cur_sw_fm_band==1){
					save_radio_freq(frequency,station_save_pos*2+AM_CH_OFFSET);
				}
	                    Disp_Con(DISP_FREQ);
			      return;
	
		       case INFO_HALF_SECOND:

				timerout_cnt++;
				if(timerout_cnt==4){
		                    Disp_Con(DISP_FREQ);
				      return;
				}
				break;	
		}
	}
}
#define MANUAL_SEL_STATION_KEY    	(INFO_STOP| KEY_SHORT_UP)
void restore_station_from_ch()
{
	station_sel_pos++;
	if(station_sel_pos> Current_Band.MAX_CH)
		station_sel_pos=0;

	if(cur_sw_fm_band==0){
		frequency = read_radio_freq(station_sel_pos*2+FM_CH_OFFSET);
	}
	else if(cur_sw_fm_band==1){
		frequency = read_radio_freq(station_sel_pos*2+AM_CH_OFFSET);
	}
#ifdef UART_ENABLE
	printf("------->-station form  TABLE  fre:%4u   \r\n",frequency);
#endif

	set_radio_freq(FM_CUR_FRE,NO_SHOW_FREQ);
       Disp_Con(DISP_SEL_POS);
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

    	set_radio_freq(FM_CUR_FRE,SHOW_FREQ);
#ifdef ADKEY_SELECT_MODE
    	mode_switch_protect_bit=0;
#endif	
	Mute_Ext_PA(UNMUTE);

    while (1)
    {

	dac_out_select(DAC_AMUX1);

	key = get_msg();
#if 0
	if(key!= 0xff)
	    	printf("------->-get_msg   %x \r\n",(u16)key);
#endif
	if(dac_cnt >20){
		dac_sw(0);
	}
        switch (key)
        {

        case INFO_NEXT_SYS_MODE:
		return;
        case INFO_NEXT_FM_MODE:
		radio_band_hdlr();			
		break;
#ifdef FM_SAVE_STATION_MANUAL
     	case MANUAL_SEL_STATION_KEY:
		restore_station_from_ch();
		break;
     	case MANUAL_STATION_SAVE_KEY:
		radio_save_station_hdlr();
		break;
#endif		
	case BAND_FULL_SCAN_KEY:
		full_band_scan_hdlr();
		break;
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
             	set_radio_freq(FM_FRE_INC,SHOW_FREQ);
		break;
        case INFO_FRE_DOWN | KEY_SHORT_UP:
        case INFO_PREV_FIL | KEY_SHORT_UP:		
             	set_radio_freq(FM_FRE_DEC,SHOW_FREQ);
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

#ifdef RADIO_ST_INDICATOR
	 	KT_Radio_ST_Check();
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
				    	set_radio_freq(FM_CUR_FRE,SHOW_FREQ);
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
    //case MSG_USB_DISK_IN  :
    //case MSG_SDMMC_IN :
    //		break;
	
#ifdef USE_RTC_ALARM_FUNCTION
    		case INFO_ALM_BELL:
		    	sys_clock_pll();							
			alm_bell_mode();
             		set_radio_freq(FM_FRE_INC,SHOW_FREQ);			
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

void fm_osc_output_select(bool flag)
{
   // P05 OUT CLK for FM module
   if(flag){
#if defined(SYS_CRYSTAL_USE_12M)
	   P05_source_select(1);
#else
	   P05_source_select(2);
#endif
	   P0PU |=(1<<5);
	   P0DIR &=~(1<<5);
	   _nop_();
   	   _nop_();
   }
   else{
   	P05_source_select(0);
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

#ifndef DISABLE_P05_OSC_OUTPUT
   	fm_osc_output_select(TRUE);
#endif
       if (KT_AMFMWakeUp()==0)
       {
    		Disp_Con(DISP_ERROR);
		delay_10ms(50);
	}
	//else
	{

#ifdef GPIO_SEL_BAND_INFO_CONFIG
		scan_gpio_band_info_config();
		radio_preset_init();
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
		if(cur_sw_fm_band>MW_MODE)cur_sw_fm_band=FM_MODE;
#endif

#ifdef AM_RADIO_FUNC	
		if(work_mode == SYS_AMREV){
			cur_sw_fm_band = MW_MODE;
		}
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

#ifndef DISABLE_P05_OSC_OUTPUT
    		fm_osc_output_select(FALSE);
#endif

#ifdef RADIO_ST_INDICATOR
	     radio_st_ind=0;
#endif

	}
	
}
#endif

