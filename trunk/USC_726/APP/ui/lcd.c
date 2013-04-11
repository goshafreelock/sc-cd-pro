/*--------------------------------------------------------------------------*/
/**@file     lcd.c
   @brief    点阵LCD屏底层驱动
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#include "config.h"
#include "lcd.h"
#include "disp.h"

_xdata u8 LCDBuff[LCDPAGE][LCDCOLUMN]; ///<显存
u8 currPage;                ///<当前PAGE
u8 updata_page_len;         ///跟新的PAGE数目
coordinate LCDsite;         ///<显示坐标
volatile bool lcd_busy;     ///<LCD硬件忙标记

u8 _code lcd_init_table[] =
{
    LCD_BIAS_SET | 0x00,   //lcd bias setting
    LCD_ADC_SEL,           //0XA0 ADC
    LCD_COMMON_MODE,       //0xc8 common output mode
    (LCD_V0_RATIO | 0x03),   //Resistor Ratio
    LCD_ELE_VOLUME,        //0X81 /electronic Volume
    0x44,                  //contrast
    LCD_POWER_MODE,        //0x2f lcd power control
    LCD_START_LINE | 0,    //0x40
    LCD_ON
};   ///<lcd 初始化命令表

/*----------------------------------------------------------------------------*/
/**@brief    清全屏，只清BUFF但不更新
   @param
   @return
   @note     void DispClear(void)
*/
/*----------------------------------------------------------------------------*/
void clear_lcd(void)
{
    while (lcd_busy);
    my_memset(&LCDBuff[0][0], 0x0, LCDPAGE*LCDCOLUMN);
}
/*----------------------------------------------------------------------------*/
/**@brief    清屏的指定区域，只清BUFF但不更新
   @param    p1:起始 page
   @param    p2:结束page
   @return
   @note     void LCDClearArea(u8 p1,u8 p2)
*/
/*----------------------------------------------------------------------------*/
void clear_lcd_area(u8 p1,u8 p2)
{
    while (lcd_busy);
    for (;p1 <= p2;p1++)
    {
        my_memset(&LCDBuff[p1][0], 0, LCDCOLUMN);
    }
}
/*----------------------------------------------------------------------------*/
/**@brief    向LCD屏发送一条命令
   @param    cmd: 命令
   @return
   @note     void LCDSendCmd(u8 cmd)
*/
/*----------------------------------------------------------------------------*/
void LCDSendCmd(u8 cmd)
{
    LCD_A0_L;    //LCD_COMMAND
    _nop_();
    _nop_();
    _nop_();
    LCD_CS_L;
    _nop_();
    _nop_();
    _nop_();
    SPIBUF = cmd;
    while (!(SPICON & 0x80)); //等待发送完成
    SPICON &= ~0x80;
    LCD_CS_H;
}

/*----------------------------------------------------------------------------*/
/**@brief   显示初始化函数
   @param	无
   @return	无
   @note    void DispInit(void)
*/
/*----------------------------------------------------------------------------*/
void init_disp(void)
{
    u8 i;

    LCDPORT_OUT();
    LCDPORT_H();
    LCD_RES_L;
    Delay(10);
    LCD_RES_H;
    Delay(10);
    SPIBAUD = 0x0;
    SPICON = 0x0B;

    for (i = 0; i < 9; i++)
    {
        LCDSendCmd(lcd_init_table[i]);
        Delay(10);
    }

    lcd_busy = 0;
    Disp_Con(DISP_POWER_UP);
    for (i = 0; i <= LCDPAGE; i++)
    {
        spi_dmapro();
        while (!(SPIDMACON & 0x80));
    }
}
/*----------------------------------------------------------------------------*/
/**@brief    设置显示的起始坐标
   @param     x:列坐标
   @param     y:行坐标
   @return	  无
   @note      void LCD_SET(u8 y,u8 x)
*/
/*----------------------------------------------------------------------------*/
void LCD_SET(u8 y,u8 x)
{
    LCDSendCmd(LCD_COLUMN_HIGH | x>>4);
    LCDSendCmd(LCD_COLUMN_LOW | (0x0f & x));
    LCDSendCmd(LCD_PAGE_ADDR | y);
}


/*----------------------------------------------------------------------------*/
/**@brief   更新显存的内容到LCD
   @param   start_page ：更新开始的起始Page
   @param   page_len ：更新的Page数
   @return	无
   @note    void updataLCD(u8 start_page, u8 page_len)
*/
/*----------------------------------------------------------------------------*/
void draw_lcd(u8 start_page, u8 page_len)
{
    while (lcd_busy);
    lcd_busy = 1;
    currPage = start_page;
    updata_page_len = page_len;
    SPIDMAISREN();
}
/*----------------------------------------------------------------------------*/
/**@brief   关机时的清屏函数
   @param   无
   @return	无
   @note    void lcd_power_down(void)
*/
/*----------------------------------------------------------------------------*/
void lcd_power_down(void)
{
    clear_lcd();
    draw_lcd(0,4);
    while (lcd_busy);
}

