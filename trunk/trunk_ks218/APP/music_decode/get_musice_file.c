/*--------------------------------------------------------------------------*/
/**@file       get_musice_file.c
   @brief      �ļ�����
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
extern xd_u8 play_status;

extern bool play_sel_flag;

#ifdef USB_STOP_MODE_AFTER_TOC
extern bool toc_ready_stop;
#endif			

#ifdef USE_USB_SD_DECODE_FUNC	       
extern bool folder_mode_select;
#endif
bool repeat_off_flag=0;

#ifdef USE_USB_PROG_PLAY_MODE
extern bool usb_prog_icon_bit;
extern xd_u8 usb_prog_tab[USB_PROG_MAX];
extern bool get_prog_song_num(u8 get_Mode);
#endif

/*----------------------------------------------------------------------------*/
/**@brief   ��ȡһ�������
   @param   ��
   @return  ��ȡ���������
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

/*----------------------------------------------------------------------------*/
/**@brief    ������һ������һ���ļ������Զ���ȡ��һ��
   @param    dir��0:next file, 1:previous file, 2:auto next,���ܻᵼ���豸���л�
   @return   ��
   @note     void get_music_file1(u8 dir)
*/
/*----------------------------------------------------------------------------*/
void get_music_file1(u8 dir)
{


	if (device_check() == 0)                    //�����ǰ���ŵ��豸�����ߣ�������һ���豸, ����ѡ���һ���ļ����в���
    {
        if (find_device(0))
        {
            put_msg_lifo(INFO_NEXTMODE);                //�Ҳ�����Ч�豸����Ҫ���ص�����ģʽ
            return;
        }
    }

#ifdef GETFILE_UART_ENABLE
    printf(" ---> get_music_file1	%x \r\n",(u16)dir);
#endif
	
    if (!fs_get_filenum(play_mode, dir))                //���Ҵ����ļ�����Ѿ�������ǰ�豸�ķ�Χ(Ҳ�п����ǵ�ǰ�豸�Ѿ�������)
    {


	 if(repeat_off_flag){

#ifdef GETFILE_UART_ENABLE
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
    		}
#endif	
		//folder_mode_select=0;
		return;
	 }
	 
        if (find_device(0))                            //������һ���豸
        {
            	put_msg_lifo(INFO_NEXTMODE);                //�Ҳ�����Ч�豸����Ҫ���ص�����ģʽ
            	return;
        }
	    	Disp_Con(DISP_FILENUM);	

        if (given_file_number == 0)                     //given_file_number = 0;˵���Ǻ������ļ�
            given_file_number = fs_msg.fileTotal;
    }

	if(play_status == MUSIC_STOP){

	    	Disp_Con(DISP_FILENUM);	
	}
	else if(play_status == MUSIC_PAUSE){

		play_sel_flag=1;
		fs_getfile_bynumber(given_file_number);
	    	Disp_Con(DISP_FILENUM);	
		
#ifdef GETFILE_UART_ENABLE
    printf(" ---> given_file_number	%x \r\n",(u16)given_file_number);
#endif

	}
	else{

		put_msg_lifo(INIT_PLAY);
		
	}
}
/*----------------------------------------------------------------------------*/
/**@brief   ��ȡָ���豸�е�ָ���ļ�������������������Զ�����һ���ɲ��ŵ��ļ�
   @param   ��
   @return  ��
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
/**@brief   ����һ���豸�в�����Ч�ļ����в���
   @param   ��
   @return  ��
   @note    void get_music_file3(void)
*/
/*----------------------------------------------------------------------------*/
void get_music_file3(void)
{
	//if (find_device(0))
    //{
        put_msg_lifo(INFO_NEXTMODE);                //�Ҳ�����Ч�豸����Ҫ���ص�����ģʽ
	//	return;
   // }
    //put_msg_lifo(INIT_PLAY);
}
/*----------------------------------------------------------------------------*/
/**@brief   ��ȡ�ļ���
   @param   playmode   ѭ������ģʽ
   @param   searchMode  0����һ���ļ���1����һ���ļ���2����ǿ��REPEAT_ONEģʽ�r��REPEAT_ALLģʽ�²����ļ�
   @return  0:fail 1:success
   @note     bool fs_get_filenum(PLAY_MODE playmode, u8 searchMode)
*/
/*----------------------------------------------------------------------------*/

