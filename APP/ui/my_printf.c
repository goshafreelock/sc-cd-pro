/*--------------------------------------------------------------------------*/
/**@file   my_printf.c
   @brief    ��ʽ�����
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#include "my_printf.h"
#include "config.h"
#include "led.h"
#include "lcdsegment.h"

#if 1//(NO_DISP != monitor)

extern void disp_putchar(u8 chardata,u8 loc);
u8 _xdata aisc[4];
void printf_char(u8 chardata,u8 loc)
{
	disp_putchar(chardata,loc);
}
/*----------------------------------------------------------------------------*/
/**@brief    ��ʾ�ַ���
   @param	 * str���ַ�����ַ ��loc:����λ��
   @return	 ��
   @note     void printf_str(u8 *str,u8 loc)
*/
/*----------------------------------------------------------------------------*/
void printf_str(u8 *str,u8 loc)
{
    u8 c;
    while (1)
    {
        c = *str++;
        if (c == '\0')
            break;
        disp_putchar(c,loc++);
    }
}

/*----------------------------------------------------------------------------*/
/**@brief    ����ת��
   @param	 dat��ת������
   @return	 ��
   @note     void number_ascii(u16 dat)
*/
/*----------------------------------------------------------------------------*/
void number_ascii(u16 dat)
{
    aisc[0] = dat%10 + '0';
    aisc[1] = (dat/10)%10 + '0';
    aisc[2] = (dat/100)%10 + '0';
    aisc[3] = (dat/1000)%10 + '0';
}

/*----------------------------------------------------------------------------*/
/**@brief    ������ʾ
   @param	 dat��ת�����֣�loc�����ֿ�ʼλ�� ��len�����ֳ���
   @return	 ��
   @note     void number_ascii(u16 dat)
*/
/*----------------------------------------------------------------------------*/
void printf_num(u16 dat,u8 loc,u8 len)
{
    if((!len) || (len>FIGURE_NUM))
    {
        return;
    }
    number_ascii(dat);
    do
    {
        disp_putchar(aisc[--len],loc++);
    }
    while(len);
}

#endif
