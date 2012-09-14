/*--------------------------------------------------------------------------*/
/**@file   key.c
   @brief  按键消息扫描模块
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#include "Custom_config.h"

#include "key.h"
#include "clock.h"
#include "my_IR_key.h"
#include "main.h"
#include "KT_AMFMdrv.h"


extern void putbyte(u8);
extern void P0IE_ctl(u8, u8);
extern u8 return_cnt(u8, u8);
extern xd_u8 bP0IE;
extern xd_u8 work_mode;

bool key_voice_disable;
u8 keyBack;  ///<上一次按键扫描结果
u8 keyCnt;   ///<连续有效的按键扫描计数
u16 _idata user_code;   ///<红外遥控提取的用户码
u8 _idata irStep;      ///<IR当前状态
u16 _idata irBuffer;   ///<IR读取读取出的数据
u8 power_down_cnt;
u16 _idata dac_cnt;
xd_u8 LDO_IN_Volt=0;
xd_u8 adc_vdd12;
xd_u8 adc_vddio;
xd_u8 adc_vdd12l;
xd_u8 adc_vddiol;
u8 key_value;
bool sys_pwr_flag=0;

xd_u8 fm_sw_volt=0,sys_mod_volt=0,cur_work_mod=0,sys_func_sel=0;
extern xd_u8 sw_fm_mod,cur_sw_fm_band,curr_menu;
extern bool sys_pwr_flag,sys_mute_flag;
extern xd_u8 sw_fm_pos;

extern bool aux_online;

#ifdef ALARM_USE_MULTI_SOURCE_FUNC
bool alarm_power_on_protect=0;
#endif
#if 1
extern _code MY_IR_KEY_STRUCT My_IRTab[];       /*user code is 0xFF*/
extern u8 get_my_IR_key_MAX();
static xd_u8 MAX_IR_KEY = 0;
bool IR_key_det=0;
#endif

/*------------
----------------------------------------------------------------*/
/**@brief  LDO IN 电量检测
   @param  无
   @return 无
   @note   u8 ldoin_voltage(void)
*/
/*----------------------------------------------------------------------------*/
u8 ldoin_voltage(void)
{
    return (((((u16)adc_vddio*248)+5)/10)/adc_vdd12);
}
#if defined(USE_BAT_MANAGEMENT)
#include "lcdsegment.h"

xd_u8 batt_level=0;
#ifdef USE_GPIO_MEASURE_VOLT
#define BAT_LOW_VOLT   	0X7D/*LDOIN=3.3V*/ //(1D/*LDOIN=3.5V*/) (43/*LDOIN=3.7V*/) (45/*LDOIN=3.8V*/) (46/*LDOIN=3.9V*/) (47/*LDOIN=4.0V*/) (48/*LDOIN=4.1V*/) (49/*LDOIN=4.1V*/) 
#define BAT_HALF_VOLT   	0x8D
#define BAT_FULL_VOLT   	0xAB
#define BAT_CHARGE_FULL	0xE8
#else
#define BAT_LOW_VOLT   	37/*LDOIN=3.3V*/ //(1D/*LDOIN=3.5V*/) (43/*LDOIN=3.7V*/) (45/*LDOIN=3.8V*/) (46/*LDOIN=3.9V*/) (47/*LDOIN=4.0V*/) (48/*LDOIN=4.1V*/) (49/*LDOIN=4.1V*/) 
#define BAT_FULL_VOLT   	49
#define BAT_HALF_VOLT   	42
#define BAT_CHARGE_FULL	(BAT_FULL_VOLT+2)
#endif


#if defined(CHARGER_DETECT_INDICATOR)

#ifdef DC_CHARGER_DET_SHARE_IIC
#define CHARGER_DET_PORT	P01
#elif defined(DC_CHARGER_DET_USE_P10)
#define CHARGER_DET_PORT	P10
#else
#define CHARGER_DET_PORT	P03
#endif

