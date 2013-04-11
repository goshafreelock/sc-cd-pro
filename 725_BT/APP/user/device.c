/*--------------------------------------------------------------------------*/
/**@file    DEVICE.C
   @brief   设备控制接口模块
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/

#include "device.h"
#include "filesystem.h"
#include "IIC.h"
extern xd_u8 given_device;                ///<指定查找的设备
extern u8 _xdata win_buffer[512];
extern FSAPIMSG _pdata fs_msg;
extern void stop_decode(void);

volatile u8 _bdata device_online;      ///<当前在线的设备
extern xd_u16 given_file_number;

sbit sdmmc_connect = device_online^0;   ///<SD卡在线状态标记寄存器；1:在线 0：不在线
sbit udisk_connect = device_online^1;   ///<USB MASS storage在线状态标记寄存器；1:在线 0：不在线
sbit pc_connect = device_online^4;      ///<PC在线状态标记寄存器；1:在线 0：不在线
u8 device_active;                       ///<当前在线活动设备
u8 const _code file_type[] = "MP3WAVMP1MP2SMP";  ///<解码文件格式

u16 playpoint_filenum;							///<成功读取断点信息后的文件号：0为没有找到 非0为找到
u16 read_playpoint_info(u8 dev);
extern PLAYPOINT_TIME playpoint_time;
/*----------------------------------------------------------------------------*/
/**@brief   设备初始化
   @param   无
   @return  0：找到有效谁而；其他：没有找到有效设备
   @note    u8 device_init(void)
*/
/*----------------------------------------------------------------------------*/
u8 device_init(void)
{
    device_online = get_device_online_status();
	if(playpoint_time.last_device != device_active)
	{
		write_playtime(&playpoint_time);
	}
    if (device_active & device_online)  ///<当前需要活动的设备在线
    {
		flush_all_msg();                ///<清空全部消息
        stop_decode();                  ///<停止解码
#if FILE_ENCRYPTION
         password_start(0);
#endif
        if (device_active == BIT(USB_DISK))
        {
			if (usb_host_emuerate_devcie(win_buffer))     ///<USB MASS STORAGE 枚举
            {
				f_init(win_buffer, (u16)usb_otp_read, get_usb_device_pkt_size());      ///<枚举成功后 初始化文件系统参数
            }
            else
            {
				return 0xff;             ///<枚举失败
            }
        }
        else if (device_active == BIT(SDMMC))
        {
			if (init_sd())                  ///<sd 卡鉴定
            {
				f_init(win_buffer, (u16)sdmmc_otp_read, 1);//get_usb_device_pkt_size());    ///<鉴定成功后 初始化文件系统参数
            }
            else
            {
				return 0xff;                       ///<鉴定失败
            }
        }
    }
    if (!f_mount())                                      ///<文件系统初始化
    {
		fs_ext_setting(file_type);
		set_playpoint_info(read_playpoint_info(device_active));
		fs_scan_disk();
		playpoint_filenum = get_scan_filenumber();				   //根据对应设备的起始簇找出有断点的文件号
		/**/
		if(0 == playpoint_filenum)
		{
		    if(given_file_number)
			    given_file_number = 1;
		}
		
        if (fs_msg.fileTotal == 0)
        {

			return 0xfe;
        }
#if ((USE_DEVICE == MEMORY_STYLE)&&(FAT_MEMORY))    
        check_reserve_sector(device_active);
#endif 
        return 0;
    }
    else
	{
                                                 //无有效设备
        return 0xff;
	}
}
/*----------------------------------------------------------------------------*/
/**@brief    查找指定的设备或自动查找下一个有效设备
   @param    select : 1：查找SD设备；2：超找USB mass storage设备：其他：没有指定查找设备，自动查找下一个
   @return   0：成功查找到一个设备；其他：设备查找失败
   @note     u8 find_device(u8 select)
*/
/*----------------------------------------------------------------------------*/
u8 find_device(u8 select)
{
    static u8 last_device;
	u8 i;

	given_device = 0;
	
    device_online = get_device_online_status();
    
	if ((select == 0x01) || (select == 0x02))
    {
		if (select & device_online)
        {
			device_active = select;
			last_device = select;

            if (!device_init())
			{
                return 0;
			}
        }
        else
        {
            last_device = select;
        }
        device_active = 0;
        return 1;
    }
    else
    {
		if (device_active)
		{
			for (i = 0;i < MAX_DEVICE;i++)
	        {
				if (device_active == BIT(SDMMC))
	            {
					device_active = BIT(USB_DISK);
	            }
	            else// if (device_active == BIT(USB_DISK))
				{  
					device_active = BIT(SDMMC);
				}
	
	            if ((device_active & device_online) == 0)
                {
	                continue;
	            }
	            if (!device_init())             //找到有效设备
				{
	                break;
				}
	        }
		}
		else	//设备第一次初始化失败
		{
			if (last_device == BIT(SDMMC))
			{
				device_active = BIT(USB_DISK);
			}
			else if (last_device == BIT(USB_DISK))
			{
				device_active = BIT(SDMMC);	
			}
			if ((device_active & device_online))
			{
				if (!device_init())             //找到有效设备
				{
					i = 0;
				}
			}
			else
			{
				i = MAX_DEVICE;	
			}
			last_device = 0;		
		}

        if (i == MAX_DEVICE)
        {
            last_device = 0;	       
            device_active = 0;
            return 1;                       //无有效设备或无有效文件
        }
        else
        {
            return 0;
        }
    }
}
/*----------------------------------------------------------------------------*/
/**@brief   USB mass storage 读接口函数： 此函数编译后的结果绝对不能用到 DATA GROUP,所以不能随便修改
   @param   *buf 读BUFF
   @param   lbaH: LBA的高16位
   @param   lbaL: LBA的低16位
   @return  0：成功读取；其他读取失败
   @note     u8 io_read_usb(u8 _xdata *buf, u16 lbah, u16 lbal)
*/
/*----------------------------------------------------------------------------*/
u8 io_read_usb(u8 _xdata *buf, u16 lbah, u16 lbal)
{
	return read_usb(lbal, lbah, buf);
}
/*----------------------------------------------------------------------------*/
/**@brief   SD 卡读接口函数：此函数编译后的结果绝对不能用到 DATA GROUP,所以不能随便修改
   @param   *buf 读BUFF
   @param   lbaH: LBA的高16位
   @param   lbaL: LBA的低16位
   @return  0：成功读取；其他读取失败
   @note     u8 io_read_sdmmc(u8 _xdata *buf, u16 lbah, u16 lbal)
*/
/*----------------------------------------------------------------------------*/
u8 io_read_sdmmc(u8 _xdata *buf, u16 lbah, u16 lbal)
{
	return sdmmc_read(lbal, lbah, buf);
}



