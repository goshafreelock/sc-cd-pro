/*--------------------------------------------------------------------------*/
/**@file     iic_drv.h
   @brief    iic_drvÍ·ÎÄ¼þ
   @details
   @author
   @date   2012-9-13
   @note
*/
/*----------------------------------------------------------------------------*/

#ifndef _IIC_DRV_H_
#define _IIC_DRV_H_


#ifdef __C51__
#include "AC209N.h"
#include "typedef.h"
#include "uart.h"

#if 1
#define iic_drv_data_out()    	P2DIR &= ~(BIT(2));//P0PU |= (1<<2)
#define iic_drv_data_in()     	P2DIR |= BIT(2);P0PU |= BIT(2)
#define iic_drv_data_r()      	P22
#define iic_drv_data_h()      	P22 = 1
#define iic_drv_data_l()      	P22 = 0

#define iic_drv_clk_out()    	P2DIR &= ~(BIT(0));P2PU |= BIT(0)
#define iic_drv_clk_h()      	P20 = 1
#define iic_drv_clk_l()      		P20= 0
#endif
#define iic_drv_delay()      Delay(12)


void iic_drv_init_io(void);
void iic_drv_start(void);
void iic_drv_stop(void);
bool drv_rev_ack(void);
void drv_send_ack(u8 flag);
u8 iic_drv_revbyte_io( void ) large;
u8 iic_drv_revbyte( u8 para ) large;
void iic_drv_sendbyte_io(u8 byteI2C) large;
void iic_drv_sendbyte(u8 byte);

#endif


#endif