//bool charger_in_flag=0;
bool charger_detect(void)
{
    //static bool charger_det=0;
#ifdef DC_CHARGER_DET_SHARE_IIC

    	CHARGER_DET_PORT =1;
	P0PD &=~(BIT(1));
	P0DIR |= (BIT(1));

	if(!CHARGER_DET_PORT){
#elif defined(DC_CHARGER_DET_USE_P10)

    	CHARGER_DET_PORT =1;
	P1PDA &=~(BIT(0));
    	//P1PU  |= (BIT(0));
	P1DIR |= (BIT(0));

	if(!CHARGER_DET_PORT){

#else

    	CHARGER_DET_PORT =0;

    	P0PU  &= ~(BIT(3));
    	P0PD|= (BIT(3));
    	P0DIR |= (BIT(3));

     if(CHARGER_DET_PORT){

#endif

#if 1
			if(LDO_IN_Volt>=BAT_CHARGE_FULL){
				
				batt_level = 3;
			}
			else{				
				if(batt_level++>3)
					batt_level=0;
			}
#endif		

#ifdef DC_CHARGER_DET_SHARE_IIC
		P0DIR &=~ (BIT(1));
#elif defined(DC_CHARGER_DET_USE_P10)
		P1DIR &=~ (BIT(0));
#endif

		return 1;
     }
//     charger_in_flag =0;
#ifdef DC_CHARGER_DET_SHARE_IIC
		P0DIR &=~ (BIT(1));
#elif defined(DC_CHARGER_DET_USE_P10)
		P1DIR &=~ (BIT(0));
#endif
     return 0;
}
#endif
void Bat_icon_chk(void)
{
#ifndef RTC_DISP_IN_IDLE_MODE
	if(work_mode==SYS_IDLE)return;
#endif
	disp_clr_icon(ICON_MUTE);

	if(sys_mute_flag){
		disp_icon(ICON_MUTE);
	}

	disp_icon(ICON_BAT);
	
	disp_icon(ICON_BAT_S1);
	disp_icon(ICON_BAT_S2);
	disp_icon(ICON_BAT_S3);
	disp_icon(ICON_BAT_S4);

	switch(batt_level)
	{
		case 0:
			disp_clr_icon(ICON_BAT_S1);
			disp_clr_icon(ICON_BAT_S2);
			disp_clr_icon(ICON_BAT_S3);
			break;
		case 1:
			disp_clr_icon(ICON_BAT_S1);
			disp_clr_icon(ICON_BAT_S2);
	
			break;
		case 2:
			disp_clr_icon(ICON_BAT_S1);
			break;
		case 3:
			break;			
	}

}
void bmt_hdlr(void)
{
#ifdef USE_GPIO_MEASURE_VOLT
	LDO_IN_Volt=adc_vddio;
	//printf_u16(adc_vddio,'V');
#else	 
	LDO_IN_Volt=ldoin_voltage();
	//printf_u16(LDO_IN_Volt,'L');
#endif
	
#if 1//efined(CHARGER_DETECT_INDICATOR)
	if(charger_detect()){

		return;
	}
#endif

	if(LDO_IN_Volt>BAT_FULL_VOLT){
		batt_level = 3;
	}
	else if(LDO_IN_Volt>BAT_HALF_VOLT){
		batt_level = 2;
	}
	else if(LDO_IN_Volt>BAT_LOW_VOLT){
		batt_level = 1;
	}
	else{
		batt_level = 0;
	}
	
	Bat_icon_chk();

}

#endif	

/*----------------------------------------------------------------------------*/
/**@brief  按键遥控初始化keyInit
   @param  无
   @return 无
   @note   void keyInit(void)
*/
/*----------------------------------------------------------------------------*/
void keyInit(void)
{
    ADCCON = 0xf8;
    adc_clock();
    P0PD &= ~(BIT(7));
    //P0IE = ~(BIT(7));
    P0DIR |= BIT(7);
	
    P0PD &= ~(BIT(6));
    //P0IE = ~(BIT(6));
    P0DIR |= BIT(6);
	
    P0PD &= ~(BIT(5));
    //P0IE = ~(BIT(5));
    P0DIR |= BIT(5);	
	
    key_value = 0xff;
    //P3PU  &= ~(1<<4);
    //P3DIR |= (1<<4);
    MAX_IR_KEY = get_my_IR_key_MAX();

}
/*----------------------------------------------------------------------------*/
/**@brief  ADC循环扫描程序 2MS被调用一次,分别扫描VDD1.2
   @param  无
   @return 无
   @note   void adc_scan(void)
*/
/*----------------------------------------------------------------------------*/
void adc_scan(void)
{
    static u8 cnt;

    if (cnt == 0)
    {
        adc_vdd12 = ADCDATH;//
        adc_vdd12l = ADCDATL;
#ifdef USE_GPIO_MEASURE_VOLT
    	 P0PD &= ~(BIT(3));
	 P0DIR |= BIT(3);
        ADCCON = ADC_KEY_IO3; //
        P0IE = ~(BIT(3));	 
#else
        ADCCON = ADC_LDOIN;
#endif
    }
    else if (cnt == 1)
    {
        adc_vddio = ADCDATH;//
        adc_vddiol = ADCDATL;//
#if defined(ADKEY_PORT_P06)
    	 P0PD &= ~(BIT(6));
    	 P0DIR |= BIT(6);
        ADCCON = ADC_KEY_IO6; //
        P0IE = ~(BIT(6));
#elif defined(ADKEY_PORT_P02)
    	 P0PD &= ~(BIT(2));
    	 P0DIR |= BIT(2);
        ADCCON = ADC_KEY_IO2; //
        P0IE = ~(BIT(2));
#else
	 P0DIR |= BIT(7);
    	 P0PD &= ~(BIT(7));
        ADCCON = ADC_KEY_IO7; //
        P0IE = ~(BIT(7));
#endif		
    }
#ifdef ADKEY_SELECT_MODE
    else if (cnt == 2)
    {
        key_value = ADCDATH;
        ADCCON = ADC_KEY_IO6; //
    	 P0PD &= ~(BIT(6));
    	 P0DIR |= BIT(6);
	 P0IE = ~(BIT(6));        
    }
    else if (cnt == 3)
    {
        sys_mod_volt = ADCDATH;
    
        ADCCON = ADC_KEY_IO5; //
        P0PD &= ~(BIT(5));
    	 P0DIR |= BIT(5);
     	 P0IE = ~(BIT(5));        
    }	
    else if (cnt == 4)
    {
        fm_sw_volt = ADCDATH;
        ADCCON = ADC_VDD_12;
    }		
#else
    else if (cnt == 2)
    {
        key_value = ADCDATH;
        ADCCON = ADC_VDD_12;
    }		
#endif
    else
    {
        cnt = 0;
    }
    adc_clock();
    cnt++;
    if (cnt >= ADC_MAX_USED)
        cnt = 0;
}

#ifdef JOG_STICK_FUNC
xd_u8 touchkeyval;
xd_u8 keyval_buf;
xd_u8  JogBuf;
#ifdef FAST_STICK_TUNE_FUNC
xd_u8 fast_step_cnt=0,reset_cnt=0,last_reg=0;
#endif
void JogDetect(void)
{
	u8 port_val=0;

#if defined(JOG_STICK_PORT_P02_P03)	

    P0DIR &= ~(BIT(3)|(BIT(2)));    //P11, P12
    P0 |=(BIT(3)|(BIT(2)));
    P0PU	|= (BIT(3)|(BIT(2)));
    P0DIR |= (BIT(2)|(BIT(3)));    //P11, P12
    _nop_();
	
    touchkeyval =((P0&(BIT(2)|(BIT(3))))>>2);
#elif defined(JOG_STICK_PORT_P24_P25)
    //P2IE |=(BIT(5)|(BIT(4)));
    DACCON0|=0x40;
    P2DIR &= ~(BIT(5)|(BIT(4)));    //P11, P12
    P2 |=(BIT(5)|(BIT(4)));
    P2PU	|= (BIT(5)|(BIT(4)));
    P2DIR |= (BIT(5)|(BIT(4)));    //P11, P12
    _nop_();
	
    touchkeyval =((P2&(BIT(4)|(BIT(5))))>>4);
    //port_val =(P2&(BIT(4)|(BIT(5))));
    //touchkeyval = ((port_val & (BIT(4)))>>4)|((port_val & (BIT(5)))>>4);

#else//elif defined(K000_LYXD_00000_REC_V001)
    P0DIR &= ~(BIT(4)|(BIT(3)));    //P11, P12
    P0 |=(BIT(4)|(BIT(3)));
    P0PU	|= (BIT(4)|(BIT(3)));
    P0DIR |= (BIT(4)|(BIT(3)));    //P11, P12
    _nop_();
    port_val =(P0&(BIT(4)|(BIT(3))));
    touchkeyval = ((port_val & (BIT(4)))>>4)|((port_val & (BIT(3)))>>2);
#endif

#ifdef FAST_STICK_TUNE_FUNC
	if(last_reg!=touchkeyval){
		last_reg=touchkeyval;
	}
	else{

		if(fast_step_cnt>0){

			reset_cnt++;
			if(reset_cnt>=12){
				fast_step_cnt=0;
			}
		}
	}
#endif

	if((touchkeyval == 0x00) || (touchkeyval == 0x03))
    	{										
	        if(touchkeyval == keyval_buf)
	        {
	            JogBuf = 0;
	        }
	        else
	        {
	            keyval_buf = touchkeyval;
	            if (touchkeyval == 0x00)
	            {
	                if (JogBuf == 0x02)
	                {
	                    // Increase
#ifdef FAST_STICK_TUNE_FUNC
	                    	reset_cnt=0;
				if(fast_step_cnt<6)
				fast_step_cnt++;
#endif			
		             put_msg_fifo(INFO_FRE_UP | KEY_SHORT_UP);
	                    JogBuf = 0;
	                }
	                if (JogBuf == 0x01)
	                {
	                    // Decrease
#ifdef FAST_STICK_TUNE_FUNC                   
	                     reset_cnt=0;
				if(fast_step_cnt<6)
				fast_step_cnt++;            
#endif			
		             put_msg_fifo(INFO_FRE_DOWN|KEY_SHORT_UP);
	                    JogBuf = 0;
	                }
	            }
	            else
	            {
#ifdef FAST_STICK_TUNE_FUNC
	                 reset_cnt=0;
#endif
	                if (JogBuf == 0x02)
	                {
		             //put_msg_fifo(INFO_PLUS | KEY_SHORT_UP);
	                    JogBuf = 0;
	                }
	                if (JogBuf == 0x01)
	                {			
	                   // Increase 
		            // put_msg_fifo(INFO_MINUS|KEY_SHORT_UP);
	                    JogBuf = 0;
	                }					
	            }
	        }
    	}
	else
	{
		JogBuf = touchkeyval;
    	}
}

#endif

#ifdef ADKEY_SELECT_MODE
bool mode_switch_protect_bit=0;
void ad_mod_sel_hdlr()
{
#ifdef USE_LINE_IN_DETECT_FUNC
	if(aux_online)return;
#endif

#ifdef USE_ADVOLT_FOR_FUNC_SEL_TYPE_TWO		

//4 P06 AD PORT FOR MODE SEL
	if(sys_mod_volt<ADKEY2_RES_6){	

		cur_work_mod =SYS_IDLE;
	}
	else{
		
#ifdef ALARM_USE_MULTI_SOURCE_FUNC
		alarm_power_on_protect=0;
#endif
		cur_work_mod =0xFF;
	}

	if(!mode_switch_protect_bit){

#ifdef ALARM_USE_MULTI_SOURCE_FUNC
		if(!alarm_power_on_protect)
#endif
		{
			if(sys_pwr_flag==0){

				if(curr_menu == DISP_SCAN_DISK)return;
				
				if(cur_work_mod == SYS_IDLE){
					
					if(work_mode !=cur_work_mod){
						set_brightness_all_on();
						work_mode = SYS_IDLE;
						put_msg_fifo(INFO_NEXT_SYS_MODE);				
					}
				}
			}
			else{

				if(work_mode==SYS_IDLE){

					if(work_mode ==cur_work_mod){
						sys_pwr_flag =0;
					}
				}
			}
		}
	}
#elif defined(USE_ADVOLT_FOR_FUNC_SEL_TYPE_FOUR)
//4 P06 AD PORT FOR MODE SEL
	if(sys_mod_volt<ADKEY2_RES_6){	

		cur_work_mod =SYS_IDLE;
	}
	else{
		
#ifdef ALARM_USE_MULTI_SOURCE_FUNC
		alarm_power_on_protect=0;
#endif
		cur_work_mod =0xFF;
	}

	if(!mode_switch_protect_bit){

#ifdef ALARM_USE_MULTI_SOURCE_FUNC
		if(!alarm_power_on_protect)
#endif
		{
			if(sys_pwr_flag==0){

				if(curr_menu == DISP_SCAN_DISK)return;
				
				if(cur_work_mod == SYS_IDLE){
					
					if(work_mode !=cur_work_mod){
						set_brightness_all_on();
						work_mode = SYS_IDLE;
						put_msg_fifo(INFO_NEXT_SYS_MODE);				
					}
				}
			}
			else{

				if(work_mode==SYS_IDLE){

					if(work_mode ==cur_work_mod){
						sys_pwr_flag =0;
					}
				}
			}
		}
	}
#elif defined(USE_ADVOLT_FOR_FUNC_MODE_SEL)
//4 P06 AD PORT FOR MODE SEL

	if(sys_mod_volt>ADKEY2_RES_NOKEY){
			
	      	   cur_work_mod =SYS_MP3DECODE;
	}
	else if(sys_mod_volt>ADKEY2_RES_2){	
	   	  cur_work_mod =SYS_FMREV;
	}
	else {
		cur_work_mod =SYS_IDLE;
	}

	if(!mode_switch_protect_bit){

		if(sys_pwr_flag==0){

			if(curr_menu == DISP_SCAN_DISK)return;
			
			if(work_mode !=cur_work_mod){
				set_brightness_all_on();
				work_mode = cur_work_mod;
				put_msg_fifo(INFO_NEXT_SYS_MODE);				
			}
		}
		else{

			if(cur_work_mod==SYS_IDLE){

				if(work_mode ==cur_work_mod){
					sys_pwr_flag =0;
				}
			}
		}
	}
#endif


#ifdef USE_ADVOLT_FOR_FUNC_SEL_TYPE_TWO	
//4 P05 AD PORT FOR FUNC SEL

	 	if(fm_sw_volt>ADKEY2_RES_6)
	      	   	sys_func_sel =SYS_MP3DECODE ;
	  	else 
	      	   	sys_func_sel =SYS_FMREV ;

		if(sys_pwr_flag==0){

			if(!mode_switch_protect_bit){

				if((cur_work_mod !=SYS_IDLE)
#ifdef ALARM_USE_MULTI_SOURCE_FUNC
					||(alarm_power_on_protect==1)
				)
#endif				
				{

					if(curr_menu == DISP_SCAN_DISK)return;

					if(work_mode !=sys_func_sel){
						set_brightness_all_on();
						work_mode = sys_func_sel;
						put_msg_fifo(INFO_NEXT_SYS_MODE);				
					}
				}
			}
		}
#elif defined(USE_ADVOLT_FOR_FUNC_SEL_TYPE_FOUR)
//4 P05 AD PORT FOR FUNC SEL

	 	if(fm_sw_volt>ADKEY2_RES_8)
	      	   	sys_func_sel =SYS_MP3DECODE ;
	  	else{
			
	      	   	sys_func_sel =SYS_FMREV ;
				
		 	if(fm_sw_volt>ADKEY2_RES_6){
	      	   		sw_fm_pos =FM_MODE ;		
		 	}
			else if(fm_sw_volt>ADKEY2_RES_4){
	      	   		sw_fm_pos =MW_MODE ;
			}
			else{
	      	   		sw_fm_pos =SW_MODE ;
									
			}

			if(sys_pwr_flag==0){

				if(!mode_switch_protect_bit){

					if(work_mode == SYS_FMREV){
						if(sw_fm_pos !=sw_fm_mod){
							cur_sw_fm_band =sw_fm_pos;
							sw_fm_mod=sw_fm_pos;
							put_msg_fifo(INFO_BAND_SEL);
						}
					}
				}
			}
		}

		if(sys_pwr_flag==0){

			if(!mode_switch_protect_bit){

				if((cur_work_mod !=SYS_IDLE)
#ifdef ALARM_USE_MULTI_SOURCE_FUNC
					||(alarm_power_on_protect==1)
				)
#endif				
				{

					if(curr_menu == DISP_SCAN_DISK)return;

					if(work_mode !=sys_func_sel){
						set_brightness_all_on();
						work_mode = sys_func_sel;
						put_msg_fifo(INFO_NEXT_SYS_MODE);				
					}
				}
			}
		}
#elif defined(USE_ADVOLT_FOR_RADIO_BAND_SEL)
	if(work_mode==SYS_FMREV){

#if 0
        for (i = 0; i < 9; i++)
            if (fm_sw_volt > adkey1_check_table[i])
                break;
#else			
	  	//if(fm_sw_volt>ADKEY2_RES_NOKEY)
	      	//   	sw_fm_mod =SW9_MODE ;
	     	 if(fm_sw_volt>ADKEY2_RES_10)
	      	   	sw_fm_mod =SW8_MODE ;
	    	else if(fm_sw_volt>ADKEY2_RES_9)
	      	   	sw_fm_mod =SW7_MODE ;
	  	else if(fm_sw_volt>ADKEY2_RES_8)
	      	   	sw_fm_mod =SW6_MODE ;
	 	else if(fm_sw_volt>ADKEY2_RES_7)
	      	   	sw_fm_mod =SW5_MODE ;
	 	else if(fm_sw_volt>ADKEY2_RES_6)
	      	   	sw_fm_mod =SW4_MODE ;
	 	else if(fm_sw_volt>ADKEY2_RES_5)
	      	   	sw_fm_mod =SW3_MODE ;
	 	else if(fm_sw_volt>ADKEY2_RES_4)
	      	   	sw_fm_mod =SW2_MODE ;
	 	else if(fm_sw_volt>ADKEY2_RES_3)
	      	   	sw_fm_mod =SW1_MODE ;
	 	else if(fm_sw_volt>ADKEY2_RES_2)
	      	   	sw_fm_mod =SW_MODE ;
	 	else if(fm_sw_volt>ADKEY2_RES_1)
	      	   	sw_fm_mod =MW_MODE ;
	  	else 
	      	   	sw_fm_mod =FM_MODE ;

		if(cur_sw_fm_band !=sw_fm_mod){
			cur_sw_fm_band=sw_fm_mod;
#if 0
			deg_str(" \r\n");
			printf_u16(fm_sw_volt,'V');
			printf_u16(cur_sw_fm_band,'F');
			printf_u16(sw_fm_mod,'F');
			deg_str(" \r\n");
		    deg_str("---->MSG_CHANGE_FM_MODE \n");
#endif		
				set_brightness_all_on();
				put_msg_fifo(INFO_NEXT_FM_MODE);
		}
#endif

	}
#endif		
	
}
u8 get_cur_select_func()
{
	return sys_func_sel;
}
#endif
/*----------------------------------------------------------------------------*/
/**@brief  读取键值keyDetect
   @param  无
   @return 无
   @note   u8 keyDetect(void)
*/
/*----------------------------------------------------------------------------*/

/* for get ir user code
u16 debug_ir;
*/
u8 keyDetect(void)
{
    u8 keyTemp;
    u8 key_index=0;
	
    keyTemp = NO_KEY;
    if (irStep == 32)
    {
        if (0xff00 == user_code)
        {
		for(key_index = 0;key_index <MAX_IR_KEY;key_index++){
	            if((My_IRTab[key_index].IR_Key_Value)==(irBuffer & 0xff)){
				keyTemp = (My_IRTab[key_index].APP_Key);
				
				if(My_IRTab[key_index].APP_Key ==INFO_PLAY){
					IR_key_det=1;
				}
			        //printf_u16(keyTemp,'R');
				break;
		     }
		}     
        }
        else if (0xfd02 == user_code)
        {
		for(key_index = 0;key_index <MAX_IR_KEY;key_index++){
	            if((My_IRTab[key_index].IR_Key_Value)==(irBuffer & 0xff)){
				keyTemp = (My_IRTab[key_index].APP_Key);
			        //printf_u16(keyTemp,'R');
				break;
		     }
		} 
	}
    }
    return keyTemp;
}
/*----------------------------------------------------------------------------*/
/**@brief  power按键函数
   @param  无
   @return 无
   @note   u8 key_power(void)
*/
/*----------------------------------------------------------------------------*/
u8 key_power(void)
{
//    if (P34)
//    {
//        return INFO_POWER;
//    }
    return NO_KEY;
}
/*----------------------------------------------------------------------------*/
/**@brief  按键音函数
   @param  无
   @return 无
   @note   void key_tone(void)
*/
/*----------------------------------------------------------------------------*/
void key_tone(void)
{
	set_brightness_all_on();
	return;
#if 0	
    	power_down_cnt = 0;
	dac_cnt = 0;
    if (key_voice_disable)
    {
        return ;
    }
//    sys_clock_pll();	//(MAIN_CLK_PLL);
	dac_sw(1);			//不降频的用户需注掉
    write_dsp(-6, 5, 0x10);
#endif	
}
/*----------------------------------------------------------------------------*/
/**@brief  处理扫描到的按键，长按、短按、HOLD按键状态等在这里进行 ；按键音也在这里处理
   @param  无
   @return 无
   @note void keyScan(void)
   @note 开发板ADKEY结果：
   @note VOL-(0v): 0
   @note VOL+(0.43v): 0x21
   @note PREV(0.82v): 0x3F
   @note NEXT(1.237v): 0x5F
   @note PLAY(1.616v): 0x7C
   @note FM/MP3(2.05v): 0x9E
   @note EQ(2.50v): 0xC1
   @note PlayMode(2.93v): 0xE2
*/
/*----------------------------------------------------------------------------*/
void keyScan(void);

