/*--------------------------------------------------------------------------*/
/**@file     lcd.h
   @brief    点阵LCD屏底层驱动头
   @details
   @author
   @date   2010-12-13
   @note
*/
/*----------------------------------------------------------------------------*/

#ifndef _LCD_H_
#define _LCD_H_

#ifdef __C51__
#include "AC209N.h"
#include "typedef.h"
#include "uart.h"
#include "main.h"
#include "gpio_if.h"



void spi_dmapro(void);

typedef struct
{
    u8 x;         //列位置
    u8 y;         //行位置
}coordinate;
#if 0
#define LCDPORT_OUT()  LCD_DATA_OUT;LCD_CLK_OUT;LCD_A0_OUT;LCD_RES_OUT;LCD_CS_OUT;//P1DIR &= ~( BIT(7)|BIT(6) );P3DIR &= ~( BIT(0) | BIT(1) | BIT(2) ) 
#define LCDPORT_H()    P1 |= ( BIT(7)|BIT(6) );LCD_A0_H;  LCD_RES_H;	LCD_CS_H;
#define LCD_DATA_OUT P1DIR &= ~BIT(7);P1PU |= BIT(7)//7 
#define	LCD_CLK_OUT	 P1DIR &= ~BIT(6);P1PU |= BIT(6)//6

#define	LCD_A0_OUT	  P0DIR &= ~BIT(5);P0PU |= BIT(5)
#define	LCD_A0_L	  P05 = 0      //P0 &= ~BIT(5)
#define	LCD_A0_H	  P05 = 1      //P0 |= BIT(5)

#define LCD_RES_OUT	  P3DIR &= ~BIT(2);P3PU |= BIT(2)
#define LCD_RES_L	  P32 = 0      //P3 &= ~BIT(2)
#define LCD_RES_H	  P32 = 1      //P3 |= BIT(2)

#define LCD_CS_OUT	  P0DIR &= ~BIT(4);P0PU |= BIT(4)
#define LCD_CS_L	  P04 = 0      //P0 &= ~BIT(4)
#define LCD_CS_H	  P04 = 1      //P0 |= BIT(4)

#define SPIDMAISRDIS()  IE1 &= ~BIT(3)
#define SPIDMAISREN()	IP1 |= BIT(3), IE1 |= BIT(3)

#endif

void init_disp(void);
void draw_lcd(u8 start_page, u8 page_len);
void clear_lcd(void);
void clear_lcd_area(u8 y1,u8 y2);
#endif

#ifdef __AX51__
#define LCD_CS_H	  setb P0.4
#define LCD_CS_L	  clr  P0.4
#define	LCD_A0_L	  clr  P0.5      //P0 &= ~BIT(5)
#define	LCD_A0_H	  setb P0.5      //P0 |= BIT(5)

SPIDMAISRDIS  macro
anl IE1, #~(1 << 3)
endm

#endif

#define  LCDPAGE  4
#define  LCDCOLUMN   96

#define LCD_COLUMN_LOW          0x00
#define LCD_COLUMN_HIGH         0x10
#define LCD_V0_RATIO            0x20
#define LCD_POWER_MODE          0x2f
#define LCD_START_LINE          0x40
#define LCD_ELE_VOLUME          0x81
#define LCD_STATIC_IND_OFF      0xac
#define LCD_ON                  0xaf
#define LCD_OFF                 0xae
#define LCD_ALL_ON              0xa5
#define LCD_ALL_NORMAL          0xa4
#define LCD_STATIC_IND_ON       0xad
#define LCD_ADC_SEL             0xa0
#define LCD_DISP_MODE           0xa6
#define LCD_BIAS_SET            0xa2
#define LCD_PAGE_ADDR           0xb0
#define LCD_COMMON_MODE         0xc8
#define LCD_PAGE_BLINK          0xd5
#define LCD_DRIV_MODE           0xd2
#define LCD_NOP                 0xe3
#define LCD_OSC_SET             0xe4
#define LCD_END                 0xee
#define LCD_SOFT_RST            0xe2


#endif
