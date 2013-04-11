/*--------------------------------------------------------------------------*/
/**@file   dac.h
   @brief  DAC����
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
/**@brief   ��DAC
   @param   delay: DAC�򿪺����ʱʱ�䣬����ʱ��Ϊ1�����ϣ�����������DACʱ�ĳ����
   @return  ��
   @note
*/
/*----------------------------------------------------------------------------*/
void dac_on(void);

/*----------------------------------------------------------------------------*/
/**@brief   ��DAC
   @param   ��
   @return  ��
   @note
*/
/*----------------------------------------------------------------------------*/
void dac_off(void);

/*----------------------------------------------------------------------------*/
/**@brief   DAC�������
   @param   ��
   @return  ��
   @note
*/
/*----------------------------------------------------------------------------*/
void dac_highz_output(void);




/*----------------------------------------------------------------------------*/
/**@brief   DAC_EN 0 1ѡ��
   @param   flag��1����DAC_EN0��DAC_EN1��0���ر�DAC_EN0��DAC_EN1��
   @return  ��
   @note	void dac_sw(u8 flag)
*/
/*----------------------------------------------------------------------------*/
void dac_sw(u8 flag);

/*----------------------------------------------------------------------------*/
/**@brief   ����������
   @param   vol:������ֵ��mode������ģʽ
	//mode
	//= 0, �ı�DAC��������������ֵ ,
	//= 1, �ı�DAC������ͬʱ������ֵ,
	//= 2, �ı�������ʹ��music_vol,
   @return  ��ǰ������ֵ
   @note	u8 main_vol_set(u8 vol, u8 mode)
*/
/*----------------------------------------------------------------------------*/
u8 main_vol_set(u8 vol, u8 mode);
/*----------------------------------------------------------------------------*/
/**@brief   DAC mute����
   @param   mute��Mute����λ��mode������ģʽ
   //mute :1 mute enable ,0 mute disable, mode: 1 change  main_vol_mute & dac, 0 chang main_vol_mute only
   @return  ��
   @note	void dac_mute_control(u8 mute, u8 mode)
*/
/*----------------------------------------------------------------------------*/
void dac_mute_control(u8 mute, u8 mode);
/*----------------------------------------------------------------------------*/
/**@brief   DAC Mute �л�
   @param   ��
   @return  ��
   @note	bool dac_mute_toggle(void)
*/
/*----------------------------------------------------------------------------*/
bool dac_mute_toggle(void);
/*----------------------------------------------------------------------------*/
/**@brief   ����DAC Analog/Digital�������
   @param   max_analog�����ģ��������max_digital�������������
   @return  ��
   @note	void set_max_vol(u8 max_analog, u8 max_digital)
*/
/*----------------------------------------------------------------------------*/
void set_max_vol(u8 max_analog, u8 max_digital);
/*----------------------------------------------------------------------------*/
/**@brief   DAC ��ʼ��
   @param   ��
   @return  ��
   @note	void dac_init(void)
*/
/*----------------------------------------------------------------------------*/
void dac_init(void);
/*----------------------------------------------------------------------------*/
/**@brief   DAC ͨ��ѡ�񲢵��ڵ�ǰ����
   @param   ��
   @return  ��
   @note	void dac_out_select(u8 sel)
*/
/*----------------------------------------------------------------------------*/
void dac_out_select(u8 sel);
/*----------------------------------------------------------------------------*/
/**@brief   ����DAC ģ������
   @param   vol��ģ������ֵ
   @return  ��
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

#define MAX_ANALOG_VOL		30		///<���ģ������
#define MAX_DIGITAL_VOL		30  	///<�����������
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
