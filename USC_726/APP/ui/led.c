/*--------------------------------------------------------------------------*/
/**@file    led.c
   @brief   4位LED驱动
   @details
   @author
   @date   2010-12-13
   @note
*/
/*----------------------------------------------------------------------------*/
#include "Custom_config.h"
#include "led.h"
#ifdef DISP_LED_GPIO_DRV


#define LED_SBUF_IDX		4
#define LED0_STATUS          LED_BUFF[LED_SBUF_IDX]
#define LED0_FLASH_STATUS    LED_FLASH[LED_SBUF_IDX]



xd_u8 LED_BUFF[5];   ///<LED显存
xd_u8 LED_FLASH[5];
/*
u8 _code led_icon[] =    ///<LED标志位
{
    LED0_FM,
    LED0_USB,
    LED0_SD,
    LED0_MP3,
    LED0_2POINT,
	LED0_PLAY,
	LED0_PAUSE,
};
*/
#define LED_MINUS     0x40

u8 _code LED_NUMBER[10] =
{
    0x3f,0x06,0x5b,0x4f,0x66,  ///<0~4
    0x6d,0x7d,0x07,0x7f,0x6f   ///<5~9
};

u8 _code LED_LARGE_LETTER[26] =
{
    0x77,0x40,0x39,0x3f,0x79,///<ABCDE
    0x71,0x40,0x76,0x06,0x40,///<FGHIJ
    0x40,0x38,0x40,0x37,0x3f,///<KLMNO
    0x73,0x40,0x50,0x6d,0x40,///<PQRST
    0x3e,0x3e,0x40,0x76,0x40,///<UVWXY
    0x40///<Z
};

u8 _code LED_SMALL_LETTER[26] =
{
    0x77,0x7c,0x58,0x5e,0x79,///<abcde
    0x71,0x40,0x40,0x40,0x40,///<fghij
    0x40,0x38,0x40,0x54,0x5c,///<klmno
    0x73,0x67,0x50,0x40,0x40,///<pqrst
    0x3e,0x3e,0x40,0x40,0x40,///<uvwxy
    0x40///<z
};
/*----------------------------------------------------------------------------*/
/**@brief    清空LED BUFF函数，LED显示位置设为1
   @param	 无
   @return	 无
   @note     void clear_led(void)
*/
/*----------------------------------------------------------------------------*/
void init_disp_buf(void)
{
    LED_BUFF[0] = 0;	  /* 千位 */
    LED_BUFF[1] = 0;	  /* 百位 */
    LED_BUFF[2] = 0;	  /* 十位 */
    LED_BUFF[3] = 0;	  /* 个位 */
    LED_BUFF[4] = 0;	  /* 状态位 */
    LED_FLASH[0] = 0;
    LED_FLASH[1] = 0;
    LED_FLASH[2] = 0;
    LED_FLASH[3] = 0;
    LED_FLASH[4] = 0;
}

/*----------------------------------------------------------------------------*/
/**@brief    数码管状态位显示
   @param	 id：要显示的状态位
   @return	 无
   @note     void led_disp_icon(u8 id)
*/
/*----------------------------------------------------------------------------*/
void disp_icon(u8 id)
{
    LED0_STATUS |= id;	
}
void disp_clr_icon(u8 id)
{
    LED0_STATUS &=~id;	
}
/*----------------------------------------------------------------------------*/
/**@brief    LED闪烁设置
   @param	 id：状态位
   @return	 无
   @note     void led_flash_icon(u8 id)
*/
/*----------------------------------------------------------------------------*/
void disp_clr_flash_icon(u8 id)
{
    LED0_FLASH_STATUS &=~id;	
}
void disp_flash_icon(u8 id)
{
    LED0_FLASH_STATUS |= id;	
}

