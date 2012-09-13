/*--------------------------------------------------------------------------*/
/**@file     fm_receive_api.c
   @brief    FM����Ӧ�ýӿ�
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/


#include "fm_api.h"


extern _xdata u8 fre_point[];
extern bit key_voice_disable;
extern u8 device_active;                           ///< ��ǰ����豸
extern xd_u8 all_channl;                              ///< FM������ǰ��������̨����
extern xd_u16 frequency;                              ///< ��ǰƵ��
extern xd_u8 fre_channl;                              ///< FM������ǰ���ڵ�̨��
extern xd_u8 given_device;
extern xd_u8 work_mode;

extern void KT_AMFMStandby(void);
extern xd_u8 KT_AMFMWakeUp(void);
extern void KT_AMFMSetMode(xd_u8 AMFM_MODE);
extern xd_u8 KT_AMTune(xd_u16 Frequency);
extern xd_u8 KT_FMTune(xd_u16 Frequency);
#if 0
/*----------------------------------------------------------------------------*/
/**@brief   FMģ���ʼ���ӿں���
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
/**@brief   �ر�FMģ���Դ
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
/**@brief   ����һ��FMƵ��Ľӿں���
   @param   fre��Ƶ��
   @return  1����̨��0����̨
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
/**@brief    ��ȡһ���Ѿ����µ�̨��
   @param    flag ��̨��
   @return	 ��
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
/**@brief    ȫƵ������
   @param	 ��
   @return	 ���ؼ���
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
		if(flag) 	   //������̨ѭ��
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
		return 1;			   //����FMģʽ
	return 0;				   //����FMģʽ
}

/*----------------------------------------------------------------------------*/
/**@brief   ��һ��Ƶ��Ϊ̨
   @param	 ��Ҫ�洢��Ƶ��
   @return	 ��
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