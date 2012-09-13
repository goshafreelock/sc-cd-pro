/*--------------------------------------------------------------------------*/
/**@file     iic_api.c
   @brief    IIC_API模块
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#include "Custom_config.h"

#include "iic.h"
#include "config.h"
#include "device.h"
bool iic_busy = 0;
extern  bool fat_memory;
extern u8 xdata fat_flag;
/*----------------------------------------------------------------------------*/
/**@brief   IIC写函数
   @param   chip_id ：目标IC的ID号
   @param   iic_addr: 目标IC的目标寄存器的地址
   @param   *iic_dat: 写望目标IC的数据的指针
   @param   n:需要写的数据的数目
   @return  无
   @note    void  iic_write(u8 chip_id,u8 iic_addr,u8 *iic_dat,u8 n)
*/
/*----------------------------------------------------------------------------*/
void  iic_write(u8 chip_id,u8 iic_addr,u8 *iic_dat,u8 n) large
{
    iic_busy  = 1;
    iic_start();                //I2C启动
    iic_sendbyte(chip_id);         //写命令
    if (0xff != iic_addr)
    {
        iic_sendbyte(iic_addr);   //写地址
    }
    for (;n>0;n--)
    {
        iic_sendbyte(*iic_dat++);      //写数据
    }
    iic_stop();                 //I2C停止时序
    iic_busy = 0;
}
/*----------------------------------------------------------------------------*/
/**@brief   IIC总线向一个目标ID读取几个数据
   @param   address : 目标ID
   @param   *p     :  存档读取到的数据的buffer指针
   @param   number :  需要读取的数据的个数
   @return  无
   @note    void i2c_read_nbyte(u8 address,u8 *p,u8 number)
*/
/*----------------------------------------------------------------------------*/

void iic_readn(u8 chip_id,u8 iic_addr,u8 *iic_dat,u8 n) large
{

    iic_busy = 1;
    iic_start();                //I2C启动
    iic_sendbyte(chip_id);         //写命令
    if (0xff != iic_addr)
    {
        iic_sendbyte(iic_addr);   //写地址
    }
    for (;n>1;n--)
    {
        *iic_dat++ = iic_revbyte(0);      //写数据
    }
    *iic_dat++ = iic_revbyte(1);
    iic_stop();                 //I2C停止时序
    iic_busy = 0;
}
/*----------------------------------------------------------------------------*/
/**@brief   EEROM读函数
   @param   addr ：要写的EEROM的地址
   @param   无
   @return  无
   @note    u8 iic_read(u8 iic_addr)
*/
/*----------------------------------------------------------------------------*/
u8 read_eerom(u8 iic_addr)
{
    u8  byte;
    iic_busy = 1;
    iic_start();                    //I2C启动
    iic_sendbyte(0xa0);             //写命令
    iic_sendbyte(iic_addr);       //写地址
    iic_start();                    //写转为读命令，需要再次启动I2C
    iic_sendbyte(0xa1);             //读命令
    byte = iic_revbyte(1);
    iic_stop();                     //I2C停止
    iic_busy = 0;
    return  byte;
}


/*----------------------------------------------------------------------------*/
/**@brief   带有毫秒延时的EEROM写函数
   @param   addr ：要写的EEROM的地址
   @param   dat    ：需要写的数据
   @return  无
   @note    void write_info(u8 addr,u8 dat)
*/
/*----------------------------------------------------------------------------*/
void write_eerom(u8 addr,u8 dat)
{
    iic_write(0xa0,addr,&dat,1);
    delay_10ms(2);
}
#if defined(EEPROM_RTC_RAM_COMPATIBLE)
static bool eeprom_type=0;
u8 _code eeprom_chk[2]={0x66,0xCC};
void check_eeprom_status(void)
{
	u8 read_back_1=0,read_back_2=0;
	iic_busy = 1;
	write_eerom(IIC_VERIFY_BYTE_ADDR,eeprom_chk[0]);
	write_eerom(IIC_VERIFY_BYTE_ADDR+1,eeprom_chk[1]);

	read_back_1 = read_eerom(IIC_VERIFY_BYTE_ADDR);
	read_back_2 = read_eerom(IIC_VERIFY_BYTE_ADDR+1);

	if((read_back_1==eeprom_chk[0])&&(read_back_2==eeprom_chk[1])){

		eeprom_type = 1;
	}
	else{
		eeprom_type = 0;
	}

	printf("eeprom_type %x \r\n",(u16)eeprom_type);
	
	iic_busy = 0;
}
bool get_eeprom_status(void)
{
	return eeprom_type;
}
#endif

/*----------------------------------------------------------------------------*/
/**@brief   EEROM/RTC RAM读函数
   @param   addr ：要写的EEROM/RTC RAM的地址
   @return  读到的数据
   @note    u8 read_info(u8 addr)
*/
/*----------------------------------------------------------------------------*/
u8 read_info(u8 addr)
{
#if (USE_DEVICE == MEMORY_STYLE)
	if(!fat_memory)
	{
	    return read_eerom(addr);
	}
	else
	{
	    return read_rtc_ram(addr);
	}
#elif ((USE_RTCRAM == MEMORY_STYLE))
	return read_rtc_ram(addr);
#elif ((USE_EEPROM == MEMORY_STYLE))
	return read_eerom(addr);
#endif
}
/*----------------------------------------------------------------------------*/
/**@brief   EEROM/RTC RAM写函数
   @param   addr ：要读的EEROM/RTC RAM的地址
   @param   dat    ：需要读的数据
   @return  无
   @note    void write_info(u8 addr,u8 dat)
*/
/*----------------------------------------------------------------------------*/
void write_info(u8 addr,u8 dat)
{
#if (USE_DEVICE == MEMORY_STYLE)
	if(!fat_memory)
	{
	    write_eerom(addr,dat);
	}
	else
	{
        fat_flag =( BIT(USB_DISK) | BIT(SDMMC) );
	    write_rtc_ram(addr,dat);
	}
#elif ((USE_RTCRAM == MEMORY_STYLE))
	write_rtc_ram(addr,dat);
#elif ((USE_EEPROM == MEMORY_STYLE))
	write_eerom(addr,dat);
#endif
}