/*----------------------------------------------------------------------------*/
/**@brief    LED闪烁设置
   @param	 flag：闪烁的数据位
   @return	 无
   @note     void led_flash_section(u8 flag)
*/
/*----------------------------------------------------------------------------*/
void led_flash_section(u8 flag)
{
	if(flag!= LED_SBUF_IDX)
	{
	    LED_FLASH[flag]=0xff;
	}	
}
/*----------------------------------------------------------------------------*/
/**@brief    LED 显示初始化
   @param	 无
   @return	 无
   @note     void init_led_disp(void)
*/
/*----------------------------------------------------------------------------*/
void init_disp(void)
{
    	init_disp_buf();
    	led_com_init();
    	led_seg_init();
}

/*----------------------------------------------------------------------------*/
/**@brief    LED 亮度设置
   @param	 br:亮度数值
   @return	 无
   @note     void led_set_brightness(u8 br)
*/
/*----------------------------------------------------------------------------*/
void disp_set_brightness(u8 br)
{
    if (br > 16)
        return;
    if (br == 16)
        PWM4CON = 0;
    else
        PWM4CON = br | 0xd0;
}

/*----------------------------------------------------------------------------*/
/**@brief    LED 字符显示
   @param	 chardata：字符数据
   @return	 无
   @note     void led_putchar(u8 chardata,u8 loc)
*/
/*----------------------------------------------------------------------------*/
void disp_putchar(u8 chardata,u8 loc)
{
    loc += 0;
    if ((chardata < ' ') || (chardata > '~'))
    {
        return;
    }
    if ((chardata >= '0') && (chardata <= '9'))
    {
        LED_BUFF[loc] = LED_NUMBER[chardata - '0'];
    }
    else if ((chardata >= 'a') && (chardata <= 'z'))
    {
        LED_BUFF[loc] = LED_SMALL_LETTER[chardata - 'a'];
    }
    else if ((chardata >= 'A') && (chardata <= 'Z'))
    {
        LED_BUFF[loc] = LED_LARGE_LETTER[chardata - 'A'];
    }
    else if (chardata == ':')
    {
        LED0_STATUS |= ICON_2POINT;
    }
    else if (chardata == ' ')
    {
    }
    else if (chardata == '-')
    {
        LED_BUFF[loc] = LED_MINUS;
    }
}

/*----------------------------------------------------------------------------*/
/**@brief    LED扫描函数
   @param
   @return
   @note     void disp_scan(void)
*/
/*----------------------------------------------------------------------------*/
#if defined(USE_BAT_MANAGEMENT)
extern void custom_buf_update(void);
#endif

#ifdef LED_CUSTOM_SCAN_HARDWARE
u8 _code align_com_gpio[5] ={BIT(1),BIT(2),BIT(3),BIT(4),BIT(0)};
void align_seg_gpio(u8 seg_data)
{
 	P16=((seg_data&0x01)>0)?1:0;
 	P15=((seg_data&0x02)>0)?1:0;
 	P14=((seg_data&0x04)>0)?1:0;
 	P13=((seg_data&0x08)>0)?1:0;
 	P12=((seg_data&0x10)>0)?1:0;
 	P11=((seg_data&0x20)>0)?1:0;
 	P10=((seg_data&0x40)>0)?1:0;
}
#endif
void disp_scan(void)
{
    static u8 cnt;
    static u8 counter;
    static bool flash;
    u8 com, seg;


#if defined(USE_BAT_MANAGEMENT)
    custom_buf_update();
#endif
    counter++;
    if (counter == 250)
    {
        counter = 0;
        flash = !flash;
    }

    if (flash)
    {
        seg = LED_BUFF[cnt] ^ LED_FLASH[cnt];   
    }
    else
    {
        seg = LED_BUFF[cnt]; 
    }
    com = BIT(cnt); 
    /**/
    LED_COM &= ~0x1f;
    LED_COM_MASK &= ~0x1f;
    LED_SEG |= 0x7f;			 //清LED COM SEG
    
#ifdef LED_CUSTOM_SCAN_HARDWARE
    align_seg_gpio(~seg);
    LED_COM |= align_com_gpio[cnt];
    LED_COM_MASK |= align_com_gpio[cnt];
#else
    LED_SEG &= ~seg;
    LED_COM |= com;
    LED_COM_MASK |= com;
#endif	
    cnt = (cnt >= 4)?( 0 ): (cnt+1);
}
#endif
