/*--------------------------------------------------------------------------*/
/**@file     rtc_cpu.c
   @brief    RTC�ײ㺯��
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/


#include "rtc_cpu.h"

/*----------------------------------------------------------------------------*/
/**@brief ��RTC RAM����
   @param addr��RAM��ַ 0~63
   @return  RAM��ַ����
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
/**@brief дRTC RAM����
   @param addr:RAM��ַ��dat:д��RAM��ֵ0~63
   @return  ��
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
/**@brief дRTC�Ĵ���
   @param 	cmd���Ĵ���ֵ
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
/**@brief ��RTC�Ĵ���
   @param 	void
   @return  �Ĵ��������ֵ
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
/**@brief ��RTC 32Bit�Ĵ���
   @param 	cmd��������
   @return  ��ȡ�Ľ��
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
/**@brief дRTC 32Bit�Ĵ���
   @param 	cmd��д���sec��д���ֵ
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
/**@brief д���ӼĴ���
   @param 	sec��д�������ֵ
   @return  void
   @note  void send_alm_sec(u32 sec)
*/
/*----------------------------------------------------------------------------*/
void send_alm_sec(u32 sec)
{
    irtc_write32(WRITE_RTC_ALM,sec);
}
/*----------------------------------------------------------------------------*/
/**@brief �����ӼĴ���
   @param 	void
   @return  ����ֵ
   @note  u32 read_alm_sec(void)
*/
/*----------------------------------------------------------------------------*/
u32 read_alm_sec(void)
{
    return irtc_read32(READ_RTC_ALM);
}
/*----------------------------------------------------------------------------*/
/**@brief дʱ��Ĵ���
   @param 	sec��д���ʱ��ֵ
   @return  void
   @note   void send_rtc_sec(u32 sec)
*/
/*----------------------------------------------------------------------------*/
void send_rtc_sec(u32 sec)
{
    irtc_write32(WRITE_RTC_SEC,sec);
}
/*----------------------------------------------------------------------------*/
/**@brief ��ʱ��Ĵ���
   @param 	void
   @return  ������ʱ��
   @note  u32 read_rtc_sec(void)
*/
/*----------------------------------------------------------------------------*/
u32 read_rtc_sec(void)
{
    return irtc_read32(READ_RTC_SEC);
}

/*----------------------------------------------------------------------------*/
/**@brief ������
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
/**@brief ������
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
/**@brief �����RTC RAM���
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