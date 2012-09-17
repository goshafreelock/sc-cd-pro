/*lcd segment*/
#include "Custom_config.h"
#include "lcdsegment.h"

#ifdef DISP_LCD_GPIO_DRV

xd_u16 lcd_buff[5]={0};

extern void custom_buf_update(void);

u8 _code LCD_NUMBER[10] =
{
    0x3f,0x06,0x5b,0x4f,0x66,  ///<0~4
    0x6d,0x7d,0x07,0x7f,0x6f   ///<5~9
};

u8 _code LCD_LARGE_LETTER[26] =
{
    0x77,0x40,0x39,0x3f,0x79,///<ABCDE
    0x71,0x40,0x76,0x06,0x40,///<FGHIJ
    0x40,0x38,0x40,0x37,0x3f,///<KLMNO
    0x73,0x40,0x50,0x6d,0x40,///<PQRST
    0x3e,0x3e,0x40,0x76,0x40,///<UVWXY
    0x40///<Z
};

u8 _code LCD_SMALL_LETTER[26] =
{
    0x77,0x7c,0x58,0x5e,0x79,///<abcde
    0x71,0x40,0x40,0x40,0x40,///<fghij
    0x40,0x38,0x40,0x54,0x5c,///<klmno
    0x73,0x67,0x50,0x40,0x40,///<pqrst
    0x3e,0x3e,0x40,0x40,0x40,///<uvwxy
    0x40///<z
};
void init_disp_buf(void)
{
	lcd_buff[0] =0x0000;
	lcd_buff[1] =0x0000;
	lcd_buff[2] =0x0000;
	lcd_buff[3] =0x0000;
	lcd_buff[4] =0x0000;
}
#if 0
void lcd_backlight_ctrl(bool on)
{
	P0DIR &=~(BIT(4));P0PU|= (BIT(4)); 

	if(!on){
    		LCD_BACKLIGHT_HIGH();
	}
	else{
    		LCD_BACKLIGHT_LOW();
	}

}
#endif
/*----------------------------------------------------------------------------*/
/**@brief    数码管状态位显示
   @param	 id：要显示的状态位
   @return	 无
   @note     void led_disp_icon(u8 id)
*/
/*----------------------------------------------------------------------------*/

