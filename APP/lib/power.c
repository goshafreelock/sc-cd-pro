/*--------------------------------------------------------------------------*/
/**@file    power.c
   @brief   电源管理
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#include "Custom_config.h"
#include "power.h"

bool sys_clock_flag;  ///<AD
u8 clock_change_delay;
extern void lcd_power_down(void);
extern void sys_init(void);
extern void sys_info_init(void);
/*----------------------------------------------------------------------------*/
/**@brief 系统进入省电(sleep)模式
   @param 无
   @return  无
   @note 系统进入省电模式后，芯片功耗为200uA以下，通过P00,P05,P07,P34可以唤醒芯片，芯片起来后，按reset模式运行(不会接着运行后面的程序)\n
   系统晶振为24M时，不支持此功能\n
   如果芯片的供电是通过一个软件开关(如MOS管)，当需要关机时，不需要调用此函数，直接关掉MOS管就可以了。
*/
/*----------------------------------------------------------------------------*/
void system_sleep(void)
{
#if OSC_CLOCK == 24000000L
#else
    dac_off();				//关闭DAC，需要约3S钟才没有杂音，如果功放有mute功能，也可以将此句换成mute功放.由于dac_off()调用了delay_10ms，所以在这之前不能关中断
    EA = 0;
    DACCON0 = 0xc0;			//再次关DAC，如果没有执行dac_off(),DAC同样可以被关掉，只是会有杂音，但如果控制了功放的mute，也不会有杂音
    DACCON1 = 0;
    DACCON2 = 0;
    Delay(20000);
    USBCON0 = 0;			//关USB
    ADCCON = 0;				//关ADC
    P0DIR = 0xFF;			//关IO，具体的原则为：所有IO必须为数字功能，并设成输出，如果IO已经被外围电路拉成了固定状态（如一个用于选择FM芯片的IO口，已经通过外部电阻接到地），
    P1DIR = 0xFF;			//则不用再作其它处理，但如果IO是悬空的，则需要将其上拉或下拉打开，保证其电平不会处于浮空。
    P2DIR = 0xFF;
    P3DIR = 0xFF;
    P4DIR = 0xFF;

    P0IE = 0xff;
    P0PU = 0;
    P1PU = 0;
    P2PU = 0xff;
    P3PU = 0;
    P4PU = 0xff;

    P0PD = 0x7f;			//本例中，P07正常时作为ADKEY使用，sleep后，作为数字口，通过下降沿来reset系统，因ADKEY外部有上拉，所以内部不能开上，下拉
    P1PDB = 0;
    P1PDA = 0xff;
    P3PD = 0xff;

#if OSC_CLOCK == 7600000L
    PLL_input(PLL_IN_76M);
#endif

#if OSC_CLOCK == 12000000L
    PLL_input(PLL_IN_12M);
#endif

    Delay(1000);
    P3IE = 0x7F;

#if OSC_CLOCK == 32768L
    mainclock_select(MAIN_CLK_32K);
#else
    CLKCON |= 0X4;
    mainclock_select(MAIN_CLK_RC);
#endif

    CLKCON &= ~0x36;
    WKUPPND &= 0xf0;
    WKUPCON |= BIT(2) | BIT(6);			//本例使用P07作为唤醒IO
    PCON = 1;
#endif
}
void sys_sleep_mode()
{
#if 1
	usb_suspend();			//Entered Suspend mode
	suspend_sdmmc();
	dac_mute_control(1,1);
#if 1
    	DACCON1 |= BIT(6);                  //DAC高阻
	CLKGAT |= BIT(5);  //SDCLK关闭	

	//CLKCON |= (BIT(3));
	//CLKCON |= (BIT(6));
	//Delay(600);
#endif	

	rc_pll_delay();
	//Delay(200);	
	sysclock_div2(1);
#endif	
	
}
void sys_restore_mode()
{
#if 1
#if 1
	//CLKCON &=~(BIT(3));
	//CLKCON &=~(BIT(6));
	//delay_10ms(2);
	CLKGAT &= ~(BIT(5));  //SDCLK打开
	
    	DACCON1 &=~(BIT(6));                  //DAC高阻
	Delay(200);	
#endif	
	sys_clock_pll();		
	dac_mute_control(0,1);
#endif	

}
/*----------------------------------------------------------------------------*/
/**@brief   设置系统时钟工作在RC上
   @param   无
   @return  无
   @note    void sys_clock_rc(void)
*/
/*----------------------------------------------------------------------------*/
#pragma disable
void sys_clock_rc(void)
{
    mainclock_select(MAIN_CLK_RC);
    sd_speed_init(0, 3);
    SPIBAUD = 0x0;
    sys_clock_flag = 0;
}

