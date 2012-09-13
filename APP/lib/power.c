/*--------------------------------------------------------------------------*/
/**@file    power.c
   @brief   ��Դ����
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
/**@brief ϵͳ����ʡ��(sleep)ģʽ
   @param ��
   @return  ��
   @note ϵͳ����ʡ��ģʽ��оƬ����Ϊ200uA���£�ͨ��P00,P05,P07,P34���Ի���оƬ��оƬ�����󣬰�resetģʽ����(����������к���ĳ���)\n
   ϵͳ����Ϊ24Mʱ����֧�ִ˹���\n
   ���оƬ�Ĺ�����ͨ��һ���������(��MOS��)������Ҫ�ػ�ʱ������Ҫ���ô˺�����ֱ�ӹص�MOS�ܾͿ����ˡ�
*/
/*----------------------------------------------------------------------------*/
void system_sleep(void)
{
#if OSC_CLOCK == 24000000L
#else
    dac_off();				//�ر�DAC����ҪԼ3S�Ӳ�û�����������������mute���ܣ�Ҳ���Խ��˾任��mute����.����dac_off()������delay_10ms����������֮ǰ���ܹ��ж�
    EA = 0;
    DACCON0 = 0xc0;			//�ٴι�DAC�����û��ִ��dac_off(),DACͬ�����Ա��ص���ֻ�ǻ�������������������˹��ŵ�mute��Ҳ����������
    DACCON1 = 0;
    DACCON2 = 0;
    Delay(20000);
    USBCON0 = 0;			//��USB
    ADCCON = 0;				//��ADC
    P0DIR = 0xFF;			//��IO�������ԭ��Ϊ������IO����Ϊ���ֹ��ܣ��������������IO�Ѿ�����Χ��·�����˹̶�״̬����һ������ѡ��FMоƬ��IO�ڣ��Ѿ�ͨ���ⲿ����ӵ��أ���
    P1DIR = 0xFF;			//���������������������IO�����յģ�����Ҫ���������������򿪣���֤���ƽ���ᴦ�ڸ��ա�
    P2DIR = 0xFF;
    P3DIR = 0xFF;
    P4DIR = 0xFF;

    P0IE = 0xff;
    P0PU = 0;
    P1PU = 0;
    P2PU = 0xff;
    P3PU = 0;
    P4PU = 0xff;

    P0PD = 0x7f;			//�����У�P07����ʱ��ΪADKEYʹ�ã�sleep����Ϊ���ֿڣ�ͨ���½�����resetϵͳ����ADKEY�ⲿ�������������ڲ����ܿ��ϣ�����
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
    WKUPCON |= BIT(2) | BIT(6);			//����ʹ��P07��Ϊ����IO
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
    	DACCON1 |= BIT(6);                  //DAC����
	CLKGAT |= BIT(5);  //SDCLK�ر�	

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
	CLKGAT &= ~(BIT(5));  //SDCLK��
	
    	DACCON1 &=~(BIT(6));                  //DAC����
	Delay(200);	
#endif	
	sys_clock_pll();		
	dac_mute_control(0,1);
#endif	

}
/*----------------------------------------------------------------------------*/
/**@brief   ����ϵͳʱ�ӹ�����RC��
   @param   ��
   @return  ��
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
/**@brief   ��ϵͳʱ�ӹ�����PLL��
   @param   ��
   @return  ��
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
/**@brief   �ȴ������ĵ�Դ����̧��
   @param    ��
   @return   ��
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
/**@brief  ����RCʱ�ӷ�Ƶ
   @param   ��
   @return  ��
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
/**@brief    PLL��ʱ����Ϻ�ָ���RCʱ��ģʽ
   @param    ��
   @return   ��
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
/**@brief    ϵͳ���纯��
   @param    ��
   @return   ��
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
/**@brief   ϵͳ�ϵ纯��
   @param   ��
   @return  ��
   @note    void sys_power_up(void)
*/
/*----------------------------------------------------------------------------*/
void sys_power_up(void)
{
    //power_ctl(1);
}

#if defined(PWR_CTRL_WKUP)||defined(WKUP_PIN_USE_ENABLE)
/***
RTC ���ƼĴ�����BIT4 = 1ʱ����������Чʱ����WKUP��1.8V�����BIT4 = 0���ҡ�������Чʱ���ر�WKUP��1.8V������ڶ�BIT4����ʱ����λ���������ܸĶ����üĴ�������λ��ֵ��
������дRTC ���ƼĴ����ĺ����ֱ��ǣ�
����u8 read_rtc_reg(void)
����void write_rtc_reg(u8 cmd)
---------------------------------------------------------------------------------------------------
*********************************/
/**@brief        WKUP������ƺ���
@param     dir:1���1.8V��0���ر����
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

