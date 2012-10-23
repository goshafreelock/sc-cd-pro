/*--------------------------------------------------------------------------*/
/**@file     main.c
   @brief    主流程
   @details
   @author
   @date   2011-3-7
   @note   AC209N
*/
/*----------------------------------------------------------------------------*/
#include "Custom_config.h"
#include "main.h"
#include "device.h"
#include "led.h"
#include "lcd.h"
#include "lcdsegment.h"
#include "fm_rev.h"
#include "aux_fun.h"
#include "msgfor_usb.h"
#include "rtc_mode.h"
#include "decode_otp.h"
#include "rtc_cpu.h"
#include "fat_memory.h"
#include "uart.h"
#include "voice_time.h"
#include "mcu_master.h"

extern u8 _idata music_vol;
extern xd_u8 curr_menu;
extern bit pc_connect;
extern bit iic_busy;
extern u8 bdata device_online;
extern bool main_fade_en, digital_fade_en;     ///< 是否允许主音量和数字音量的淡入淡出，如果使能，模式切换和歌曲切换的时间会延长。默认不开启
extern u16 _idata dac_cnt;
xd_u8 work_mode;
u8 _xdata win_buffer[512] _at_ 0x600;
xd_u16 cfilenum;
bool aux_online=0;
xd_u8 bP0IE;
extern bool main_fade_en;			///<启用淡入淡出标志
extern volatile bool bObufMute;
extern bool main_vol_mute;
extern bool dac_automute;
extern xd_u8 play_status;
extern xd_u8 return_cnt;
extern xd_u8 curr_menu,disp_scenario;
volatile u8 bDACTimeOut;			//等待OBUF 清空timeout
extern xd_u8 LDO_IN_Volt;
extern bool sys_pwr_flag,sys_mute_flag;
#ifdef JOG_STICK_FUNC	 
extern void JogDetect(void);
#endif

#ifdef ADKEY_SELECT_MODE
extern bool mode_switch_protect_bit;
#endif
#ifdef ALARM_USE_MULTI_SOURCE_FUNC
extern bool alarm_power_on_protect;
#endif

#if ((USE_RTC_EEPROM == MEMORY_STYLE))
extern void check_eeprom_status(void);
#endif

#if defined(PWR_CTRL_WKUP)
extern void wkup_pin_ctrl(bool dir);
#endif
#ifdef USE_CD_MCU_MASTER_FUNC			
extern bool mcu_master_tranceive_tick;
#endif
extern void KT_AMFMStandby(void);

#ifdef ADKEY_SELECT_MODE
extern void ad_mod_sel_hdlr();
#endif

#ifdef RTC_DISP_IN_IDLE_MODE
void rtc_disp_hdlr(void);
extern u8 alm_bell_mode(void);

#endif


//#define ADKEY_DEBUG
#ifdef ADKEY_DEBUG
extern u8  key_value;   ///<红外遥控提取的用户码
void AD_Debug_func()
{
	while(1){
			
		printf("ad read value %x \r\n",(u16)((key_value)));
		//printf("ad read value %d \r\n",(u16)((key_value*33)/255));
		delay_10ms(2);
	}		
}
#endif
/*----------------------------------------------------------------------------*/
/**@brief 	P0IE初始化函数
   @param 	无
   @return  无
   @note 	void P0IE_init(void)
*/
/*----------------------------------------------------------------------------*/
void P0IE_init(void)
{
	bP0IE = 0xff;		 	//P0IE reset:0xff
}
/*----------------------------------------------------------------------------*/
/**@brief 	P0IE控制函数(由于P0IE寄存器不能读,不建议用户使用&=/|=这样的操作)
   @param 	sel:置位/清零；dat：写入P0IE的位
   @return  无
   @note 	void P0IE_ctl(u8 sel, u8 dat)
*/
/*----------------------------------------------------------------------------*/
void P0IE_ctl(u8 sel, u8 dat)
{
	if (sel)
	{
		bP0IE |= BIT(dat);
	}
	else
	{
		bP0IE &= ~(BIT(dat));
	}
	P0IE = bP0IE;		
}
#ifdef SYS_GPIO_SEL_FUNC
bool gpio_sel_func=0;
void gpio_sel_func_mode()
{

	GPIO_SEL_FUNC_GPIO_INIT();
	_nop_();
	
	if(GPIO_SEL_FUNC_GPIO_READ){

		if(work_mode!= HIGH_LEVEL_SEL_MODE){
			gpio_sel_func=1;
			Set_Curr_Func(HIGH_LEVEL_SEL_MODE);
      			put_msg_lifo(INFO_NEXT_SYS_MODE);	   
		}
	}
	else{

		if(work_mode> SYS_MP3DECODE_SD){
			gpio_sel_func=1;		
			
			if((get_device_online_status()&0x01)>0){

				Set_Curr_Func(SYS_MP3DECODE_SD);

			}
			else{

				Set_Curr_Func(LOW_LEVEL_SEL_MODE);

			}
      			put_msg_lifo(INFO_NEXT_SYS_MODE);	   
		}
	}
}
#endif

