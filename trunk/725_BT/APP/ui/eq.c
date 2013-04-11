/*--------------------------------------------------------------------------*/
/**@file     eq.c
   @brief    ��Ч����ģ��
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#include "eq.h"

extern void write_dsp(u8, u8 , u8 );
extern void set_dsp_eq(u8 * eq_table);
extern void close_dsp_eq(void);

u8 _code dsp_eq_table[4][5] =
{
    {0, 0, 0 , 25, 50},
    {0, 0, 50 , 0, 0},
    {0, 50, 0 , 0, 0},
    {50, 50, 0 , 0, 0}
};    ///<dsp��Ч��

u8 _code EQTableCode[5][11] =
{
    {0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x00}, //NORMAL
    {0x0f,0x0f,0x08,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x0f}, //POP
	{0x01,0x01,0x08,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f}, //POP
	{0x0f,0x0f,0x08,0x01,0x04,0x08,0x0a,0x0c,0x0f,0x0f,0x0f}, //POP
	{0x01,0x01,0x08,0x0f,0x0f,0x08,0x01,0x01,0x01,0x01,0x0f}, //POP

};    ///<DECODER��Ч��

/*----------------------------------------------------------------------------*/
/**@brief   ����DSP��Ч
   @param   * eq_table �� ��Ч����ָ��
   @return  ��
   @note    void set_dsp_eq(u8 * eq_table)
*/
/*----------------------------------------------------------------------------*/
void set_dsp_eq(u8 * eq_table)
{
    u8 i;
    write_dsp(0x7f, 0xff, 0x20);
    for (i = 0; i < 5; i++)
    {
        write_dsp(eq_table[i], 0, 0x20);
    }
    write_dsp(0x7f, 0xfe, 0x20);
}



/*----------------------------------------------------------------------------*/
/**@brief   ����decode ��Ч
   @param   n : ��Чģʽ
   @return  ��
   @note    void EQ_SET(u8 n)
*/
/*----------------------------------------------------------------------------*/
void decode_eq(u8 n)
{

    if (NORMAL == n)
    {
        set_decode_eq(0);
    }
    else
    {
        set_decode_eq(EQTableCode[n]);
    }
}




/*----------------------------------------------------------------------------*/
/**@brief   ��Ч���ƽӿں���
   @param   n : ��Чģʽ
   @return  ��
   @note    void set_eq(u8 n)
*/
/*----------------------------------------------------------------------------*/
void set_eq(u8 n)
{
#if EQ_WORK_USE == DECODE_EQ
    decode_eq(n);
#else
    if (NORMAL == n)
    {
        close_dsp_eq();
    }
    else
    {
        set_dsp_eq(dsp_eq_table[n-1]);
    }
#endif
}