bool fs_get_filenum(PLAY_MODE playmode, u8 searchMode)
{
    u16 fileTotal;


#ifdef USE_USB_PROG_PLAY_MODE
    if(usb_prog_icon_bit){
		return  get_prog_song_num(searchMode);
    }
#endif	
    fileTotal = fs_msg.fileTotal;

   // given_file_number = fs_msg.fileNumber;

#ifdef GETFILE_UART_ENABLE
    printf(" ---> PLAY_MODE	%x \r\n",(u16)playmode);
    printf(" ---> fs_msg.fileNumber	%4u \r\n",(u16)fs_msg.fileNumber);
    printf(" ---> given_file_number	%4u \r\n",(u16)given_file_number);
    printf(" ---> fs_msg.fileTotal	%4u \r\n",(u16)fs_msg.fileTotal);
#endif

    if ((playmode == REPEAT_ONE) && (searchMode != 2))
        playmode = REPEAT_ALL;					//�ڵ���ѭ��ģʽ�£�ת����ȫѭ��ģʽ


    repeat_off_flag =0;

#ifdef USE_USB_SD_DECODE_FUNC	       
#if 0
    if(folder_mode_select){

		if((playmode == REPEAT_ONE)){

		}
		else{
			playmode = REPEAT_FOLDER;
		}
    }
#endif
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

#ifdef GETFILE_UART_ENABLE
	       printf(" ----->REPEAT_ALL  -- \r\n");
#endif
		
        if (searchMode == 1)					//prev file
        {
            given_file_number--;
            if (given_file_number == 0)
            {
#if 0            
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
#endif
			given_file_number=fileTotal;

            }
        }
        else					                //next file
        {
            given_file_number++;
            if (given_file_number > fileTotal)
            {
			given_file_number = 1;
#if 0			
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
#endif
            }
        }
        break;

    case REPEAT_OFF:

#ifdef GETFILE_UART_ENABLE
	       printf(" ----->REPEAT_OFF  -- \r\n");
#endif
		
        if (searchMode == 1)					//prev file
        {
            given_file_number--;
            if (given_file_number == 0)
            {            

#if 0            
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
#endif
			given_file_number=fileTotal;

            }
        }
        else					                //next file
        {
        
            given_file_number++;
            if (given_file_number > fileTotal)
            {
#if 0            
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
#endif
			given_file_number =1;

			if(searchMode==GET_PLAY_FILE){
				repeat_off_flag =1;
				return 0;		
			}
		}
        }
        break;	
    case REPEAT_FOLDER:
#ifdef GETFILE_UART_ENABLE
	       printf(" ----->REPEAT_FOLDER  -- \r\n");
#endif
		
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

    if (device_check() == 0)                    //�����ǰ���ŵ��豸�����ߣ�������һ���豸, ����ѡ���һ���ļ����в���
    {
        if (find_device(0))
        {
            put_msg_lifo(INFO_NEXTMODE);                //�Ҳ�����Ч�豸����Ҫ���ص�����ģʽ
            return;
        }
    }
    given_file_number= get_dir_file(cmd);                //���Ҵ����ļ�����Ѿ�������ǰ�豸�ķ�Χ(Ҳ�п����ǵ�ǰ�豸�Ѿ�������)

#ifdef GETFILE_UART_ENABLE
    printf(" ---> select_folder_file	given_file_number  :%4u \r\n",(u16)given_file_number);
    printf(" ---> fs_msg.dirTotal		%4u \r\n",(u16)fs_msg.dirTotal);
#endif

    if((play_status == MUSIC_STOP)||(play_status==MUSIC_PAUSE)){

		fs_getfile_bynumber(given_file_number);
		
		 Disp_Con(DISP_DIR);	
    }
    else{
		
   	 put_msg_lifo(INIT_PLAY);
    }
}
#endif



