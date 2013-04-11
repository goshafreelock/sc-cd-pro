/*--------------------------------------------------------------------------*/
/**@file     rtc_cpu.c
   @brief    RTC底层函数
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/


#include "rtc_cpu.h"

/*----------------------------------------------------------------------------*/
/**@brief 读RTC RAM操作
   @param addr：RAM地址 0~63
   @return  RAM地址数据
   @note u8 read_rtc_ram(u8 addr)
*/
/*----------------------------------------------------------------------------*/
#pragma disable
u8 read_rtc_ram(u8 addr)
{
    u8 dat;
    IRTC_CSEN;
    irtc_sr_byte(READ_RTC_RAM);
    irtc_sr_byte(addr);
    dat=irtc_sr_byte(0x0);
    IRTC_CSDIS;
    return dat;
}
/*----------------------------------------------------------------------------*/
/**@brief 写RTC RAM操作
   @param addr:RAM地址；dat:写入RAM的值0~63
   @return  无
   @note void write_rtc_ram(u8 addr,u8 dat)
*/
/*----------------------------------------------------------------------------*/
void write_rtc_ram(u8 addr,u8 dat)
{
    IRTC_CSEN;
    irtc_sr_byte(WRITE_RTC_RAM);
    irtc_sr_byte(addr);
    irtc_sr_byte(dat);
    IRTC_CSDIS;
}

/*----------------------------------------------------------------------------*/
/**@brief 写RTC寄存器
   @param 	cmd：寄存器值
   @return  void
   @note  void write_rtc_reg(u8 cmd)
*/
/*----------------------------------------------------------------------------*/
void write_rtc_reg(u8 cmd)
{
    IRTC_CSEN;
    irtc_sr_byte(WRITE_RTC_REG);
    irtc_sr_byte(cmd);
    IRTC_CSDIS;
}
/*----------------------------------------------------------------------------*/
/**@brief 读RTC寄存器
   @param 	void
   @return  寄存器里面的值
   @note  u8 read_rtc_reg(void)
*/
/*----------------------------------------------------------------------------*/
#pragma disable
u8 read_rtc_reg(void)
{
    u8 tmp;
    IRTC_CSEN;
    irtc_sr_byte(READ_RTC_REG);
    tmp = irtc_sr_byte(0);
    IRTC_CSDIS;
    return tmp;
}

/*----------------------------------------------------------------------------*/
/**@brief 读RTC 32Bit寄存器
   @param 	cmd：读命令
   @return  读取的结果
   @note   u32 irtc_read32(u8 cmd)
*/
/*----------------------------------------------------------------------------*/
u32 irtc_read32(u8 cmd)
{
    u32 rtc_buf32;
    IRTC_CSEN;
    irtc_sr_byte(cmd);
    ((u8 *)(&rtc_buf32))[0]=irtc_sr_byte(0x0);
    ((u8 *)(&rtc_buf32))[1]=irtc_sr_byte(0x0);
    ((u8 *)(&rtc_buf32))[2]=irtc_sr_byte(0x0);
    ((u8 *)(&rtc_buf32))[3]=irtc_sr_byte(0x0);
    IRTC_CSDIS;
    return rtc_buf32;
}
/*----------------------------------------------------------------------------*/
/**@brief 写RTC 32Bit寄存器
   @param 	cmd：写命令；sec：写入的值
   @return  void
   @note   void irtc_write32(u8 cmd,u32 sec)
*/
/*----------------------------------------------------------------------------*/
void irtc_write32(u8 cmd,u32 sec) large
{
    u8 buf;
    IRTC_CSEN;
    irtc_sr_byte(cmd);

    buf=((u8 *)(&sec))[0];
    irtc_sr_byte(buf);
    buf=((u8 *)(&sec))[1];
    irtc_sr_byte(buf);
    buf=((u8 *)(&sec))[2];
    irtc_sr_byte(buf);
    buf=((u8 *)(&sec))[3];
    irtc_sr_byte(buf);

    IRTC_CSDIS;
}
/*----------------------------------------------------------------------------*/
/**@brief 写闹钟寄存器
   @param 	sec：写入的闹钟值
   @return  void
   @note  void send_alm_sec(u32 sec)
*/
/*----------------------------------------------------------------------------*/
void send_alm_sec(u32 sec)
{
    irtc_write32(WRITE_RTC_ALM,sec);
}
/*----------------------------------------------------------------------------*/
/**@brief 读闹钟寄存器
   @param 	void
   @return  闹钟值
   @note  u32 read_alm_sec(void)
*/
/*----------------------------------------------------------------------------*/
u32 read_alm_sec(void)
{
    return irtc_read32(READ_RTC_ALM);
}
/*----------------------------------------------------------------------------*/
/**@brief 写时间寄存器
   @param 	sec：写入的时间值
   @return  void
   @note   void send_rtc_sec(u32 sec)
*/
/*----------------------------------------------------------------------------*/
void send_rtc_sec(u32 sec)
{
    irtc_write32(WRITE_RTC_SEC,sec);
}
/*----------------------------------------------------------------------------*/
/**@brief 读时间寄存器
   @param 	void
   @return  读出的时间
   @note  u32 read_rtc_sec(void)
*/
/*----------------------------------------------------------------------------*/
u32 read_rtc_sec(void)
{
    return irtc_read32(READ_RTC_SEC);
}

/*----------------------------------------------------------------------------*/
/**@brief 开闹钟
   @param 	void
   @return  void
   @note  void enable_alm(void)
*/
/*----------------------------------------------------------------------------*/
void enable_alm(void)
{
    u8 rtccon;
    rtccon=read_rtc_reg();
    write_rtc_reg(rtccon|0x02);

}
/*----------------------------------------------------------------------------*/
/**@brief 关闹钟
   @param 	void
   @return  void
   @note void disable_alm(void)
*/
/*----------------------------------------------------------------------------*/
void disable_alm(void)
{
    u8 rtccon;
    rtccon=read_rtc_reg();
    write_rtc_reg(rtccon&0xfd);

}
/*----------------------------------------------------------------------------*/
/**@brief 掉电后RTC RAM清空
   @param 	void
   @return  void
   @note  void reset_rtc_ram(void)
*/
/*----------------------------------------------------------------------------*/
void reset_rtc_ram(void)
{
    u8 i;
    for (i = 0; i < 64; i++)
    {
        write_rtc_ram(i, 0);
    }
}