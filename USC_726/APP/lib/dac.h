/*--------------------------------------------------------------------------*/
/**@file   dac.h
   @brief  DAC配置
   @details
   @author
   @date   2010-12-13
   @note
*/
/*----------------------------------------------------------------------------*/
#ifndef _DAC_H_
#define _DAC_H_

#ifdef __C51__
#include "config.h"


/*----------------------------------------------------------------------------*/
/**@brief   开DAC
   @param   delay: DAC打开后的延时时间，建议时间为1秒以上，可以消除开DAC时的冲击声
   @return  无
   @note
*/
/*----------------------------------------------------------------------------*/
void dac_on(void);

/*----------------------------------------------------------------------------*/
/**@brief   关DAC
   @param   无
   @return  无
   @note
*/
/*----------------------------------------------------------------------------*/
void dac_off(void);

/*----------------------------------------------------------------------------*/
/**@brief   DAC输出高阻
   @param   无
   @return  无
   @note
*/
/*----------------------------------------------------------------------------*/
void dac_highz_output(void);




/*----------------------------------------------------------------------------*/
/**@brief   DAC_EN 0 1选择
   @param   flag；1：打开DAC_EN0、DAC_EN1；0：关闭DAC_EN0、DAC_EN1；
   @return  无
   @note	void dac_sw(u8 flag)
*/
/*----------------------------------------------------------------------------*/
void dac_sw(u8 flag);

/*----------------------------------------------------------------------------*/
/**@brief   设置主音量
   @param   vol:主音量值；mode：调节模式
	//mode
	//= 0, 改变DAC音量，不记忆新值 ,
	//= 1, 改变DAC音量，同时记忆新值,
	//= 2, 改变音量，使用music_vol,
   @return  当前主音量值
   @note	u8 main_vol_set(u8 vol, u8 mode)
*/
/*----------------------------------------------------------------------------*/
u8 main_vol_set(u8 vol, u8 mode);
/*----------------------------------------------------------------------------*/
/**@brief   DAC mute控制
   @param   mute：Mute控制位；mode：设置模式
   //mute :1 mute enable ,0 mute disable, mode: 1 change  main_vol_mute & dac, 0 chang main_vol_mute only
   @return  无
   @note	void dac_mute_control(u8 mute, u8 mode)
*/
/*----------------------------------------------------------------------------*/
void dac_mute_control(u8 mute, u8 mode);
/*----------------------------------------------------------------------------*/
/**@brief   DAC Mute 切换
   @param   无
   @return  无
   @note	bool dac_mute_toggle(void)
*/
/*----------------------------------------------------------------------------*/
bool dac_mute_toggle(void);
/*----------------------------------------------------------------------------*/
/**@brief   设置DAC Analog/Digital最大音量
   @param   max_analog：最大模拟音量；max_digital：最大数字音量
   @return  无
   @note	void set_max_vol(u8 max_analog, u8 max_digital)
*/
/*----------------------------------------------------------------------------*/
void set_max_vol(u8 max_analog, u8 max_digital);
/*----------------------------------------------------------------------------*/
/**@brief   DAC 初始化
   @param   无
   @return  无
   @note	void dac_init(void)
*/
/*----------------------------------------------------------------------------*/
void dac_init(void);
/*----------------------------------------------------------------------------*/
/**@brief   DAC 通道选择并调节当前音量
   @param   无
   @return  无
   @note	void dac_out_select(u8 sel)
*/
/*----------------------------------------------------------------------------*/
void dac_out_select(u8 sel);
/*----------------------------------------------------------------------------*/
/**@brief   设置DAC 模拟音量
   @param   vol：模拟音量值
   @return  无
   @note	void analog_vol_set(u8 vol)
*/
/*----------------------------------------------------------------------------*/
void analog_vol_set(u8 vol);
void dac_unmute(void);
void dac_mute(void);
/*
void dac_mute(void);
void dac_unmute(void);
u8 digital_vol_set(u8 vol);
void dsp_set_volume(u16 vol);
*/
extern u8 _idata max_analog_vol,max_digital_vol;
extern u8 _idata dac_channel;
#endif

#define  MAX_MAIN_VOL 		30
#define  DAC_DECODE         0
#define  DAC_AMUX0          1
#define  DAC_AMUX1          2

#define MAX_ANALOG_VOL		30		///<最大模拟音量
#define MAX_DIGITAL_VOL		30  	///<最大数字音量
#define DECODE_ANALOG_VOL_CUT	4

#define CHANGE_VOL_NO_MEM   0
#define CHANGE_VOL_MEM   	1
#define SET_USE_CURRENT_VOL 2
#define CHANGE_VOL_INC      3
#define CHANGE_VOL_DEC      4
#define CHANGE_VOL_ONLY     5

#define CHANGE_VOL_PEND     0x10
#define CHANGE_VOL_NO_PEND  0x0 

#endif
