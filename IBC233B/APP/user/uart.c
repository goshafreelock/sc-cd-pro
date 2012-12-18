/*--------------------------------------------------------------------------*/
/**@file    uart.c
   @brief   ���ڴ�ӡ����
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#include "Custom_config.h"

#include "uart.h"
#include"clock.h"

/*----------------------------------------------------------------------------*/
/**@brief  ���ڳ�ʼ������
   @param  ��
   @return ��
   @note   void uartInit(void)
*/
/*----------------------------------------------------------------------------*/
void uartInit(void)
{
#if defined (_USE_UART_P0_)||defined(BLUE_TOOTH_UART_FUNC)
    UTBAUD = 0x9c;
    P0DIR &= ~(1<<6);
    P0DIR |= (1<<7);
    UTCON = 0x09;							/* enable uart */
#elif defined(_USE_UART_P2_)
    UTBAUD = (48000000/(8 * 115200)) - 1;	//25;//0x37;					/* 25: cpu 24M, 115200bps*/	/*77 for 384*/ /*0x137 for 9600*/
    P2DIR &= 0xef;							/* P06 for transmit */
    P2DIR |= 0x20;							/* P07 for receive */
    UTCON = 0x41;							/* enable uart */
#else
    UTCON = 0x00;							/* disable uart */
#endif
}
#if defined(BLUE_TOOTH_UART_FUNC)

void blue_tooth_uart_init()
{
	sysclock_div2(1);
	uartInit();
	delay_10ms(10);
}

bool bt_frame_rev_finished=0;
u8  uart_rev=0;
xd_u8 rev_phase=0,rev_length=0;
extern xd_u8 rev_cmd[7];

void uart_isr()  interrupt  7
{
    //static u8  Uart_data_temp=0 ;

    _push_(DPCON);
    _push_(DP1L);
    _push_(DP1H);
    DPCON = 0x0;

    //uart_rev = UTBUF;

   //UTBUF=	uart_rev;
   //while (!(UTSTA & 0x80));

#if 1
    	uart_rev = UTBUF;

	if((uart_rev==0xAA)&&(rev_phase==0)){

		rev_cmd[0]=0xAA;
		rev_phase=1;
	}
	else if((uart_rev==0x00)&&(rev_phase==1)){

		rev_cmd[1]=0x00;
		rev_phase=2;
	}
	else if((rev_phase==2)){

		rev_cmd[2]=uart_rev;
		rev_length=0;
		rev_phase=3;
	}	
	else if((rev_phase==3)){

		if(rev_length<(rev_cmd[2]+1)){
			rev_cmd[2+rev_length++]=uart_rev;
		}
		else{
			rev_phase=0;
			bt_frame_rev_finished=1;
		}
	}
	else{
		rev_length=0;
		rev_phase=0;
	}
#endif

    UTSTA &= ~BIT(6);
    _pop_(DP1H);
    _pop_(DP1L);
    _pop_(DPCON);
}
#endif
/*----------------------------------------------------------------------------*/
/**@brief  ���ڴ�ӡ��������������
   @param  c��һ��8λ����ֵ
   @return ��
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
/**@brief  ���ڴ�ӡ������������
   @param  c��һ��8λ����ֵ
   @return ��
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

