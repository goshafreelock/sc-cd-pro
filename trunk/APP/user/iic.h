/*--------------------------------------------------------------------------*/
/**@file     iic.h
   @brief    IIC头文件
   @details
   @author
   @date   2010-12-13
   @note
*/
/*----------------------------------------------------------------------------*/

#ifndef _IIC_H_
#define _IIC_H_

#define USE_EEPROM     0   // 使用EEPROM记忆
#define USE_RTCRAM     1   // 使用RTC_RAM记忆
#define USE_DEVICE     2   // 使用播放设备记忆
#define USE_RTC_EEPROM	3

#define MEMORY_STYLE    USE_RTC_EEPROM      //记忆方式选择
#ifdef __C51__
#include "AC209N.h"
#include "typedef.h"
#include "uart.h"
#include "gpio_if.h"


//#define iic_data_out()    P2DIR &= ~(1<<7);P2PU |= (1<<7)
//#define iic_data_in()     P2DIR |= (1<<7);P2PU |= (1<<7)
//#define iic_data_r()      (P2 & (1<<7))
//#define iic_data_h()      P2 |= (1<<7)
//#define iic_data_l()      P2 &= ~(1<<7)
//
//#define iic_clk_out()    P2DIR &= ~(1<<6);P2PU |= (1<<6)
//#define iic_clk_h()      P2 |= (1<<6)
//#define iic_clk_l()      P2 &= ~(1<<6)
#if 0
#define iic_data_out()    P0DIR &= ~(1<<2);//P0PU |= (1<<2)
#define iic_data_in()     P0DIR |= (1<<2);P0PU |= (1<<2)
#define iic_data_r()      P02
#define iic_data_h()      P02 = 1
#define iic_data_l()      P02 = 0

#define iic_clk_out()    P0DIR &= ~(1<<3);P0PU |= (1<<3)
#define iic_clk_h()      P03 = 1
#define iic_clk_l()      P03 = 0
#endif
#define iic_delay()      Delay(12)
/*
#if USE_RTC_RAM
extern u8 read_rtc_ram(u8 addr);
extern void write_rtc_ram(u8 addr,u8 dat);
#define  read_info(n)    read_rtc_ram(n)
#define  write_info(n,m)      write_rtc_ram(n,m)

#else

#define  read_info(n)    read_eerom(n)
#define  write_info(n,m)      write_eerom(n,m)
#endif
*/

extern u8 read_info(u8 addr);
extern void write_info(u8 addr,u8 dat);
extern u8 read_rtc_ram(u8 addr);
extern void write_rtc_ram(u8 addr,u8 dat);

void iic_init_io(void);
void iic_start(void);
void iic_stop(void);
bool r_ack(void);
void s_ack(u8 flag);
u8 iic_revbyte_io( void ) large;
u8 iic_revbyte( u8 para ) large;
void iic_sendbyte_io(u8 byteI2C) large;
void iic_sendbyte(u8 byte);

void  iic_write(u8 chip_id,u8 iic_addr,u8 *iic_dat,u8 n) large;
void iic_readn(u8 chip_id,u8 iic_addr,u8 *iic_dat,u8 n) large;


u8 read_eerom(u8 iic_addr);
void write_eerom(u8 addr,u8 dat);
#endif

#define IIC_DELAY_DEFAULT 30



#define MEM_ACTIVE_DEV     	0
#define MEM_EQ_MODE        	1
#define MEM_PLAY_MODE      	2
#define MEM_VOL            		3
#define MEM_SYSMODE        	4

#define MEM_ALM_SW		   	5
#define MEM_ALM_HOUR       	6
#define MEM_ALM_MIN        	7


#define MEM_PRESET_REG       	8
#define MEM_BAND_SEL          	9

#define MEM_FREQ_BASE         	10

#define MEM_FM_ALL_CH     	15
#define MEM_AM_ALL_CH     	16

#define FM_CH_OFFSET		20

#define AM_CH_OFFSET	70

#define MEM_SD_PLAYPOINT_INFO  41
#define MEM_USB_PLAYPOINT_INFO 43


#define MEM_USB_FRAME_CNT	45
#define MEM_USB_NEXT_FRAME  49
#define MEM_SD_FRAME_CNT	53
#define MEM_SD_NEXT_FRAME   57	

#define MEM_APP_VOL         62
#define MEM_USB_VOL         63
#define MEM_EEPROM          64	

#define IIC_VERIFY_BYTE_ADDR	100

#endif
