/*--------------------------------------------------------------------------*/
/**@file    uart.h
   @brief   串口打印头文件
   @details
   @author
   @date   2010-12-13
   @note
*/
/*----------------------------------------------------------------------------*/
#include "Custom_config.h"

#ifndef _UART_
#define _UART_

#ifdef __C51__
#include "AC209N.h"
#include "intrins.h"


#include "typedef.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "entry.h"

void uartInit(void);
#if defined(BLUE_TOOTH_UART_FUNC)
void blue_tooth_uart_init();
void blue_tooth_uart_release();
#endif
void putbyte(u8 c);
char putchar (char c);
void sys_printf(char *str);
void printf_u8(u8 dat);
void printf_u16(u16 dat,u8 c);
void sys_printf(char *str);
#endif
#endif