/*----------------------------------------------------------------------------*/
/**@brief   设系统时钟工作在PLL上
   @param   无
   @return  无
   @note    void sys_clock_pll(void)
*/
/*----------------------------------------------------------------------------*/
#pragma disable
void sys_clock_pll(void)
{
    SPIBAUD = 0x04;
    sd_speed_init(1, 200);
    mainclock_select(MAIN_CLK_PLL);
    sys_clock_flag = 1;
    clock_change_delay = 0;
}

/*----------------------------------------------------------------------------*/
/**@brief   等待开机的电源按键抬起
   @param    无
   @return   无
   @note    void waiting_power_key()
*/
/*----------------------------------------------------------------------------*/
void waiting_power_key()
{
    //while (P34)
  //  {
  //  }
}

/*----------------------------------------------------------------------------*/
/**@brief  设置RC时钟分频
   @param   无
   @return  无
   @note    void adc_clock(void)
*/
/*----------------------------------------------------------------------------*/
void adc_clock(void)
{
    if (sys_clock_flag)
    {
        ADCCON |= 0x30;
    }
}

/*----------------------------------------------------------------------------*/
/**@brief    PLL延时，完毕后恢复到RC时钟模式
   @param    无
   @return   无
   @note     void rc_pll_delay(void)
*/
/*----------------------------------------------------------------------------*/
void rc_pll_delay(void)
{
    if (clock_change_delay < 20)
    {
        clock_change_delay++;
    }
    else if (clock_change_delay<60)
    {
        clock_change_delay = 100;
	sys_clock_rc();	
    }
}

/*----------------------------------------------------------------------------*/
/**@brief    系统掉电函数
   @param    无
   @return   无
   @note     void sys_power_down(void)
*/
/*----------------------------------------------------------------------------*/
void sys_power_down(void)
{
    //lcd_power_down();
   // EA = 0;
    //power_ctl(0);
    //while (1);
}

/*----------------------------------------------------------------------------*/
/**@brief   系统上电函数
   @param   无
   @return  无
   @note    void sys_power_up(void)
*/
/*----------------------------------------------------------------------------*/
void sys_power_up(void)
{
    //power_ctl(1);
}

#if defined(PWR_CTRL_WKUP)||defined(WKUP_PIN_USE_ENABLE)
/***
RTC 控制寄存器，BIT4 = 1时　或　闹钟有效时　打开WKUP的1.8V输出；BIT4 = 0　且　闹钟无效时　关闭WKUP的1.8V输出。在对BIT4操作时必须位操作，不能改动到该寄存器其他位的值。
　　读写RTC 控制寄存器的函数分别是：
　　u8 read_rtc_reg(void)
　　void write_rtc_reg(u8 cmd)
---------------------------------------------------------------------------------------------------
*********************************/
/**@brief        WKUP输出控制函数
@param     dir:1输出1.8V；0：关闭输出
@return
@note        void wkup_ctl(bool dir) 
/*********************************/
#define IRTC_CSEN_LIB    IRTCON |= BIT(0)
#define IRTC_CSDIS_LIB   IRTCON &= ~(BIT(0))
void wkup_pin_ctrl(bool dir)    
{
	u8 tmp;
	IRTC_CSEN_LIB;
	irtc_sr_byte(0x3b);
	tmp = irtc_sr_byte(0);
	IRTC_CSDIS_LIB;
	if(dir)
	{
		IRTC_CSEN_LIB;
		irtc_sr_byte(0x3a);
		irtc_sr_byte(tmp|BIT(4));
		IRTC_CSDIS_LIB;
	}
	else
	{
		IRTC_CSEN_LIB;
		irtc_sr_byte(0x3a);
		irtc_sr_byte(tmp&~(BIT(4)));
		IRTC_CSDIS_LIB;
	 }
}
#endif