#ifdef USE_LINE_IN_DETECT_FUNC
/*----------------------------------------------------------------------------*/
/**@brief LINE IN 在线检测
   @param 无
   @return  无
   @note void aux_check(void)
*/
/*----------------------------------------------------------------------------*/
void aux_check(void)
{
    static u8 aux_cnt_online;

#ifdef AUX_DETECT_SHARE_WITH_IIC_SCL
    if (iic_busy)
    {
        return;
    }
#endif	

#if defined(INDEPENDENT_AUX_DETECT_GPIO)
	AUX_DETECT_GPIO=1;
#elif defined(AUX_DETECT_USE_VPP)

#elif defined(LINE_IN_DETECT_SHARE_LED_STATUS_PORT)
	if(AUX_DETECT_GPIO==0)return;
#else
	AUX_DETECT_GPIO=1;
#endif

    aux_detect_in();
	
    //DACCON0 |= 0x80;
    _nop_();
    _nop_();
    _nop_();

    if (AUX_DETECT_GPIO)
    {
        aux_cnt_online = 0;

        if (aux_online)
        {
            put_msg_lifo(MSG_AUX_OUT);
            aux_online = 0;
        }
    }
    else
    {
        aux_cnt_online++;
        if (aux_cnt_online > 20)
        {
            if (!aux_online)
            {
                put_msg_lifo(MSG_AUX_IN);
                aux_online = 1;
            }
        }
    }
    aux_detect_in_off(); //linein check port
}
#endif

