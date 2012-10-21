/*--------------------------------------------------------------------------*/
/**@file     eq.h
   @brief    音效头文件
   @details
   @author
   @date   2010-12-13
   @note
*/
/*----------------------------------------------------------------------------*/
#ifndef _EQ_H_
#define _EQ_H_

#ifdef __C51__
#include "config.h"

enum
{
    NORMAL = 0,
    POP,
    ROCK,
    JAZZ,
    CLASSIC,
};

void set_eq(u8 n);
#endif

#endif