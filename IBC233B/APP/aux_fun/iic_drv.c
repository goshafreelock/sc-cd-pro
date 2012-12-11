/*--------------------------------------------------------------------------*/
/**@file     iic_drv.c
   @brief    IO模拟的IIC的驱动
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
/**@brief   IIC的IO口初始化
   @param   无
   @return  无
   @note    void iic_drv_init_io(void)
*/
/*----------------------------------------------------------------------------*/
void iic_drv_init_io(void)
{
    iic_drv_data_out();         //SDA设置成输出
    iic_drv_clk_out();         //SCL设置成输出
    iic_drv_data_h();
    iic_drv_clk_h();
}

/*----------------------------------------------------------------------------*/
/**@brief   读取ACK
   @param   无
   @return  都会的ACK/NACK的电平
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
   @param   无
   @return  无
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
/**@brief   读取ACK
   @param   无
   @return  都会的ACK/NACK的电平
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
/**@brief   发送一个ACK信号的数据,
   @param   flag ：发送的ACK/nack的类型
   @return  无
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
/**@brief   从IIC总线接收一个BYTE的数据,
   @param   无
   @return  读取回的数据
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
/**@brief   从IIC总线接收一个BYTE的数据,并发送一个指定的ACK
   @param   para ：发送ACK 还是 NACK
   @return  读取回的数据
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
/**@brief   向IIC总线发送一个BYTE的数据
   @param   byte ：要写的EEROM的地址
   @return  无
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
            iic_drv_data_h();                //最高位是否为1,为1则SDA= 1,否则 SDA=0
        else
            iic_drv_data_l();
        iic_drv_delay();
        iic_drv_clk_h();
        iic_drv_delay();
        byte <<= 1;                   //数据左移一位,进入下一轮送数
        iic_drv_clk_l();
    }
}
/*----------------------------------------------------------------------------*/
/**@brief   向IIC总线发送一个BYTE的数据,并读取ACK
   @param   byte ：要写的EEROM的地址
   @return  无
   @note    void iic_drv_sendbyte(u8 byte)
*/
/*----------------------------------------------------------------------------*/
bool iic_drv_sendbyte(u8 byte)
{
    iic_drv_sendbyte_io(byte);
    return drv_rev_ack();

}

#endif

