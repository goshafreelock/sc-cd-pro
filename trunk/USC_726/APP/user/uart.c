/*--------------------------------------------------------------------------*/
/**@file    uart.c
   @brief   串口打印函数
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#include "Custom_config.h"

#include "uart.h"

/*----------------------------------------------------------------------------*/
/**@brief  串口初始化函数
   @param  无
   @return 无
   @note   void uartInit(void)
*/
/*----------------------------------------------------------------------------*/
void uartInit(void)
{

    UTBAUD = (48000000/(8 * 115200)) - 1;	//25;//0x37;					/* 25: cpu 24M, 115200bps*/	/*77 for 384*/ /*0x137 for 9600*/
    UTCON = 0x01;
#if defined (_USE_UART_P0_)
    P0DIR &= ~(1<<6);
    P0DIR |= (1<<7);
    UTCON = 0x41;							/* enable uart */
#elif defined(_USE_UART_P2_)
    P2DIR &= 0xef;							/* P06 for transmit */
    P2DIR |= 0x20;							/* P07 for receive */
    UTCON = 0x41;							/* enable uart */
#else
    UTCON = 0x00;							/* disable uart */
#endif
}
/*----------------------------------------------------------------------------*/
/**@brief  串口打印函数，不处理换行
   @param  c：一个8位的数值
   @return 无
   @note   void putbyte(u8 c)
*/
/*----------------------------------------------------------------------------*/
#pragma disable
void putbyte(u8 c)
{
    UTBUF = c;
    while (!(UTSTA & 0x80))
    {
    }
}
/*----------------------------------------------------------------------------*/
/**@brief  串口打印函数，处理换行
   @param  c：一个8位的数值
   @return 无
   @note   char putchar (u8 c)
*/
/*----------------------------------------------------------------------------*/

char putchar (u8 c)
{
    if (c == '\n')
    {
        putbyte(0x0d);
        putbyte(0x0a);
    }
    else
    {
        putbyte(c);
    }

    return (c);
}
void sys_printf(char *str)
{
      printf(str);
      printf("\r\n"); 	  
}
void put_str(u8 *p)
{
    while(*p)
	{
	    putchar(*p);
		p++;
	}

}
void get_hex_data(u8 dat)
{
   u8 dat0;
   if(dat<0x0a)
   {
       dat0 = dat+'0';
   }
   else
   {
       dat0 = dat+'A'-10;
   }
   putchar(dat0);
}

void printf_u8(u8 dat)
{
    putchar(' ');
    get_hex_data(dat >> 4);
    get_hex_data(dat & 0xf);
    putchar(' ');
	//putchar('\n');
}

void printf_u16(u16 dat,u8 c)
{
    putchar(' ');
    get_hex_data(((u8 *)(&dat))[0] >> 4);
    get_hex_data(((u8 *)(&dat))[0] & 0xf);

    get_hex_data(((u8 *)(&dat))[1] >> 4);
    get_hex_data(((u8 *)(&dat))[1] & 0xf);
    putchar(c);
	//putchar('\n');
}
void printf_u32(u32 dat)
{
    putchar(' ');
    get_hex_data(((u8 *)(&dat))[0] >> 4);
    get_hex_data(((u8 *)(&dat))[0] & 0xf);

    get_hex_data(((u8 *)(&dat))[1] >> 4);
    get_hex_data(((u8 *)(&dat))[1] & 0xf);

    get_hex_data(((u8 *)(&dat))[2] >> 4);
    get_hex_data(((u8 *)(&dat))[2] & 0xf);

    get_hex_data(((u8 *)(&dat))[3] >> 4);
    get_hex_data(((u8 *)(&dat))[3] & 0xf);
    putchar(' ');
}
void printf_buf(u8 _xdata *buf, u16 len)
{
    u16 i;

    printf(":%04X", (u16)buf);
    for (i = 0; i < len; i++)
    {
        if ((i % 16) == 0)
            printf("\n");
        printf("%02bX ", buf[i]);
    }

    printf("\n");
}

