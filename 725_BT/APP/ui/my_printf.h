/*--------------------------------------------------------------------------*/
/**@file   my_printf.h
   @brief    ��ʽ�����ͷ�ļ�
   @details
   @author
   @date   2010-12-13
   @note
*/
/*----------------------------------------------------------------------------*/
#ifndef   _MY_PRINTF_H_
#define  _MY_PRINTF_H_

#include "config.h"
#ifdef __C51__
#include "stdarg.h"
void printf_char(u8 chardata,u8 loc);
void printf_cmd(u8 cmd);
void printf_str(u8 *str,u8 loc);
void printf_num(u16 dat,u8 loc,u8 len);
//void my_printf(u8 _code * fmt, ...) compact;

#endif

/*
%:����ת����:
x: ����X����
y: ����Y����
m: ��ʾͼƬ,ͨ������
e: ��ʾ�ַ���С�ӱ�
n: ��ʾ�ַ���С����
a: ��ʾС����
k: ��ʾС���֣����Ҽ��ϻ���
p: ��ʾƵ��
f: LED �� ��������,ָ��ICON��������
c: ���������ʾ����
*/

#endif