void disp_icon(u8 id)
{
    switch(id)
    {
	case ICON_USB:
		F_USB_DEV |=USB_DEV_MASK;
		break;
	case ICON_SD:
		F_SD_DEV |=SD_DEV_MASK;
		break;
	case ICON_AUX:
		F_AUX_DEV |=AUX_DEV_MASK;
		break;
	case ICON_FM_MHZ:

		F_FM_DEV |= FM_DEV_MASK;
		F_MHZ_DEV |=FM_MHZ_MASK;
		break;
	case ICON_AM_KHZ:
		F_AM_DEV |= AM_DEV_MASK;
		F_KHZ_DEV |=AM_KHZ_MASK;
		break;
	case ICON_SW:
		F_SW_DEV |=SW_MHZ_MASK;
#if defined(NEW_DH_LCD_MODULE)
		F_P1_DEV |=FM_P1_MASK;
#endif					
		break;				
	case ICON_REP_1:
		F_REP_ONE|=REP_ONE_MASK;
		break;
	case ICON_REP_ALL:
		F_REP_ALL |=REP_ALL_MASK;
		break;
	case ICON_COL:
		F_POINTS |=POINTS_MASK;
		break;		
	case ICON_MUTE:
		F_SPK_DEV |=SPK_MUTE_MASK;
		break;
#ifdef USE_RTC_ALARM_FUNCTION
	case ICON_BELL:
		F_RTC_ALM |=RTC_ALM_MASK;
		break;
#endif		
	case ICON_BAT:
		F_BAT_ICON_BUF |=BATTERY_MASK;
		break;	
	case ICON_BAT_S1:
		S1_ICON_BUF |=S1_ICON_MASK;
		break;	
	case ICON_BAT_S2:
		S1_ICON_BUF |=S2_ICON_MASK;
		break;	
	case ICON_BAT_S3:
		S1_ICON_BUF |=S3_ICON_MASK;
		break;	
	case ICON_BAT_S4:
		S1_ICON_BUF |=S4_ICON_MASK;
		break;		
    }
}
void disp_clr_icon(u8 id)
{

    switch(id)
    {
	case ICON_USB:
		F_USB_DEV &=~USB_DEV_MASK;
		break;
	case ICON_SD:
		F_SD_DEV &=~SD_DEV_MASK;
		break;
	case ICON_AUX:
		F_AUX_DEV &=~AUX_DEV_MASK;
		break;
	case ICON_FM_MHZ:
		F_MHZ_DEV &=~FM_MHZ_MASK;
		break;
	case ICON_AM_KHZ:
		F_KHZ_DEV &=~AM_KHZ_MASK;
		break;
	case ICON_SW:
		F_SW_DEV &=~SW_MHZ_MASK;
#if defined(NEW_DH_LCD_MODULE)
		F_P1_DEV &=~FM_P1_MASK;
#endif					
		break;			
	case ICON_REP_1:
		F_REP_ONE&=~REP_ONE_MASK;
		break;
	case ICON_REP_ALL:
		F_REP_ALL &=~REP_ALL_MASK;
		break;
	case ICON_COL:
		F_POINTS &=~POINTS_MASK;
		break;
	case ICON_MUTE:
		F_SPK_DEV &=~SPK_MUTE_MASK;
		break;	
#ifdef USE_RTC_ALARM_FUNCTION
	case ICON_BELL:
		F_RTC_ALM &=~RTC_ALM_MASK;
		break;
#endif				
	case ICON_BAT:
		F_BAT_ICON_BUF &=~BATTERY_MASK;
		break;	
	case ICON_BAT_S1:
		S1_ICON_BUF &=~S1_ICON_MASK;
		break;	
	case ICON_BAT_S2:
		S1_ICON_BUF &=~S2_ICON_MASK;
		break;	
	case ICON_BAT_S3:
		S1_ICON_BUF &=~S3_ICON_MASK;
		break;	
	case ICON_BAT_S4:
		S1_ICON_BUF &=~S4_ICON_MASK;			
    }
}
/*----------------------------------------------------------------------------*/
/**@brief    LED闪烁设置
   @param	 id：状态位
   @return	 无
   @note     void led_flash_icon(u8 id)
*/
/*----------------------------------------------------------------------------*/
u8  xdata lcd_flash_icon=ICON_NONE;
u8 xdata lcd_flash_timer=0;
void disp_flash_icon(u8 id)
{
    lcd_flash_icon = id;	
}
void disp_clr_flash_icon(u8 id)
{
	lcd_flash_icon =0xFF;
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
    	seg_init();
    	com_init();
	LCD_BACKLIGHT_INIT();
	LCD_BACKLIGHT_ON();
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
#if 0
    if (br > 16)
        return;
    if (br == 16)
        PWM4CON = 0;
    else
        PWM4CON = br | 0xd0;
#endif
}