#if SDMMC_GPIO_MODE
/*----------------------------------------------------------------------------*/
/**@brief   使用独立的GPIO检测SD card在线
   @param   void
   @return  void
   @note    void sdmmc_detect(void)
*/
/*----------------------------------------------------------------------------*/
void sdmmc_detect(void)
{
    	static u8 sdmmc_cnt_online;

	P3IE |= BIT(3);
	P3DIR |= BIT(3); //linein check port
    	P3PU |= BIT(3);
    	_nop_();
    	_nop_();
    	_nop_();
	_nop_();
	_nop_();
    	if (P33)
    	{
        	sdmmc_cnt_online = 0;
		sdmmc_chk_io(0);
    	}
    	else
    	{
       	if(sdmmc_cnt_online<51)
		{
		    sdmmc_cnt_online++;
        	}
		if (sdmmc_cnt_online > 50)
        	{
		    	sdmmc_chk_io(1);
        	}
		else
		{
		    sdmmc_chk_io(0xff);
		}
    	}
}
#endif
/*----------------------------------------------------------------------------*/
/**@brief  10MS中断
   @param  无
   @return 无
   @note   void timer0isr(void)
*/
/*----------------------------------------------------------------------------*/
void timer1isr(void)
{
    static u8 ms_cnt, counter;

    _push_(DPCON);
    _push_(DP1L);
    _push_(DP1H);
    DPCON = 0x0;
    T1CON &= ~(BIT(7));

    disp_scan();

#ifdef USE_USB_SD_DECODE_FUNC	       
	udisk_disconnect_check();
 	usb_diskin_detect();
#endif	
#ifdef JOG_STICK_FUNC	 
    	JogDetect();
#endif	

	counter++;
    if (counter == 5)
    {
        counter = 0;

		
        if(dac_cnt < 21){
		    dac_cnt++;
	}
	adc_scan();

#ifdef USE_LINE_IN_DETECT_FUNC        
        aux_check();
#endif        
        keyScan();
        dec_delay_counter();

#ifdef USE_USB_SD_DECODE_FUNC	       

#ifndef NO_SD_DECODE_FUNC
#if SDMMC_CMD_MODE
		sd_online_check();
#elif SDMMC_CLK_MODE
		sdmmc_online_chk();
#elif SDMMC_GPIO_MODE
	  	sdmmc_detect();
#endif
#endif	

#endif	
        ms_cnt++;

#ifdef USE_CD_MCU_MASTER_FUNC			
	 if(ms_cnt%10==0){
		 mcu_master_tranceive_tick=1;
	 }	 
#endif

        if (ms_cnt ==  50)
        {
            ms_cnt = 0;
            put_msg_fifo(INFO_HALF_SECOND);
            LDO_IN_Volt=ldoin_voltage();
#ifdef USE_RTC_ALARM_FUNCTION
            check_alm();
#endif
#ifdef SYS_GPIO_SEL_FUNC
	     gpio_sel_func_mode();
#endif
#ifdef ADKEY_SELECT_MODE
	    ad_mod_sel_hdlr();
#endif
        }
        adc_start();
    }

    _pop_(DP1H);
    _pop_(DP1L);
    _pop_(DPCON);
}

/*----------------------------------------------------------------------------*/
/**@brief   PLL初始化
   @param   无
   @return  无
   @note	void pll_init(void)
*/
/*----------------------------------------------------------------------------*/
void pll_init(void)
{
    P0PU = 0;
    PCON = 0;
    USBCON0 |= BIT(0);							//usb io is port
    P3PD |= 0xc0;
    P3PU &= ~0xC0;
    CLKGAT = 0;
    CLKCON = 0x01;
    DACCON1 |= BIT(6);                  //DAC高阻
#ifdef USE_POWER_KEY   
    sys_power_up();
#endif
    disp_init_if();						//在芯片上电后，马上显示初始界面

#if OSC_CLOCK == 24000000L
    PLL_input(PLL_IN_OTHER);
    PLL_output(PLL_OUT_DOUBLE);
    mainclock_select(MAIN_CLK_PLL);
#endif

#if OSC_CLOCK == 12000000L
    PLL_input(PLL_IN_12M);
    PLL_output(PLL_OUT_48M);
    mainclock_select(MAIN_CLK_PLL);
#endif

#if OSC_CLOCK == 7600000L
    PLL_input(PLL_IN_76M);
    PLL_output(PLL_OUT_48M);
    mainclock_select(MAIN_CLK_PLL);
#endif

#if OSC_CLOCK == 32768L
    PLL_input(PLL_IN_32K);
    PLL_output(PLL_OUT_48M);
    mainclock_select(MAIN_CLK_PLL);
#endif
}

/*----------------------------------------------------------------------------*/
/**@brief  timer1 初始化函数，产生5ms中断
   @param  无
   @return 无
   @note   void timer1Init(void)
*/
/*----------------------------------------------------------------------------*/
void timer1Init(void)
{
    	T1SCA = 0;//1/(2^n)分频
    	T1PRD = 64;   //约等于2MS
    	T1CNT = 0;
    	T1CON = 0x2d;//设置为TIMER模式，并将其相应的中断打开
	PT1 = 1;     //timer1 interrupt 低优先级
	ET1	= 1;

}
/*----------------------------------------------------------------------------*/
/**@brief   timer3 初始化函数
   @param	无
   @return	无
   @note	void timer3Init(void)
*/
/*----------------------------------------------------------------------------*/
void timer3Init(void)
{
    	WKUPPND |= (1<<6);  //开VPP的上拉
    	PT3 = 1;
    	T3SCA = 0x20;      // 1/(2^n)分频
    	T3CNTL = 0;
    	T3CNTH = 0;
    	T3PRD = 0x13;		//0xa1;
    	T3PRD = 0x88;		//0x22;
    	T3CON = 0x3e;       //设置为capture模式，并将其相应的中断打开
    	ET3 = 1;
}

