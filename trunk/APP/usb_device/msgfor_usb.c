/*--------------------------------------------------------------------------*/
/**@file     msgfor_usb.c
   @brief    usb 从机任务模块
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/

/* msgfor usb */

#include "msgfor_usb.h"
#include "fat_memory.h"
#include "voice_time.h"
#include "uart.h"
extern u8 _idata music_vol;
extern xd_u8 eq_mode;
extern xd_u8 return_cnt;
extern xd_u8 curr_menu;
extern xd_u8 work_mode;
extern bit pc_connect;
extern void main_vol(u8 vol);
extern xd_u16 cfilenum;
extern u8 xdata last_work_mode;
extern bool alarm_on;
u8 const _code ufi_inquiry_str[24] =	///<UFI设备信息，必须填充24个字符
{
	'J',    //供应商信息 8个字符 -- Vender information start
    	'K',
	'T', 
	'E', 
	'K',
	' ',    //需填充空字符
    ' ',    
    ' ',    
    'M',    //产品标识 16个字符 -- Production Identification start	
    'A',    
    'S',
    'S',
    ' ',    
    'S',    
    'T',    
    'O',    
    'R',
    'A',
    'G',	//需填充空字符
    'E',
    ' ',
    ' ',
    ' ',
    ' ',    //Production Identification end	   31
};
/*----------------------------------------------------------------------------*/
/**@brief   usb hid 命令接口函数
   @param   Audio_Flag ：  hid 命令
   @return  无
   @note    void usb_hid_control(u8 Audio_Flag)
*/
/*----------------------------------------------------------------------------*/
void usb_hid_control(u8 Audio_Flag)
{
    usb_hid_key(Audio_Flag);
    usb_hid_key(0);
}

/*----------------------------------------------------------------------------*/
/**@brief   usb Audio 接口函数
   @param   无
   @return  无
   @note    void USB_Audio_Vol_set(void)
*/
/*----------------------------------------------------------------------------*/
void USB_Audio_Vol_set(void)
{
    u8 vol;
    vol = USB_Audio_Volume(1, 0); //0~255
    music_vol = vol >> 3;   	
    music_vol = (music_vol > 30) ? 30 : music_vol;
    if (vol == 0)
	{
		if (vol == 0)
			music_vol = 0;
		else
			music_vol = 1;
	}
	if (music_vol == 0)
    {
        if (usb_audio_mute_status() == 0)
		{
            //printf("usb audio mute\n");
			Usb_Audio_Mute_Ctl();
		}
    }
    else
    {
        if (usb_audio_mute_status() == 1)
		{
		    //printf("usb audio ummute\n");
            Usb_Audio_Mute_Ctl();
		}
    }
	write_info(MEM_VOL, music_vol);
		
}
/*----------------------------------------------------------------------------*/
/**@brief   usb 从机任务 主循环函数
   @param   无
   @return  无
   @note    void usb_device(void)
*/
/*----------------------------------------------------------------------------*/
void usb_device(void)
{
    u8 key;

//    printf("pc online\n");
    Disp_Con(DISP_USBDEV);
    usb_config(USB_DEVICE_CFG);
    write_protect_set(0);                           //不使用写保护功能,如果需要，请根据卡座写保护的状态改变配置
    USB_Audio_Volume(0, music_vol);
    init_usb_device_hardware();
    flush_low_msg();
    while (1)
    {
#if ((USB_DEVICE_CFG & USB_MASS) == USB_MASS)
        UFI_massstorage();
#endif
#if ((USB_DEVICE_CFG & USB_SPEAKER) == USB_SPEAKER)
		dac_out_select(DAC_DECODE);
#endif	
        key = get_msg();
        switch (key)
        {
#if ((USB_DEVICE_CFG & USB_SPEAKER) == USB_SPEAKER)
        case MSG_PC_MUTE:
            //main_vol_set(0, CHANGE_VOL_NO_MEM);//main_vol(0);
            dac_mute_control(1, 1);
			break;
        case MSG_PC_UNMUTE:
			//main_vol_set(0, SET_USE_CURRENT_VOL);
			dac_mute_control(0, 1);
            break;
        case INFO_VOL_PLUS :
        case INFO_VOL_PLUS | KEY_HOLD :
			usb_hid_control(USB_AUDIO_VOLUP);
			//Disp_Con(Disp_VOL_UP);
			break;
        case INFO_VOL_MINUS :
        case INFO_VOL_MINUS | KEY_HOLD :
			usb_hid_control(USB_AUDIO_VOLDOWN);
			//Disp_Con(Disp_VOL_DOWN);
			break;

        case MSG_PC_AUDIO:
			USB_Audio_Vol_set();
			break;
#endif

#if ((USB_DEVICE_CFG & USB_HID) == USB_HID)
        case INFO_PLAY | KEY_SHORT_UP :
			//Usb_Audio_Mute_Ctl();
			usb_hid_control(USB_AUDIO_PP);
            break;

        case INFO_HALF_SECOND :
#if ((USE_DEVICE == MEMORY_STYLE)&&(FAT_MEMORY))           
            updata_fat_memory();
#endif
			return_cnt++;
            if (RETURN_TIME == return_cnt)
            {
                if (DISP_DWORD_NUMBER == curr_menu)
                {
                    cfilenum = 0;
                }
                if (DISP_USBDEV != curr_menu)
                {
                    Disp_Con(DISP_USBDEV);
                }
            }
            break;

        case INFO_NEXT_FIL | KEY_SHORT_UP:
        case INFO_NEXT_FIL | KEY_HOLD:
			usb_hid_control(USB_AUDIO_NEXTFILE);
            break;

        case INFO_PREV_FIL | KEY_SHORT_UP:
        case INFO_PREV_FIL | KEY_HOLD:
			usb_hid_control(USB_AUDIO_PREFILE);
            break;

#endif

		case MSG_USB_PC_OUT:
            return;
#if RTC_ENABLE 
		case INFO_ALM_BELL :			 ///<闹钟响消息，进入闹钟循环
               alarm_on =1;
              last_work_mode = work_mode; 
              work_mode =  SYS_RTC;
              return;
#endif
        default :
            if (!ap_handle_hotkey(key))
                return;
        }
    }

}
/*----------------------------------------------------------------------------*/
/**@brief   usb 从机任务 入口函数
   @param   无
   @return  无
   @note    void usb_audio_massstorage(void)
*/
/*----------------------------------------------------------------------------*/
void usb_audio_massstorage(void)
{
    EA = 0;
	if(usb_diskin_detect())
	{
		EA = 1;
		return;
	}
	usb_pcin_detect();
    Delay(100);
    usb_pcin_detect();
    device_check();
    EA = 1;

    if (pc_connect)
    {
        work_mode = SYS_USBDEVICE;
        sysclock_div2(0);
        sd_speed_init(1, 100);
        flush_all_msg();
		set_scsiinquiry(ufi_inquiry_str);
		set_max_vol(MAX_ANALOG_VOL, MAX_DIGITAL_VOL);			//设置USB Slave模式的音量上限
        usb_device();
        IE1 &= ~BIT(1);
        disable_usb();
		delay_10ms(50);
    }
}







