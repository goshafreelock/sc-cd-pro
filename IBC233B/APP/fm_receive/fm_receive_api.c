/*--------------------------------------------------------------------------*/
/**@file     fm_receive_api.c
   @brief    FM任务应用接口
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/


#include "fm_api.h"
#include "KT_AMFMdrv.h"
#include "RDA_FM_drv.h"


extern _xdata u8 fre_preset[];
extern bit key_voice_disable;
extern u8 device_active;                           ///< 当前活动的设备
extern xd_u8 all_channl;                              ///< FM收音当前搜索到的台总数
extern xd_u16 frequency;                              ///< 当前频率
extern xd_u8 fre_channl;                              ///< FM收音当前所在的台号
extern xd_u8 given_device;
extern _xdata SYS_WORK_MODE  work_mode;
extern xd_u8 cur_sw_fm_band;
extern xd_u16 REG_MAX_FREQ,REG_MIN_FREQ;
extern Str_Band  Current_Band;
extern xd_u8 my_music_vol;

extern void KT_AMFMStandby(void);
extern xd_u8 KT_AMFMWakeUp(void);
extern void KT_AMFMSetMode(xd_u8 AMFM_MODE);
extern xd_u8 KT_AMTune(xd_u16 Frequency);
extern xd_u8 KT_FMTune(xd_u16 Frequency);
extern xd_u8 KT_FMValidStation(xd_u16 Frequency);
extern xd_u8 KT_AMValidStation(xd_u16 Frequency);
extern xd_u8 KT_SMValidStation(xd_u16 Frequency);
extern void KT_Mute_Ctrl(bool m_f);

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



xd_u16 radio_chip_id=0x0000;
u16 radio_chip_get_id()
{
#ifdef USE_KT_FM_CHIP
	radio_chip_id = KT_AMFM_Read_ID();
	if(radio_chip_id== "KT"){
		return radio_chip_id;
	}
#endif

#ifdef USE_RDAs_FM
	radio_chip_id= RDA5807P_Get_ID();
	if((radio_chip_id  == RDAFM_ID1)||(radio_chip_id  == RDAFM_ID2)||(radio_chip_id  == RDAFM_ID3)||(radio_chip_id  == RDAFM_ID4)||(radio_chip_id  == RDAFM_ID5)){
		radio_chip_id  = RDAFM_ID;
		return radio_chip_id;
	}
#endif
	return 0x00;
}
void radio_chip_init()
{
#ifdef USE_KT_FM_CHIP
	if(radio_chip_id== "KT"){

		KT_AMFMWakeUp();
	}
#endif

	if(radio_chip_id== RDAFM_ID){

    		RDA5807P_Intialization();

	}		
}
void radio_chip_standby()
{
#ifdef USE_KT_FM_CHIP
	if(radio_chip_id== "KT"){

		KT_AMFMStandby();
	}
#endif
	if(radio_chip_id== RDAFM_ID){

		RDA5807P_PowerOffProc();

	}	
}
void radio_set_band_info()
{
#ifdef USE_KT_FM_CHIP

	if(radio_chip_id== "KT"){

		load_band_info();
	}
#endif
	if(radio_chip_id== RDAFM_ID){

		cur_sw_fm_band=0;
		Current_Band.Band=0;
		Current_Band.MAX_CH=FM_MAX_CH-1;
		Current_Band.Tune_Step=10;
		Current_Band.Seek_Step = 10;	
		Current_Band.ValidStation_Step =10 ;		
	}
}
void radio_switch_band_mode(u8 b_mode)
{
#ifdef USE_KT_FM_CHIP

	if(radio_chip_id== "KT"){

		KT_AMFMSetMode(b_mode);	
	}
#endif
	if(radio_chip_id== RDAFM_ID){

		cur_sw_fm_band=0;
	}

}
static void set_freq(u16 freq_reg)
{
#ifdef USE_KT_FM_CHIP

	if(radio_chip_id== "KT"){

	    if(cur_sw_fm_band==0){

			KT_FMTune(frequency);
	    }
	    else{

			KT_AMTune(frequency);
	    }	
	}
#endif
	if(radio_chip_id== RDAFM_ID){

		RDA5807P_ValidStop(freq_reg/10);
		RDA5807P_SetMute(FALSE);		
	}
}
void radio_chip_set_freq(u8 mode,bool disp_pro)
{
    xd_u8 freq_step =0;

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
	
    set_freq(frequency);
	
    if(disp_pro)	
	Disp_Con(DISP_FREQ);			

#if 1//def SAVE_BAND_FREQ_INFO
#ifdef FM_UART_ENABLE
    printf("------->- set_radio_freq    fre:%4u   \r\n",frequency);
#endif
    save_radio_freq(frequency,cur_sw_fm_band*2+MEM_FREQ_BASE);
#endif
    set_sys_vol(my_music_vol);	

}
bool radio_chip_valid_stop(u16 freq_reg)
{
	bool ret_var=0;
#ifdef USE_KT_FM_CHIP	
	if(radio_chip_id== "KT"){

	    if(cur_sw_fm_band==0){
		return KT_FMValidStation(freq_reg);
	    }
	    else if(cur_sw_fm_band==1){
		return KT_AMValidStation(freq_reg);
	    }
#ifdef MULTI_BAND_KT_0915_IN_USE	
	    else{
		return KT_SMValidStation(freq_reg);
	    }
#endif
		return 0;
	}
#endif

	if(radio_chip_id== RDAFM_ID){

		ret_var = RDA5807P_ValidStop(freq_reg/10);
		RDA5807P_SetMute(FALSE);		
		return ret_var;
	}

	return 0;
}