/*----------------------------------------------------------------------------*/
/**@brief  系统初始化
   @param  无
   @return 无
   @note   void sys_init(void)
*/
/*----------------------------------------------------------------------------*/
void sys_init(void)
{
#ifdef UART_ENABLE
    uartInit();
    printf("power up \r\n");
 #endif

#if RTC_ENABLE
    if (init_rtc())
    {
        work_mode = SYS_RTC;
    }
#else 
	{
		u8 rtc_reg;
	    rtc_reg=read_rtc_reg();
		//printf("RTC reg:%02bx\n",rtc_reg);	
		if (rtc_reg & BIT(6))   ///<RTC是否完全掉电
	    {
#if (MEMORY_STYLE == USE_RTCRAM)
			reset_rtc_ram();
#endif
			rtc_reg &= ~(BIT(6)); 				 //清零PDGLG
    			write_rtc_reg(rtc_reg);
	    }
	}
#endif

    P0IE_init();
    keyInit();
    timer1Init();
    timer3Init();
#ifdef USE_USB_SD_DECODE_FUNC	    
#ifndef NO_SD_DECODE_FUNC
    sd_speed_init(1,100);
    init_port_sd();
#endif
#endif
#if SDMMC_CMD_MODE
	sd_chk_ctl(SET_SD_H_CHK);
#endif
    DACCON0 |= 0x05;	//打开DSP
    EA = 1;
}

