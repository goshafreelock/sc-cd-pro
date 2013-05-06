/*--------------------------------------------------------------------------*/
/**@file     rda5807.h
   @brief    RDA5807����ͷ�ļ�
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#ifndef _RDA_5807_H_
#define _RDA_5807_H_

#include "config.h"

#ifdef __C51__
#include "iic.h"
#include "typedef.h"



bool init_RDA5807(void);
bool set_fre_RDA5807(u16 fre);
void RDA5807_PowerDown(void);
void RDA5807_mute(u8 dir);
bool RDA5807_Read_ID(void);
void RDA5807_setch(u8 db);

extern u8 _idata rda5807_dat[12];
extern u8 _xdata read_dat[10];
extern u8 _xdata fm_type;

#define RDA5807_WR_ADDRESS  0x20       ///<RDA5807 д��ַ
#define RDA5807_RD_ADDRESS  0x21       ///<RDA5807 ����ַ

#define RDA5807_RSSI  0x6

#define rda5807_stc() 	(read_dat[0] & (1 << 6))       ///<��Ƶ����
#define rda5807_true() 	(read_dat[2] & (1 << 0))      ///<�Ƿ���̨
#define rda5807_rssi() 	((read_dat[2] >> 1))          ///<�����ź�ǿ��
#define rda5807_st() 		(read_dat[0] & (1 << 2))          /// ST

#endif

#endif
