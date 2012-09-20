/*--------------------------------------------------------------------------*/
/**@file     iic_drv.c
   @brief    IOģ���IIC������
   @details
   @author
   @date   2012-9-13
   @note
*/
/*----------------------------------------------------------------------------*/
#include "Custom_config.h"

#ifdef USE_CD_MCU_MASTER_FUNC			

#include "iic_drv.h"

/*----------------------------------------------------------------------------*/
/**@brief   IIC��IO�ڳ�ʼ��
   @param   ��
   @return  ��
   @note    void iic_drv_init_io(void)
*/
/*----------------------------------------------------------------------------*/
void iic_drv_init_io(void)
{
    iic_drv_data_out();         //SDA���ó����
    iic_drv_clk_out();         //SCL���ó����
    iic_drv_data_h();
    iic_drv_clk_h();
}

/*----------------------------------------------------------------------------*/
/**@brief   ��ȡACK
   @param   ��
   @return  �����ACK/NACK�ĵ�ƽ
   @note    bool r_ack(void)
*/
/*----------------------------------------------------------------------------*/
void iic_drv_start(void)
{
    iic_drv_init_io();
    iic_drv_delay();
    iic_drv_data_l();
    iic_drv_delay();
    iic_drv_clk_l();
    iic_drv_delay();
    iic_drv_data_h();
}

/*----------------------------------------------------------------------------*/
/**@brief   STOP IIC
   @param   ��
   @return  ��
   @note    void iic_drv_stop(void)
*/
/*----------------------------------------------------------------------------*/
void iic_drv_stop(void)
{
    iic_drv_data_out();
    iic_drv_data_l();
    iic_drv_delay();
    iic_drv_clk_h();
    iic_drv_delay();
    iic_drv_data_h();
    iic_drv_delay();
}

/*----------------------------------------------------------------------------*/
/**@brief   ��ȡACK
   @param   ��
   @return  �����ACK/NACK�ĵ�ƽ
   @note    bool r_ack(void)
*/
/*----------------------------------------------------------------------------*/
bool drv_rev_ack(void)
{
    bit nack;
    iic_drv_data_in();
    iic_drv_delay();
    iic_drv_clk_h();
    iic_drv_delay();
    nack = iic_drv_data_r();
    iic_drv_clk_l();
    iic_drv_delay();
    return nack;
}

/*----------------------------------------------------------------------------*/
/**@brief   ����һ��ACK�źŵ�����,
   @param   flag �����͵�ACK/nack������
   @return  ��
   @note    void s_ack(u8 flag)
*/
/*----------------------------------------------------------------------------*/
void drv_send_ack(bool flag)
{
    iic_drv_data_out();
    iic_drv_clk_l();
    if (flag)
    {
        iic_drv_data_h();
    }
    else
    {
        iic_drv_data_l();
    }
    iic_drv_delay();
    iic_drv_clk_h();
    iic_drv_delay();
    iic_drv_clk_l();
}
/*----------------------------------------------------------------------------*/
/**@brief   ��IIC���߽���һ��BYTE������,
   @param   ��
   @return  ��ȡ�ص�����
   @note    u8 iic_drv_revbyte_io( void )
*/
/*----------------------------------------------------------------------------*/
u8 iic_drv_revbyte_io( void ) large
{
    u8 byteI2C = 0;
    u8 i;
    iic_drv_data_in();
    iic_drv_delay();
    for (i = 0;i < 8;i++)
    {
        iic_drv_clk_h();
        byteI2C <<= 1;
        iic_drv_delay();
        if (iic_drv_data_r())
        {
            byteI2C|=0x01;
        }
        iic_drv_clk_l();
        iic_drv_delay();
    }
    return byteI2C;
}
/*----------------------------------------------------------------------------*/
/**@brief   ��IIC���߽���һ��BYTE������,������һ��ָ����ACK
   @param   para ������ACK ���� NACK
   @return  ��ȡ�ص�����
   @note    u8 iic_drv_revbyte( u8 para )
*/
/*----------------------------------------------------------------------------*/
u8 iic_drv_revbyte(bool flag) large
{
    u8 byte;
    byte = iic_drv_revbyte_io();
    drv_send_ack(flag);
    return byte;
}
/*----------------------------------------------------------------------------*/
/**@brief   ��IIC���߷���һ��BYTE������
   @param   byte ��Ҫд��EEROM�ĵ�ַ
   @return  ��
   @note    void iic_drv_sendbyte_io(u8 byte)
*/
/*----------------------------------------------------------------------------*/
void iic_drv_sendbyte_io(u8 byte) large
{
    u8 i;
    iic_drv_data_out();
    iic_drv_delay();
    for (i = 0;i < 8;i++)
    {
        if (byte & BIT(7))
            iic_drv_data_h();                //���λ�Ƿ�Ϊ1,Ϊ1��SDA= 1,���� SDA=0
        else
            iic_drv_data_l();
        iic_drv_delay();
        iic_drv_clk_h();
        iic_drv_delay();
        byte <<= 1;                   //��������һλ,������һ������
        iic_drv_clk_l();
    }
}
/*----------------------------------------------------------------------------*/
/**@brief   ��IIC���߷���һ��BYTE������,����ȡACK
   @param   byte ��Ҫд��EEROM�ĵ�ַ
   @return  ��
   @note    void iic_drv_sendbyte(u8 byte)
*/
/*----------------------------------------------------------------------------*/
bool iic_drv_sendbyte(u8 byte)
{
    iic_drv_sendbyte_io(byte);
    return drv_rev_ack();

}

#endif