/*----------------------------------------------------------------------------*/
/**@brief  系统共用信息的初始化
   @param  无
   @return 无
   @note   void sys_info_init(void)
*/
/*----------------------------------------------------------------------------*/
void sys_info_init(void)
{
    delay_10ms(80);            	      //保证U盘/SD卡能有足够时间通过在线检测,稳定接入
    dac_init(); 
#if DEVICE_ENCRYPTION
    device_password_init(0x12345678);  //输入加密设备的密码
#endif 
#if ((USE_DEVICE == MEMORY_STYLE)&&(FAT_MEMORY))    
	get_info_memory();
	get_info();
#endif  
#if ((USE_RTC_EEPROM == MEMORY_STYLE))
	check_eeprom_status();
#endif

#if 1
	music_vol = read_info(MEM_VOL);
    	if ((music_vol > MAX_MAIN_VOL) || (music_vol == 0))              //每次开机时，不要超过最大音量的一半，以免开机音量过大
    	{
        	music_vol = 10;
    	}
#endif

#if VOLUME_DEFAULT
     	music_vol = VOLUME_DEFAULT;
#endif
   	set_max_vol(MAX_ANALOG_VOL, MAX_DIGITAL_VOL);
   	main_vol_set(music_vol, CHANGE_VOL_MEM);
	main_fade_en = 1;
#if 0	
#if OTP_MUSIC_ENABLE
    	decode_opt_music_file();
#endif
#if RTC_ENABLE
    if (work_mode != SYS_RTC)
#endif
    {
            usb_audio_massstorage();									//每次上电判断是否连接电脑
	}
#endif

#if 0
#ifdef USE_SYS_MODE_RECOVER
        work_mode = read_info(MEM_SYSMODE);
#ifdef UART_ENABLE
    	printf("------->- MEM_SYSMODE restored   work_mode:%d   \r\n",(u16)work_mode);
#endif
        if (work_mode > SYS_IDLE){
#ifdef USE_USB_SD_DECODE_FUNC	               	
		work_mode = SYS_MP3DECODE_USB;
#else
		work_mode = SYS_FMREV;
#endif
	}
#else
	work_mode = SYS_MCU_CD;
#endif
#endif
}
#ifdef USE_SYS_IDEL_FUNC
extern xd_u8 alm_flag;
void idle_mode(void)
{
    u8 key;
    //dac_out_select(DAC_MUSIC, 0);
    //clear_all_event();
#ifdef UART_ENABLE
	sys_printf(" SYS GO IN IDLE MODE");
#endif
    
#ifdef PWR_CTRL_IN_IDLE_MODE

#if defined(PWR_CTRL_WKUP)
	wkup_pin_ctrl(0);
#else
	PWR_CTRL_GPIO_INIT();
	PWR_CTRL_DIS();
#endif

#endif


    flush_all_msg();


    KT_AMFMStandby();

    sys_sleep_mode();
	
#ifdef RTC_DISP_IN_IDLE_MODE
	disp_scenario = DISP_RTC_SCEN;
	Disp_Con(DISP_RTC);
#else
    Disp_Con(DISP_POWER_OFF);
#endif

#if 0
        alm_flag = 1;
	 put_msg_lifo(INFO_ALM_BELL);
#endif	 

#ifdef ADKEY_SELECT_MODE
   mode_switch_protect_bit=0;
#endif


   while (1)
    {
	key = get_msg();
	//suspend_sdmmc();

#ifndef RTC_DISP_IN_IDLE_MODE
	if(sys_pwr_flag){
		if(key!=(INFO_POWER | KEY_SHORT_UP)){
			key = NO_KEY;
		}
	}
#endif	
        switch (key)
        {
#if 0        
	       case INFO_PLAY| KEY_SHORT_UP :	

		        alm_flag = 1;
	 put_msg_lifo(INFO_ALM_BELL);
	 	break;
#endif		
	       case INFO_POWER | KEY_SHORT_UP :	
			if(sys_pwr_flag){

				//sys_mute_flag =0;
	        		dac_mute_control(0,1);					//调节音量时，自动UNMUTE
				sys_pwr_flag =0;
				//work_mode =  SYS_IDLE;
			}
			break;
	       case INFO_HALF_SECOND :
#ifdef RTC_DISP_IN_IDLE_MODE
#if defined(USE_BAT_MANAGEMENT)
	     		bmt_hdlr();
#endif
            	    set_brightness_fade_out();

		     rtc_setting_exit();

		    if(disp_scenario == DISP_RTC_SCEN){

				if(curr_menu != DISP_RTC){
	            			return_cnt++;
	            			if (RETURN_TIME == return_cnt){
						rtc_disp_hdlr();
					}
					break;
				}
				rtc_disp_hdlr();
				break;
		     }
#ifdef USE_RTC_ALARM_FUNCTION
		    else if(disp_scenario == DISP_ALM_SCEN){

				rtc_disp_hdlr();
		     }
#endif			
#endif			
			break;
			
        	case INFO_NEXT_SYS_MODE:
            		return;
#ifdef USE_RTC_ALARM_FUNCTION
    		case INFO_ALM_BELL:
		    	sys_restore_mode();
			alm_bell_mode();
		    	//sys_sleep_mode();
#ifdef ALARM_USE_MULTI_SOURCE_FUNC
			if(get_cur_select_func()==SYS_MP3DECODE_USB){

				if(get_device_online_status()>0){
					alarm_power_on_protect =1;
					work_mode = SYS_MP3DECODE_USB;
    					flush_all_msg();					
		 			put_msg_lifo(INFO_NEXT_SYS_MODE);
					break;
				}
			}
			else if(get_cur_select_func()==SYS_FMREV){
				
				work_mode = SYS_FMREV;
				alarm_power_on_protect =1;	
    				flush_all_msg();									
		 		put_msg_lifo(INFO_NEXT_SYS_MODE);
				break;
			}
#endif			
			break;
#endif
    		case MSG_USB_DISK_IN  :
    		case MSG_SDMMC_IN :
	 		break;
	        default:
	            ap_handle_hotkey(key);        
	            break;
        }
    }
}
#endif

