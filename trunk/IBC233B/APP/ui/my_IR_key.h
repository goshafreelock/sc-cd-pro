/*--------------------------------------------------------------------------*/
/**@file   my_IR_key.h
   @brief  按键消息扫描头文件
   @details
   @author
   @date   2010-12-13
   @note
*/
/*----------------------------------------------------------------------------*/
#ifndef	_MY_IR_KEY_H_
#define _MY_IR_KEY_H_

/* FILL BELOW TO ASSIGN CODE TABLE*/

#define  My_IR_KEY_0			0x16
#define  My_IR_KEY_1			0x0C
#define  My_IR_KEY_2			0x18
#define  My_IR_KEY_3			0x5E
#define  My_IR_KEY_4			0x08
#define  My_IR_KEY_5			0x1C
#define  My_IR_KEY_6			0x5A
#define  My_IR_KEY_7			0x42
#define  My_IR_KEY_8			0x52
#define  My_IR_KEY_9			0x4A
#define  My_IR_KEY_100			0x19

#define  My_IR_KEY_MODE		0x46
#define  My_IR_KEY_PLAY			0x43
#define  My_IR_KEY_PLAY_M		0x0D
#define  My_IR_KEY_VOL_UP		0x15
#define  My_IR_KEY_VOL_DN		0x07	
#define  My_IR_KEY_PREV			0x44
#define  My_IR_KEY_NEXT			0x40
#define  My_IR_KEY_EQ_UP		0x09
#define  My_IR_KEY_EQ_DN		0xFF
#define  My_IR_KEY_CHN_UP		0x47
#define  My_IR_KEY_CHN_DN		0x45

#define My_IR_KEY_RETURN		0xFF
#define My_IR_KEY_NONE 			0xFF

#define My_IR_KEY_TATOL		23


#define USER_CODE	0xFF00

typedef struct{
	u8	IR_Key_Value;
	u8	APP_Key;
}MY_IR_KEY_STRUCT;

#endif/*_MY_IR_KEY_H_*/


