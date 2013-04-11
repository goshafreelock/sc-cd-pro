/*--------------------------------------------------------------------------*/
/**@file       get_musice_file.c
   @brief      文件查找
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#include "Custom_config.h"

#include "get_music_file.h"


extern xd_u16 given_file_number;
extern xd_u8 given_device;
extern xd_u8 play_mode;
extern FSAPIMSG _pdata fs_msg;
extern u8 device_active;

extern void clean_playpoint(u8 dev);
extern u16 playpoint_filenum;

#ifdef USE_USB_SD_DECODE_FUNC	       
extern bool folder_mode_select;
#endif
bool repeat_off_flag=0;

#ifdef USE_USB_PROG_PLAY_MODE
extern bool usb_prog_icon_bit;
extern xd_u8 usb_prog_tab[20];
extern bool get_prog_song_num(u8 get_Mode);
#endif

/*----------------------------------------------------------------------------*/
/**@brief   获取一个随机数
   @param   无
   @return  获取到的随机数
   @note    u8 ReadLFSR()
*/
/*----------------------------------------------------------------------------*/
u8 ReadLFSR()
{
    u8 tmp ;
    CRCFIFO = T0CNT;
    tmp = CRCFIFO ;
    return tmp;
}
extern void usb_prog_mode_cls();
/*----------------------------------------------------------------------------*/
/**@brief    超找上一个或下一个文件，或自动获取下一个
   @param    dir：0:next file, 1:previous file, 2:auto next,可能会导致设备的切换
   @return   无
   @note     void get_music_file1(u8 dir)
*/
/*----------------------------------------------------------------------------*/
void get_music_file1(u8 dir)
{


	if (device_check() == 0)                    //如果当前播放的设备不在线，则找下一个设备, 并且选择第一个文件进行播放
    {
        if (find_device(0))
        {
            put_msg_lifo(INFO_NEXTMODE);                //找不到有效设备，需要返回到其它模式
            return;
        }
    }

#ifdef UART_ENABLE
    printf(" ---> get_music_file1	%x \r\n",(u16)dir);
#endif
	
    if (!fs_get_filenum(play_mode, dir))                //查找错误，文件序号已经超出当前设备的范围(也有可能是当前设备已经不存在)
    {


	 if(repeat_off_flag){

#ifdef UART_ENABLE
		sys_printf(" REPEAT_OFF");
#endif
		folder_mode_select=0;

		repeat_off_flag =0;
		stop_decode();      
		Disp_Con(DISP_STOP);	

#ifdef USE_USB_SD_DECODE_FUNC	       
		if(folder_mode_select){
			
                	given_file_number = fs_msg.fileTotalOutDir + 1;
		}
		else
#endif			
		{
			given_file_number = 1;

		}

#ifdef USE_USB_PROG_PLAY_MODE
    		if(usb_prog_icon_bit){
				
			given_file_number =usb_prog_tab[0];
			usb_prog_mode_cls();			
    		}
#endif	
		//folder_mode_select=0;
		return;
	 }
	 
        if (find_device(0))                            //查找下一个设备
        {
            	put_msg_lifo(INFO_NEXTMODE);                //找不到有效设备，需要返回到其它模式
            	return;
        }
	    	Disp_Con(DISP_FILENUM);	

        if (given_file_number == 0)                     //given_file_number = 0;说明是后退找文件
            given_file_number = fs_msg.fileTotal;
    }
    put_msg_lifo(INIT_PLAY);
}
/*----------------------------------------------------------------------------*/
/**@brief   获取指定设备中的指定文件，如果条件不成立，自动找下一个可播放的文件
   @param   无
   @return  无
   @note    void get_music_file2(void)
*/
/*----------------------------------------------------------------------------*/
void get_music_file2(void)
{
    if (find_device(given_device)==0)
    {
	if ((given_file_number == 0) || (given_file_number > fs_msg.fileTotal))
        {
            given_file_number = 1;
        }

        put_msg_lifo(INIT_PLAY);
    }
    else
    {
		get_music_file3();
        
    }
}
/*----------------------------------------------------------------------------*/
/**@brief   在下一个设备中查找有效文件进行播放
   @param   无
   @return  无
   @note    void get_music_file3(void)
*/
/*----------------------------------------------------------------------------*/
void get_music_file3(void)
{
	if (find_device(0))
    {
        put_msg_lifo(INFO_NEXTMODE);                //找不到有效设备，需要返回到其它模式
		return;
    }
    put_msg_lifo(INIT_PLAY);
}
/*----------------------------------------------------------------------------*/
/**@brief   获取文件号
   @param   playmode   循环播放模式
   @param   searchMode  0：上一个文件；1：下一个文件；2：在强行REPEAT_ONE模式時在REPEAT_ALL模式下查找文件
   @return  0:fail 1:success
   @note     bool fs_get_filenum(PLAY_MODE playmode, u8 searchMode)
*/
/*----------------------------------------------------------------------------*/