/*----------------------------------------------------------------------------*/
/**@brief  主函数
   @param  无
   @return 无
   @note   void main(void)
*/
/*----------------------------------------------------------------------------*/
void main(void)
{
     xd_u8 sys_timer=0;

	Set_Curr_Func(SYS_IDLE);


      Mute_Ext_PA(MUTE);
	 
#ifdef PWR_CTRL_IN_IDLE_MODE

#if defined(PWR_CTRL_WKUP)
	wkup_pin_ctrl(0);
#else
	PWR_CTRL_GPIO_INIT();
	PWR_CTRL_DIS();
#endif

#endif

#ifdef ALARM_USE_MULTI_SOURCE_FUNC
	alarm_power_on_protect=0;
#endif
#ifdef ADKEY_SELECT_MODE
   	mode_switch_protect_bit=1;
#endif
	sys_clock_pll();//(MAIN_CLK_PLL);
#ifdef USE_POWER_KEY
	waiting_power_key();
#endif	
	Disp_Con(DISP_HELLO);
	sys_init();
    	sys_info_init();
	Init_Func_List();
	flush_all_msg();
#if FILE_ENCRYPTION
    password_init(0xaa);  //输入加密文件的密码
#endif  

#ifdef ADKEY_DEBUG
	AD_Debug_func();
#endif

#ifdef SYS_POWER_ON_DEFAULT_IN_RADIO
	Set_Curr_Func(SYS_FMREV);
#elif defined(SYS_POWER_ON_DEFAULT_IN_CD)
	Set_Curr_Func(SYS_MCU_CD);
#endif

#ifdef SYS_GPIO_SEL_FUNC
	//gpio_sel_func_mode();
#endif

#ifdef UART_ENABLE
    	printf("------->- SYS INIT   work_mode:%d   \r\n",(u16)work_mode);
#endif
	while (1)
       {
		Set_Curr_Func(work_mode);
        	switch (work_mode)
	       {
#ifdef USE_USB_SD_DECODE_FUNC	       
	 		case SYS_MP3DECODE_USB:
	 		case SYS_MP3DECODE_SD:	 
	 			decode_play();
	            	break;
#endif					
#ifdef USE_CD_MCU_MASTER_FUNC
		    	case SYS_MCU_CD:
				mcu_main_hdlr();
			break;
#endif			
#ifdef USE_RADIO_FUNC
	        	case SYS_FMREV:
#ifdef AM_RADIO_FUNC
	        	case SYS_AMREV:
#endif					
	            		fm_radio();
	            	break;
#endif					
#ifdef USE_AUX_FUNC			
	        	case SYS_AUX:
	            		aux_function();
	            	break;
#endif			
#ifdef USE_RTC_FUNC
	        	case SYS_RTC:
	            		rtc_function();
	            	break;
#endif
#ifdef USE_SYS_IDEL_FUNC
	        	case SYS_IDLE:
		            idle_mode();
#ifdef ADKEY_SELECT_MODE				
		   	     mode_switch_protect_bit=1;			
#endif
		   	     sys_restore_mode();
			     dac_mute_control(0,1);
			     flush_all_msg();	
			     music_vol = 26;
			     set_max_vol(MAX_ANALOG_VOL, MAX_DIGITAL_VOL);
			     main_vol_set(music_vol, CHANGE_VOL_MEM);		 
		     	break;
#endif			 
	        	default:

#ifdef USE_USB_SD_DECODE_FUNC	               	
	            		work_mode = SYS_MP3DECODE_USB;
#else
	            		work_mode = SYS_MCU_CD;
#endif
	            	break;
	        }
#ifdef USE_SYS_MODE_RECOVER
	if(work_mode !=SYS_IDLE){
              write_info(MEM_SYSMODE,work_mode);
    	}
#endif	
			
    }

}
