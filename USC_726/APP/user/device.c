/*--------------------------------------------------------------------------*/
/**@file    DEVICE.C
   @brief   �豸���ƽӿ�ģ��
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/

#include "device.h"
#include "filesystem.h"
#include "IIC.h"
extern xd_u8 given_device;                ///<ָ�����ҵ��豸
extern u8 _xdata win_buffer[512];
extern FSAPIMSG _pdata fs_msg;
extern void stop_decode(void);

volatile u8 _bdata device_online;      ///<��ǰ���ߵ��豸
extern xd_u16 given_file_number;

sbit sdmmc_connect = device_online^0;   ///<SD������״̬��ǼĴ�����1:���� 0��������
sbit udisk_connect = device_online^1;   ///<USB MASS storage����״̬��ǼĴ�����1:���� 0��������
sbit pc_connect = device_online^4;      ///<PC����״̬��ǼĴ�����1:���� 0��������
u8 device_active;                       ///<��ǰ���߻�豸
u8 const _code file_type[] = "MP3WAVMP1MP2SMP";  ///<�����ļ���ʽ

u16 playpoint_filenum;							///<�ɹ���ȡ�ϵ���Ϣ����ļ��ţ�0Ϊû���ҵ� ��0Ϊ�ҵ�
u16 read_playpoint_info(u8 dev);
extern PLAYPOINT_TIME playpoint_time;
/*----------------------------------------------------------------------------*/
/**@brief   �豸��ʼ��
   @param   ��
   @return  0���ҵ���Ч˭����������û���ҵ���Ч�豸
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
    if (device_active & device_online)  ///<��ǰ��Ҫ����豸����
    {
		flush_all_msg();                ///<���ȫ����Ϣ
        stop_decode();                  ///<ֹͣ����
#if FILE_ENCRYPTION
         password_start(0);
#endif
        if (device_active == BIT(USB_DISK))
        {
			if (usb_host_emuerate_devcie(win_buffer))     ///<USB MASS STORAGE ö��
            {
				f_init(win_buffer, (u16)usb_otp_read, get_usb_device_pkt_size());      ///<ö�ٳɹ��� ��ʼ���ļ�ϵͳ����
            }
            else
            {
				return 0xff;             ///<ö��ʧ��
            }
        }
        else if (device_active == BIT(SDMMC))
        {
			if (init_sd())                  ///<sd ������
            {
				f_init(win_buffer, (u16)sdmmc_otp_read, 1);//get_usb_device_pkt_size());    ///<�����ɹ��� ��ʼ���ļ�ϵͳ����
            }
            else
            {
				return 0xff;                       ///<����ʧ��
            }
        }
    }
    if (!f_mount())                                      ///<�ļ�ϵͳ��ʼ��
    {
		fs_ext_setting(file_type);
		set_playpoint_info(read_playpoint_info(device_active));
		fs_scan_disk();
		playpoint_filenum = get_scan_filenumber();				   //���ݶ�Ӧ�豸����ʼ���ҳ��жϵ���ļ���
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
                                                 //����Ч�豸
        return 0xff;
	}
}
/*----------------------------------------------------------------------------*/
/**@brief    ����ָ�����豸���Զ�������һ����Ч�豸
   @param    select : 1������SD�豸��2������USB mass storage�豸��������û��ָ�������豸���Զ�������һ��
   @return   0���ɹ����ҵ�һ���豸���������豸����ʧ��
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
	            if (!device_init())             //�ҵ���Ч�豸
				{
	                break;
				}
	        }
		}
		else	//�豸��һ�γ�ʼ��ʧ��
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
				if (!device_init())             //�ҵ���Ч�豸
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
            return 1;                       //����Ч�豸������Ч�ļ�
        }
        else
        {
            return 0;
        }
    }
}
/*----------------------------------------------------------------------------*/
/**@brief   USB mass storage ���ӿں����� �˺��������Ľ�����Բ����õ� DATA GROUP,���Բ�������޸�
   @param   *buf ��BUFF
   @param   lbaH: LBA�ĸ�16λ
   @param   lbaL: LBA�ĵ�16λ
   @return  0���ɹ���ȡ��������ȡʧ��
   @note     u8 io_read_usb(u8 _xdata *buf, u16 lbah, u16 lbal)
*/
/*----------------------------------------------------------------------------*/
u8 io_read_usb(u8 _xdata *buf, u16 lbah, u16 lbal)
{
	return read_usb(lbal, lbah, buf);
}
/*----------------------------------------------------------------------------*/
/**@brief   SD �����ӿں������˺��������Ľ�����Բ����õ� DATA GROUP,���Բ�������޸�
   @param   *buf ��BUFF
   @param   lbaH: LBA�ĸ�16λ
   @param   lbaL: LBA�ĵ�16λ
   @return  0���ɹ���ȡ��������ȡʧ��
   @note     u8 io_read_sdmmc(u8 _xdata *buf, u16 lbah, u16 lbal)
*/
/*----------------------------------------------------------------------------*/
u8 io_read_sdmmc(u8 _xdata *buf, u16 lbah, u16 lbal)
{
	return sdmmc_read(lbal, lbah, buf);
}