bool fs_get_filenum(PLAY_MODE playmode, u8 searchMode)
{
    u16 fileTotal;

#ifdef UART_ENABLE
    printf(" ---> fs_get_filenum	%x \r\n",(u16)searchMode);
#endif

#ifdef USE_USB_PROG_PLAY_MODE
    if(usb_prog_icon_bit){
		return  get_prog_song_num(searchMode);
    }
#endif	
    fileTotal = fs_msg.fileTotal;

    given_file_number = fs_msg.fileNumber;


    if ((playmode == REPEAT_ONE) && (searchMode != 2))
        playmode = REPEAT_ALL;					//在单曲循环模式下，转换成全循环模式


    repeat_off_flag =0;

#ifdef USE_USB_SD_DECODE_FUNC	       

    if(folder_mode_select){

		if((playmode == REPEAT_ONE)){

		}
		else{
			playmode = REPEAT_FOLDER;
		}
    }
#endif
    switch (playmode)
    {
    case REPEAT_RANDOM:
        CRCREG = T3CNTH;
        CRCREG = T3CNTL;

        ((u8 *)(&given_file_number))[0] = ReadLFSR();
        ((u8 *)(&given_file_number))[1] = ReadLFSR();

        given_file_number = given_file_number % fs_msg.fileTotal + 1;
        break;

    case REPEAT_ALL:
        if (searchMode == 1)					//prev file
        {
            given_file_number--;
            if (given_file_number == 0)
            {
			if(0x03 != (0x03&get_device_online_status()))
			{
				clean_playpoint_info(device_active);
					return 0;
			}
			if(BIT(SDMMC) == device_active)
				clean_playpoint_info(BIT(USB_DISK));
                	if(BIT(USB_DISK) == device_active)
				clean_playpoint_info(BIT(SDMMC));

			return 0;
            }
        }
        else					                //next file
        {
            given_file_number++;
            if (given_file_number > fileTotal)
            {
			given_file_number = 1;
			if(0x03 != (0x03&get_device_online_status()))
			{
				clean_playpoint_info(device_active);
				return 0;
			}
                	else
			{
				 clean_playpoint_info(BIT(SDMMC));
				 clean_playpoint_info(BIT(USB_DISK));
			}

			return 0;
            }
        }
        break;

    case REPEAT_OFF:
        if (searchMode == 1)					//prev file
        {
            given_file_number--;
            if (given_file_number == 0)
            {
			if(0x03 != (0x03&get_device_online_status()))
			{
				clean_playpoint_info(device_active);
				return 0;
			}
			if(BIT(SDMMC) == device_active)
				clean_playpoint_info(BIT(USB_DISK));
                	if(BIT(USB_DISK) == device_active)
				clean_playpoint_info(BIT(SDMMC));

			return 0;
            }
        }
        else					                //next file
        {
            given_file_number++;
			
            if (given_file_number > fileTotal)
            {
            		given_file_number=1;
			if(0x03 != (0x03&get_device_online_status()))
			{
				clean_playpoint_info(device_active);
				
				if(searchMode==GET_PLAY_FILE){
					repeat_off_flag =1;
					return 0;		
				}
			}
                	else
			{
				clean_playpoint_info(BIT(SDMMC));
				clean_playpoint_info(BIT(USB_DISK));
			}

			if(searchMode==GET_PLAY_FILE){
				
				repeat_off_flag =1;
				return 0;		
			}
		}
        }
        break;	
    case REPEAT_FOLDER:
        if (searchMode == 1)
        {
            	given_file_number--;
            	if (given_file_number == fs_msg.fileTotalOutDir)
                	given_file_number = fs_msg.fileTotalOutDir + fs_msg.fileTotalInDir;
        }
        else					//next file
        {
            	given_file_number++;
            	if (given_file_number > (fs_msg.fileTotalOutDir + fs_msg.fileTotalInDir)){
                	given_file_number = fs_msg.fileTotalOutDir + 1;
				
		if((play_mode == REPEAT_OFF)&&(searchMode==GET_PLAY_FILE)){

			repeat_off_flag =1;
			given_file_number =1;
			return 0;		
		}
	     }
        }
        break;
    }

    return 1;
}
#if defined(USE_FOLDER_SELECT_FUNC)
void select_folder_file(u8 cmd)
{

    if (device_check() == 0)                    //如果当前播放的设备不在线，则找下一个设备, 并且选择第一个文件进行播放
    {
        if (find_device(0))
        {
            put_msg_lifo(INFO_NEXTMODE);                //找不到有效设备，需要返回到其它模式
            return;
        }
    }
    given_file_number= get_dir_file(cmd);                //查找错误，文件序号已经超出当前设备的范围(也有可能是当前设备已经不存在)
    put_msg_lifo(INIT_PLAY);
}
#endif



