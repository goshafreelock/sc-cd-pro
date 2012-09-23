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


#define MASTER_SEND_CMD	0x55
#define MASTER_READ_CMD	0xAA
enum {

	NOP_CMD,
	STOP_CMD,
	PLAY_RESUME_CMD,
	PAUSE_CMD,
	NONE_CMD,
	NEXT_FILE_CMD,
	PREV_FILE_CMD,
	FAST_F_CMD,
	FAST_R_CMD,
	FFR_OFF_CMD,
	REP_RAND_OFF_CMD,
	REP_RANDOM_CMD,
	REP_OFF_CMD,
	REP_ONE_CMD,
	REP_ALL_CMD,
	MEM_CMD,
	INTRO_ON_CMD,
	INTRO_OFF_CMD,
	DOOR_IN_CMD,
	DOOR_OUT_CMD,
	DOOR_CTRL_CMD,

	NUM_0_CMD=0x20,
	NUM_1_CMD,
	NUM_2_CMD,
	NUM_3_CMD,
	NUM_4_CMD,
	NUM_5_CMD,
	NUM_6_CMD,
	NUM_7_CMD,
	NUM_8_CMD,
	NUM_9_CMD,
	NUM_10_CMD,

	REP_A_CMD,
	REP_B_CMD,
};

#ifdef IIC_MCU_PORT_USE_P21_P22
#define iic_drv_data_out()    	P2DIR &= ~(BIT(2));P2PU |= BIT(2);P22 = 1
#define iic_drv_data_in()     		P2DIR |= BIT(2);P2PU |= BIT(2)
#define iic_drv_data_r()      		P22
#define iic_drv_data_h()      		P22 = 1
#define iic_drv_data_l()      		P22 = 0

#define iic_drv_clk_out()    		P2DIR &= ~(BIT(1));P2PU |= BIT(1)
#define iic_drv_clk_h()      		P21 = 1
#define iic_drv_clk_l()      			P21= 0
#elif defined(IIC_MCU_PORT_USE_P02_P34)
#define iic_drv_data_out()    	P3DIR &= ~(BIT(4));P3PU |= BIT(4);P34= 1
#define iic_drv_data_in()     	P3DIR |= BIT(4);P3PU |= BIT(4)
#define iic_drv_data_r()      	P34
#define iic_drv_data_h()      	P34= 1
#define iic_drv_data_l()      	P34 = 0

#define iic_drv_clk_out()    	P0DIR &= ~(BIT(2));P0PU |= BIT(2)
#define iic_drv_clk_h()      	P02 = 1
#define iic_drv_clk_l()      		P02= 0
#else
#define iic_drv_data_out()    	P0DIR &= ~(BIT(1));P0PU |= BIT(1);P01= 1
#define iic_drv_data_in()     	P0DIR |= BIT(1);P0PU |= BIT(1)
#define iic_drv_data_r()      	P01
#define iic_drv_data_h()      	P01= 1
#define iic_drv_data_l()      	P01 = 0

#define iic_drv_clk_out()    	P0DIR &= ~(BIT(0));P0PU |= BIT(0)
#define iic_drv_clk_h()      	P00 = 1
#define iic_drv_clk_l()      		P00= 0
#endif
#define iic_drv_delay()      Delay(22)


void iic_drv_init_io(void);
void iic_drv_start(void);
void iic_drv_stop(void);
bool drv_rev_ack(void);
void drv_send_ack(bool flag);
u8 iic_drv_revbyte_io( void ) large;
u8 iic_drv_revbyte(bool flag) large;
void iic_drv_sendbyte_io(u8 byteI2C) large;
bool iic_drv_sendbyte(u8 byte);

#endif


#endif
