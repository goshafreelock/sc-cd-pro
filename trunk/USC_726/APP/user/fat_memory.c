/*--------------------------------------------------------------------------*/
/**@file     fat_memory.c
   @brief    �豸�ļ�ϵͳ����
   @details  
   @author	  lj
   @date   2011-10-27
   @note
*/
/*----------------------------------------------------------------------------*/
#include "fat_memory.h"
#include "iic.h"


extern u8 _xdata win_buffer[512];
extern u8 device_active;
extern u8 bdata device_online;

u8 _xdata msg_buff0[FAT_MEMORY_SIZE+1] ;//_at_ 0xc00; //_at_ 0xde4;
u8 _xdata * msg_buff;
extern u8 music_vol;           ///< ��ǰ������
extern bit play_status;        ///< ����״̬ 1:����;0:��ͣ;
extern u8 given_device;        ///< ָ����Ҫ��ȡ���豸 1 SD ; 2 USB; 0:��֪���豸���������Ƿ�ֵ
extern u8 eq_mode;
extern u8 play_mode;
extern u16 given_file_number;
extern u16 filenameCnt;
extern u16 fmtr_freq;
bool fat_memory;               //1��:eeprom���ߣ������豸���䣻0��eeprom������ �����豸����
u8 xdata fat_flag;             //0: ����Ҫ�������ݵ��豸���������������ݵ��豸
/*----------------------------------------------------------------------------*/
/**@brief  ��⵽�Ƿ�����Ϣ����
   @param  ��
   @return ��
   @note   void updata_fat_memory(void)
*/
/*----------------------------------------------------------------------------*/
void updata_fat_memory(void)
{
#if ((USE_DEVICE == MEMORY_STYLE)&&(FAT_MEMORY))    
	u8 i;
    if(!fat_memory)
	{
	    return;
	}
    if( fat_flag ==( BIT(USB_DISK) | BIT(SDMMC) ))
    {
    	for(i = 0;i<64;i++)
    	{
    	   msg_buff[i] = read_rtc_ram(i);
    	}
        msg_buff0[0]=0xff;

    }
    if(fat_flag & ( BIT(USB_DISK) | BIT(SDMMC) ))
    {
		msg_buff[FAT_MEMORY_VERIRY] = 0x55;
        msg_buff[FAT_MEMORY_VERIRY+1] = 0xaa;
        msg_buff[DEVICE_COMPARE]++;
		device_online = get_device_online_status();
		if( (device_online & BIT(USB_DISK)) && ( fat_flag & BIT(USB_DISK) ) )
		{    
           if(usb_host_emuerate_devcie(win_buffer))
			{          
               if(write_hidden_file(msg_buff0,FAT_MEMORY_SIZE+1,BIT(USB_DISK)))
		        {
                   fat_flag &=~BIT(USB_DISK);
		        }
			}	
		}
		device_online = get_device_online_status();
		if((device_online & BIT(SDMMC)) && (fat_flag & BIT(SDMMC)))
		{
			if(init_sd())
			{
				if(write_hidden_file(msg_buff0,FAT_MEMORY_SIZE+1,BIT(SDMMC)))
		        {
                   fat_flag &=~BIT(SDMMC);
		        }
			}
		}
    }
#endif
}




#if ((USE_DEVICE == MEMORY_STYLE)&&(FAT_MEMORY))    
/*----------------------------------------------------------------------------*/
/**@brief  ��ȡ�豸�ڴ洢������
   @param  dev ���豸; buff_temp:���棺��Ŷ�ȡ������Ϣ
   @return 1���豸������ǰ�������Ϣ��0���豸�ڲ�û����Ϣ
   @note   bool read_device_data(u8 dev,u8 _xdata *buff_temp)
*/
/*----------------------------------------------------------------------------*/
bool read_device_data(u8 dev,u8 _xdata *buff_temp)
{
    check_reserve_sector(dev);
	if(read_hidden_file(buff_temp,FAT_MEMORY_SIZE,dev))
    {
		if( (0x55 == buff_temp[FAT_MEMORY_VERIRY]) && (0xaa == buff_temp[FAT_MEMORY_VERIRY+1]))/// && (dev == buff_temp[4]) 
        {
            return 1;
        }			 
    }
	return 0;
}



/*----------------------------------------------------------------------------*/
/**@brief  ��ȡ�����豸�ڲ��ļ�����Ϣ
   @param  ��
   @return ��
   @note   void get_info(void)
*/
/*----------------------------------------------------------------------------*/
void get_info(void)
{
    u8 dev_tmp0;
    u8 dev_tmp1;
    u8 _xdata buff_temp[FAT_MEMORY_SIZE];
    msg_buff = &msg_buff0[1];
	if(!fat_memory)
	{
	    return;
	}
    device_online = get_device_online_status();
    dev_tmp1 = 0;
    if (device_online & BIT(USB_DISK))
    {
		if (usb_host_emuerate_devcie(win_buffer))     ///<USB MASS STORAGE ö��
        {
			f_init(win_buffer, (u16)usb_otp_read, get_usb_device_pkt_size());      ///<ö�ٳɹ��� ��ʼ���ļ�ϵͳ����
            if (!f_mount()) 
            {
				dev_tmp0 = BIT(USB_DISK);
                if(read_device_data(dev_tmp0,msg_buff))
                {
					dev_tmp1 = 0x55;
                }
            }
        }
    }
    
    if (device_online & BIT(SDMMC))
    {
		if (init_sd())                  ///<sd ������
        {
			f_init(win_buffer, (u16)sdmmc_otp_read, 1);//get_usb_device_pkt_size());    ///<�����ɹ��� ��ʼ���ļ�ϵͳ����
            if (!f_mount()) 
            {
				dev_tmp0 = BIT(SDMMC);
                if(read_device_data(dev_tmp0,buff_temp))
                {
                    if( 0x55 == dev_tmp1 )
                    {
                         if(buff_temp[DEVICE_COMPARE] > msg_buff[DEVICE_COMPARE]) 
                         {
                            if((buff_temp[DEVICE_COMPARE] == 0xff) && (msg_buff[DEVICE_COMPARE] == 0))
                            {
                                goto _GET_FAT_INFO;
                            }
                            my_memcpy(msg_buff,buff_temp,FAT_MEMORY_SIZE);
                         }
                    }
                    else
                    {
                        my_memcpy(msg_buff,buff_temp,FAT_MEMORY_SIZE);
                    }
                }
            }
        }
    }
_GET_FAT_INFO:
	{
	    u8 i;
		for(i = 0;i<64;i++)
		{
		    write_rtc_ram(i,msg_buff[i]);
		}
    }

}

/*----------------------------------------------------------------------------*/
/**@brief   ��ȡ�ļ���
   @param   *p : ��ȡ�ĳ��ļ����Ĵ��BUFF��ָ��
   @return   1:��ȡ���ǳ��ļ�����0����ȡ�����Ƕ��ļ���
   @note     bool get_filename( u8 _xdata *p)
*/
/*----------------------------------------------------------------------------*/
void get_info_memory(void)
{
    u8 tmp = 0xaa;
	if(0xaa == read_eerom(MEM_EEPROM))
	{
	    fat_memory = 0;
	}
    iic_write(0xa0,MEM_EEPROM,&tmp,1);
    delay_10ms(2);
	if(0xaa == read_eerom(MEM_EEPROM))
	{
	    fat_memory =  0;
	}
	fat_memory = 1;

}

#endif