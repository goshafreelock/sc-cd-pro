/*--------------------------------------------------------------------------*/
/**@file     lcd.c
   @brief    ����LCD���ײ�����
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#include "config.h"
#include "lcd.h"
#include "disp.h"

_xdata u8 LCDBuff[LCDPAGE][LCDCOLUMN]; ///<�Դ�
u8 currPage;                ///<��ǰPAGE
u8 updata_page_len;         ///���µ�PAGE��Ŀ
coordinate LCDsite;         ///<��ʾ����
volatile bool lcd_busy;     ///<LCDӲ��æ���

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
};   ///<lcd ��ʼ�������

/*----------------------------------------------------------------------------*/
/**@brief    ��ȫ����ֻ��BUFF��������
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
/**@brief    ������ָ������ֻ��BUFF��������
   @param    p1:��ʼ page
   @param    p2:����page
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
/**@brief    ��LCD������һ������
   @param    cmd: ����
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
    while (!(SPICON & 0x80)); //�ȴ��������
    SPICON &= ~0x80;
    LCD_CS_H;
}

/*----------------------------------------------------------------------------*/
/**@brief   ��ʾ��ʼ������
   @param	��
   @return	��
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
/**@brief    ������ʾ����ʼ����
   @param     x:������
   @param     y:������
   @return	  ��
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
/**@brief   �����Դ�����ݵ�LCD
   @param   start_page �����¿�ʼ����ʼPage
   @param   page_len �����µ�Page��
   @return	��
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
/**@brief   �ػ�ʱ����������
   @param   ��
   @return	��
   @note    void lcd_power_down(void)
*/
/*----------------------------------------------------------------------------*/
void lcd_power_down(void)
{
    clear_lcd();
    draw_lcd(0,4);
    while (lcd_busy);
}

