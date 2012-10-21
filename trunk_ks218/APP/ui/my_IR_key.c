/*--------------------------------------------------------------------------*/
/**@file   my_IR_key.c
   @brief  红外按键码值映射表
   @details
   @author
   @date   2010-12-13
   @note
*/
/*----------------------------------------------------------------------------*/
#include "key.h"
#include "my_IR_key.h"

#if 1
_code MY_IR_KEY_STRUCT My_IRTab[]=       /*user code is 0xFF*/
{
	//{My_IR_KEY_TATOL,		My_IR_KEY_TATOL},	//4 总共键个数
	{My_IR_KEY_NONE,		NO_KEY},
	{My_IR_KEY_0,			INFO_0},
	{My_IR_KEY_1,			INFO_1},
	{My_IR_KEY_2,			INFO_2},
	{My_IR_KEY_3,			INFO_3},
	{My_IR_KEY_4,			INFO_4},
	{My_IR_KEY_5,			INFO_5},
	{My_IR_KEY_6,			INFO_6},
	{My_IR_KEY_7,			INFO_7},
	{My_IR_KEY_8,			INFO_8},
	{My_IR_KEY_9,			INFO_9},
#ifdef KEY_100_ENABLE	
	{My_IR_KEY_100,			INFO_100},
#endif	
	
	{My_IR_KEY_MODE,		INFO_EQ_DOWN},
	{My_IR_KEY_PLAY,			INFO_PLAY},
	{My_IR_KEY_PLAY_M,		INFO_PLAY_MODE},
	{My_IR_KEY_VOL_UP,		INFO_VOL_PLUS},
	{My_IR_KEY_VOL_DN,		INFO_VOL_MINUS},
	{My_IR_KEY_PREV,		INFO_PREV_FIL},
	{My_IR_KEY_NEXT,		INFO_NEXT_FIL},
	{My_IR_KEY_EQ_UP,		INFO_EQ_UP},
	//{My_IR_KEY_EQ_DN,		INFO_EQ_DOWN},
	{My_IR_KEY_CHN_UP,		INFO_MUTE},
	{My_IR_KEY_CHN_DN,		INFO_POWER},
};

u8 get_my_IR_key_MAX()
{
	return (u8)((sizeof(My_IRTab))/2);
}
#endif