/*----------------------------------------------------------------------------*/
/**@brief    LED 字符显示
   @param	 chardata：字符数据
   @return	 无
   @note     void led_putchar(u8 chardata,u8 loc)
*/
/*----------------------------------------------------------------------------*/
#if defined(BK_DISP_LCD_DRV)
u8 _code lcd_disbuf_offset[4] ={0,2,4,6};
void align_lcd_disp_buff(u8 offset,u8 letter_data)
{
	u8 digit_idx=offset;
	
	digit_idx= lcd_disbuf_offset[offset];

	lcd_buff[1] &= ~(0x0002<<digit_idx);
	lcd_buff[2] &= ~(0x0003<<digit_idx);
	lcd_buff[3] &= ~(0x0003<<digit_idx);
	lcd_buff[4] &= ~(0x0003<<digit_idx);

       lcd_buff[1] |= ((letter_data & DIG_A)<<1)<<digit_idx;
       lcd_buff[2] |= (((letter_data & DIG_B))|((letter_data & DIG_F)>>5))<<digit_idx;
       lcd_buff[3] |= (((letter_data & DIG_C)>>1)|((letter_data & DIG_G)>>6))<<digit_idx;
       lcd_buff[4] |= (((letter_data & DIG_D)>>2)|((letter_data & DIG_E)>>4))<<digit_idx;   	 
}
#elif defined(JK_CD_727_V001)
u8 _code lcd_disbuf_offset[4] ={5,3,1,0};
void align_lcd_disp_buff(u8 offset,u8 letter_data)
{
	u8 digit_idx=offset;

	if(offset==3){
		
		lcd_buff[3] &= ~(0x0100);
		lcd_buff[2] &= ~(0x0101);
		lcd_buff[1] &= ~(0x0101);
		lcd_buff[0] &= ~(0x0101);

	 	lcd_buff[3] |= ((letter_data & DIG_A)>0)?0x0100:0;
	       lcd_buff[2] |= ((((letter_data & DIG_B)>0)?0x0100:0)|(((letter_data & DIG_F)>0)?0x0001:0));
	       lcd_buff[1] |= ((((letter_data & DIG_C)>0)?0x0100:0)|(((letter_data & DIG_G)>0)?0x0001:0));
	       lcd_buff[0] |= ((((letter_data & DIG_D)>0)?0x0100:0)|(((letter_data & DIG_E)>0)?0x0001:0));    
	}
	else{
		digit_idx= lcd_disbuf_offset[offset];

		lcd_buff[3] &= ~(0x0001<<digit_idx);
		lcd_buff[2] &= ~(0x0003<<digit_idx);
		lcd_buff[1] &= ~(0x0003<<digit_idx);
		lcd_buff[0] &= ~(0x0003<<digit_idx);

	       lcd_buff[3] |= ((letter_data & DIG_A))<<digit_idx;
	       lcd_buff[2] |= (((letter_data & DIG_B)>>1)|((letter_data & DIG_F)>>4))<<digit_idx;
	       lcd_buff[1] |= (((letter_data & DIG_C)>>2)|((letter_data & DIG_G)>>5))<<digit_idx;
	       lcd_buff[0] |= (((letter_data & DIG_D)>>3)|((letter_data & DIG_E)>>3))<<digit_idx;    
	}
}
#elif defined(NEW_DH_LCD_MODULE)
u8 _code lcd_disbuf_offset[4] ={0,1,3,5};
void align_lcd_disp_buff(u8 offset,u8 letter_data)
{
	u8 digit_idx=offset;
	
	digit_idx= lcd_disbuf_offset[offset];
	
	if(digit_idx==0)
	{
		 lcd_buff[2]&=~0x0001;
		 lcd_buff[3]&=~0x0001;
	        lcd_buff[4]&=~0x0001;
			
		 if((letter_data == LCD_NUMBER[1])){
			lcd_buff[2]|=0x0001;
			lcd_buff[3]|=0x0001;
		    }
		    else if((letter_data == LCD_NUMBER[2])){
			lcd_buff[4]|=0x0001;
			lcd_buff[3]|=0x0001;
		   }	
	   	return;
	}
	else{
		
	    	lcd_buff[4] &= ~(0x0002<<digit_idx);
		lcd_buff[3] &= ~(0x0003<<digit_idx);
		lcd_buff[2] &= ~(0x0003<<digit_idx);
		lcd_buff[1] &= ~(0x0003<<digit_idx);
	}
	
       lcd_buff[4] |= ((letter_data & DIG_A)<<1)<<digit_idx;
       lcd_buff[3] |= (((letter_data & DIG_B))|((letter_data & DIG_F)>>5))<<digit_idx;
       lcd_buff[2] |= (((letter_data & DIG_C)>>1)|((letter_data & DIG_G)>>6))<<digit_idx;
       lcd_buff[1] |= (((letter_data & DIG_D)>>2)|((letter_data & DIG_E)>>4))<<digit_idx;   	 
}
#else
u8 _code lcd_disbuf_offset[4] ={7,5,3,1};
void align_lcd_disp_buff(u8 offset,u8 letter_data)
{
	u8 digit_idx=offset;
	
	digit_idx= lcd_disbuf_offset[offset];

	lcd_buff[4] &= ~(0x0001<<digit_idx);
	lcd_buff[3] &= ~(0x0003<<digit_idx);
	lcd_buff[2] &= ~(0x0003<<digit_idx);
	lcd_buff[1] &= ~(0x0003<<digit_idx);

       lcd_buff[4] |= ((letter_data & DIG_A))<<digit_idx;
       lcd_buff[3] |= (((letter_data & DIG_B)>>1)|((letter_data & DIG_F)>>4))<<digit_idx;
       lcd_buff[2] |= (((letter_data & DIG_C)>>2)|((letter_data & DIG_G)>>5))<<digit_idx;
       lcd_buff[1] |= (((letter_data & DIG_D)>>3)|((letter_data & DIG_E)>>3))<<digit_idx;   	 
}
#endif
void clr_lcd_digit(u8 lcd_num)
{
	align_lcd_disp_buff(lcd_num,0x00);
}
void disp_putchar(u8 chardata,u8 loc)
{
    //loc += 0;
    if ((chardata < ' ') || (chardata > '~'))
    {
        return;
    }

    if ((chardata >= '0') && (chardata <= '9'))
    {
          align_lcd_disp_buff(loc,LCD_NUMBER[chardata - '0']);
    }
    else if ((chardata >= 'a') && (chardata <= 'z'))
    {
         align_lcd_disp_buff(loc,LCD_SMALL_LETTER[chardata - 'a']);
    }
    else if ((chardata >= 'A') && (chardata <= 'Z'))
    {
         align_lcd_disp_buff(loc,LCD_LARGE_LETTER[chardata - 'A']);
    }
    else if (chardata == ':')
    {
         disp_icon(ICON_COL);
    }
    else if (chardata == ' ')
    {
    }
    else if (chardata == '-')
    {
    }
}
//extern void Bat_icon_chk(void);
#if defined(BK_DISP_LCD_DRV)
void disp_scan(void)
{
    static xd_u8 cnt = 0;
    xd_u8 temp;
    static bool flash;

    custom_buf_update();

     lcd_flash_timer++;
    if (lcd_flash_timer == 220)
    {
        lcd_flash_timer = 0;
        flash = !flash;
    }
    if (flash)
    {
        disp_clr_icon(lcd_flash_icon);   
    }
    else
    {
        disp_icon(lcd_flash_icon); 
    }

    temp = cnt>>1;
    close_com(temp);
    if(cnt & 0x01){
	  seg07_port(lcd_buff[temp]);
	  clr_com(temp);
    }
    else
   {                            
	  seg07_port(~lcd_buff[temp]);
	  set_com(temp);
   }
	
   cnt++;
   if(cnt>9)cnt = 0;
}
#elif defined(SEG_LCD_4COM_10SEG_DRV)
void disp_scan(void)
{
    static xd_u8 cnt = 0;
    xd_u8 temp;
    static bool flash;

    custom_buf_update();

#if 0
	init_disp_buf();
	disp_putchar('0',0);
	disp_putchar('1',1);
	disp_putchar('2',2);
	disp_putchar('3',3);
#endif	

    TRADEMARK_ICON |=TRADEMARK_MASK;

    lcd_flash_timer++;
    if (lcd_flash_timer == 220)
    {
        lcd_flash_timer = 0;
        flash = !flash;
    }
    if (flash)
    {
        disp_clr_icon(lcd_flash_icon);   
    }
    else
    {
        disp_icon(lcd_flash_icon); 
    }

    temp = cnt>>1;
    close_com(temp);
    if(cnt & 0x01){
	  seg07_port(lcd_buff[temp]);
	  seg8_port( ( ((lcd_buff[temp]&0x0100)>0)?1:0 ) );
	  seg9_port( ( ((lcd_buff[temp]&0x0200)>0)?1:0 ) );
	  clr_com(temp);
    }
    else
   {                            
	  seg07_port(~lcd_buff[temp]);
	  seg8_port(( ((lcd_buff[temp]&0x0100)>0)?0:1 ));
	  seg9_port(( ((lcd_buff[temp]&0x0200)>0)?0:1 ));
	  set_com(temp);
   }

   cnt++;
   if(cnt>7)cnt = 0;
}
#else
void disp_scan(void)
{
    static xd_u8 cnt = 0;
    xd_u8 temp;
    static bool flash;

    custom_buf_update();

    TRADEMARK_ICON |=TRADEMARK_MASK;
	
    lcd_flash_timer++;
    if (lcd_flash_timer == 220)
    {
        lcd_flash_timer = 0;
        flash = !flash;
    }
    if (flash)
    {
        disp_clr_icon(lcd_flash_icon);   
    }
    else
    {
        disp_icon(lcd_flash_icon); 
    }

    temp = cnt>>1;
    close_com(temp);
    if(cnt & 0x01){
	  seg07_port(lcd_buff[temp]);
	  seg8_port( ( ((lcd_buff[temp]&0x0100)>0)?1:0 ) );
	  clr_com(temp);
    }
    else
   {                            
	  seg07_port(~lcd_buff[temp]);
	  seg8_port(( ((lcd_buff[temp]&0x0100)>0)?0:1 ));
	  set_com(temp);
   }

   cnt++;
   if(cnt>9)cnt = 0;
}
#endif

#endif


