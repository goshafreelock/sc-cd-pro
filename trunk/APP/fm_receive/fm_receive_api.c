/*--------------------------------------------------------------------------*/
/**@file     fm_receive_api.c
   @brief    FM任务应用接口
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/


#include "fm_api.h"


extern _xdata u8 fre_point[];
extern bit key_voice_disable;
extern u8 device_active;                           ///< 当前活动的设备
extern xd_u8 all_channl;                              ///< FM收音当前搜索到的台总数
extern xd_u16 frequency;                              ///< 当前频率
extern xd_u8 fre_channl;                              ///< FM收音当前所在的台号
extern xd_u8 given_device;
extern xd_u8 work_mode;

extern void KT_AMFMStandby(void);
extern xd_u8 KT_AMFMWakeUp(void);
extern void KT_AMFMSetMode(xd_u8 AMFM_MODE);
extern xd_u8 KT_AMTune(xd_u16 Frequency);
extern xd_u8 KT_FMTune(xd_u16 Frequency);
#if 0
/*----------------------------------------------------------------------------*/
/**@brief   FM模块初始化接口函数
   @param
   @return
   @note    void init_fm_rev(void)
*/
/*----------------------------------------------------------------------------*/
void init_fm_rev(void)
{
	
    //init_rda5807();
}
/*----------------------------------------------------------------------------*/
/**@brief   关闭FM模块电源
   @param
   @return
   @note    void fm_rev_powerdown(void
*/
/*----------------------------------------------------------------------------*/
void fm_rev_powerdown(void)
{
    //rda5807_poweroff();
}
/*----------------------------------------------------------------------------*/
/**@brief   设置一个FM频点的接口函数
   @param   fre：频点
   @return  1：有台；0：无台
   @note    bool set_fre(u16 fre, u8 mode)
*/
/*----------------------------------------------------------------------------*/
bool set_fre(u16 fre, u8 mode)
{
#if SDMMC_CMD_MODE   
	bit flag;
	sd_chk_ctl(DIS_SD_CMD_CHK);
	//flag = set_fre_rda5807(fre, mode);
	sd_chk_ctl(EN_SD_CMD_CHK);
	return flag;
#else
	//return set_fre_rda5807(fre, mode);
#endif

}

/*----------------------------------------------------------------------------*/
/**@brief    获取一个已经存下的台号
   @param    flag ：台号
   @return	 无
   @note     void get_channl(u8 flag)
*/
/*----------------------------------------------------------------------------*/
void get_channl(u8 flag)
{
    frequency = fre_point[flag-1] + MIN_FRE;
    if ( frequency > MAX_FRE)
    {
        frequency = MIN_FRE;
    }
    else if (frequency < MIN_FRE)
    {
        frequency = MAX_FRE;
    }
   // set_fre_rda5807(frequency,1);
    write_info(MEM_FRE_CHANNL,fre_channl);
    write_info(MEM_FRE,frequency - MIN_FRE);
}
/*----------------------------------------------------------------------------*/
/**@brief    全频段搜索
   @param	 无
   @return	 返回级数
   @note     bool scan_fre(void)
*/
/*----------------------------------------------------------------------------*/
bool scan_fre(void)
{
    u8 i;
	u8 key;
	u8 flag = 0;
    key_voice_disable = 1;
    delay_10ms(10);
    all_channl = 0;
    frequency = MIN_FRE;
    my_memset((u8 _xdata*)fre_point,0,30);
   // rda5807_mute(1);
	for (i = 0;i <= (1080-MIN_FRE);i++)
    {
        //printf("----------- fre %4u -----------------\n",(u16)(fre));
		do
		{
			key = get_msg();
			switch (key)
	        {
		        case INFO_PLAY | KEY_SHORT_UP :
		            flag = 1;
					break;
				case MSG_SDMMC_IN :
				    given_device = BIT(SDMMC);
				    put_msg_lifo(SEL_GIVEN_DEVICE_GIVEN_FILE);
		            work_mode = SYS_MP3DECODE;
		            flag = 2;
					break;
				 case MSG_USB_DISK_IN  :
			        given_device = BIT(USB_DISK);
			        put_msg_lifo(SEL_GIVEN_DEVICE_GIVEN_FILE);
			        work_mode = SYS_MP3DECODE;
			        flag = 2;
					break;
				 case MSG_AUX_IN :
				    work_mode = SYS_AUX;
				    flag = 2;
					break;
			}
		}
		while(0xff != key);
		if(flag) 	   //跳出搜台循环
			break;
        if (set_fre(frequency, 0))
        {
       //     rda5807_mute(0);
			fre_point[all_channl] = i;
            all_channl++;
			Disp_Con(DISP_FREQ);
			delay_10ms(100);
			//rda5807_mute(1);
        }
        Disp_Con(DISP_FREQ);
        if (all_channl >= MAX_CHANNL)
        {
            //deg_str("channl is full\n");
            break;
        }
        frequency++;
    }
	//rda5807_mute(0);
    key_voice_disable = 0;
    for (i = 0; i < all_channl; i++)
    {
        write_info(MEM_CHANNL + i , fre_point[i]);
    }
    write_info(MEM_ALL_CHANNL , all_channl);

	if(flag == 2)
		return 1;			   //跳出FM模式
	return 0;				   //继续FM模式
}

/*----------------------------------------------------------------------------*/
/**@brief   存一个频点为台
   @param	 需要存储的频点
   @return	 无
   @note     void save_fre(u16 fre)
*/
/*----------------------------------------------------------------------------*/
void save_fre(u16 fre)
{
    if (all_channl < MAX_CHANNL)
    {
        fre_point[all_channl] = fre - MIN_FRE;
        write_info(MEM_CHANNL + all_channl , fre - MIN_FRE);
        all_channl++;
    }
    write_info(MEM_ALL_CHANNL , all_channl);
    //delay_10ms(3);

    //delay_10ms(3);
    fre_channl = all_channl;
    get_channl(fre_channl);
}
#endif