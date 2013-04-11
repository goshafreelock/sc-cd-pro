/*--------------------------------------------------------------------------*/
/**@file     rda5807.c
   @brief    RDA5807收音底层驱动
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/



#include "rda5807.h"
#include "clock.h"

#define RDA5807_WR_ADDRESS  0x20       ///<RDA5807 写地址
#define RDA5807_RD_ADDRESS  0x21       ///<RDA5807 读地址

u8 _idata rda5807_dat[12] = {0x0, 0x2, 0x0, 0x10, 0x04, 0x0, 0x80, 0xBF, 0x0, 0x0, 0x7E, 0xC6}; ///<5807控制寄存器映射表
/** RDA5807 状态寄存器映射表  */
u8 read_dat[4] _at_ 0x74;

#define rda5807_stc() (read_dat[0] & (1 << 6))       ///<锁频结束
#define rda5807_true() (read_dat[2] & (1 << 0))      ///<是否有台
#define rda5807_rssi() ((read_dat[2] >> 1))          ///<接收信号强度
/*----------------------------------------------------------------------------*/
/**@brief    RDA5807写寄存器函数
   @param    num 需要的数目
   @return   无
   @note     void rda5807_write(u8 num)
*/
/*----------------------------------------------------------------------------*/
void rda5807_write(u8 num)
{
    iic_write(RDA5807_WR_ADDRESS,0xff,rda5807_dat,num);
    //i2c_write_nbyte(RDA5807_WR_ADDRESS,rda5807_dat,num);
}
/*----------------------------------------------------------------------------*/
/**@brief    RDA5807读寄存器函数
   @param    需要读取的数目
   @return   无
   @note     void rda5807_read(u8 num)
*/
/*----------------------------------------------------------------------------*/
void rda5807_read(u8 num)
{
    iic_readn(RDA5807_RD_ADDRESS,0xff,read_dat,num);
    //delay_10ms(1);
}
/*----------------------------------------------------------------------------*/
/**@brief    RDA5807 初始化
   @param    无
   @return   无
   @note     void init_rda5807(void)
*/
/*----------------------------------------------------------------------------*/
void init_rda5807(void)
{
    read_dat[0] = 0;
    read_dat[1] = 0;
    read_dat[2] = 0;
    read_dat[3] = 0;
    rda5807_dat[0] = 0;
    rda5807_dat[1] = 2;
    rda5807_dat[2] = 0;
    rda5807_dat[3] = 0x10;
    rda5807_dat[4] = 0x04;
    rda5807_dat[5] = 0x0;
    rda5807_dat[6] = 0x80;
    rda5807_dat[7] = 0xbf;
    rda5807_dat[8] = 0x0;
    rda5807_dat[9] = 0x0;
    rda5807_dat[10] = 0x7e;
    rda5807_dat[11] = 0xc6;
    rda5807_write(2);
    delay_10ms(40);
    rda5807_dat[0] = 0xc0;
    rda5807_dat[1] = 0x01;
    rda5807_dat[7] &=~0x0F;
    rda5807_dat[7] |= 0x07;
    rda5807_dat[0] = 0xC0;
    rda5807_write(12);
    delay_10ms(10);
}
/*----------------------------------------------------------------------------*/
/**@brief    设置一个频点RDA5807
   @param    fre 频点  875~1080
   @return   1：当前频点有台，0：当前频点无台
   @note     bool set_fre_rda5807(u16 fre, u8 mode)
*/
/*----------------------------------------------------------------------------*/
bool set_fre_rda5807(u16 fre, u8 mode)
{
    u16 pll;

    if (mode)
    {
#if 1
        if (fre < 960)  //分段调整假台多的问题
            rda5807_dat[6] = 0x80 | 0x0a;
        else
            rda5807_dat[6] = 0x80 | 0x10;
#else
        rda5807_dat[6] = 0x80 | 0x08;
#endif
        rda5807_write(8);
        delay_10ms(7);
    }
    else
    {
#if 1
        if (fre == 870)  //分段调整假台多的问题
        {
            rda5807_dat[6] = 0x80 | 0x0a;
            rda5807_write(8);
            delay_10ms(7);
        }
        else if (fre == 960)
        {
            rda5807_dat[6] = 0x80 | 0x10;
            rda5807_write(8);
            delay_10ms(7);
        }
#else
        rda5807_dat[6] = 0x80 | 0x08;
        rda5807_write(8);
        delay_10ms(7);
#endif
    }
//    printf("2\n");
    pll = (fre - 870);
    rda5807_dat[2] = pll >> 2;
    rda5807_dat[3] = ((pll & 0x0003)<<6)|0x10;

    //set_iic_delay(5);
    rda5807_write(4);
    delay_10ms(10);
    //set_iic_delay(IIC_DELAY_DEFAULT);
//    printf("3\n");
//    printf(" 0x%04x \n",(u16)read_dat);
    //do
    {
        //delay_10ms(2);
        rda5807_read(4);
//        printf(" %02x ",(u16)read_dat[0]);
//        printf(" %02x ",(u16)read_dat[1]);
//        printf(" %02x ",(u16)read_dat[2]);
//        printf(" %02x \n",(u16)read_dat[3]);
    }
    //while (!rda5807_stc());
//    for (pll = 0;pll < 12;pll++)
    {
//        printf(" %02x ",(u16)rda5807_dat[pll]);
    }
//    printf("\n");
    return rda5807_true();

}

/*----------------------------------------------------------------------------*/
/**@brief    关闭RDA5807的电源
   @param    无
   @return   无
   @note     void rda5807_poweroff(void)
*/
/*----------------------------------------------------------------------------*/
void rda5807_poweroff(void)
{
    rda5807_dat[1] &= ~(1<<0);
    rda5807_write(2);
    delay_10ms(5);
}
/*----------------------------------------------------------------------------*/
/**@brief    dir =1  mute    dir =0   unmute
   @param    无
   @return   无
   @note     void rda5807_mute(dir)
*/
/*----------------------------------------------------------------------------*/
void rda5807_mute(bool dir)
{
	if(dir)
	{
		rda5807_dat[0] &= 0xBF;// mute on
	}
	else 
	{
		rda5807_dat[0] |= 0x40;// mute off
	}
	rda5807_write(2);
}




