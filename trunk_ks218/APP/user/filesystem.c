/*--------------------------------------------------------------------------*/
/**@file      filesystem.c
   @brief     �ļ�ϵͳ����ģ��
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/

#include "filesystem.h"

extern u8 _xdata buffer[];  ///< ����BUFF
_xdata u8 filename_buff[100] _at_ 0xd80;   ///< �ļ������BUFF
/**������ļ�ϵͳ��Ϣ�Ľṹ�����  */
FSAPIMSG _pdata fs_msg _at_ 0;
FIL _pdata fat_file _at_ 0x29; 
FIL _pdata * _xdata fat_file_maskrom _at_ (0x2380 + 0x62); 
FSINFO _xdata fs_info _at_ 0x5B0;
extern void stop_decode(void);
extern u8 music_type;
u16 filename_len;               ///< ������ȡ���ļ���Ϣ�ĳ���
/*----------------------------------------------------------------------------*/
/**@brief     �����Ѿ���ȡ���ĳ��ļ���������ת��ΪASCII��
   @param   *p       ��  ��������ŵ�BUFF��ָ��
   @param   *sou_p   ��  �Ѿ���ȡ���ĳ��ļ���BUFF��ָ��
   @param    max_copy�� ��ȡ�ļ������ȵ��������
   @return    ��
   @note  void deal_longname(u8 _xdata *p,u8 _xdata *sou_p,u8 max_copy)
*/
/*----------------------------------------------------------------------------*/
void deal_longname(u8 _xdata *p,u8 _xdata *sou_p,u8 max_copy)
{
    u8 i;

    for (i = 0;i < max_copy;i++)
    {
        if (sou_p[i*2] || sou_p[i*2 +1])
        {
            if ((sou_p[i*2] >= 0x80) || (sou_p[i*2+1]))
            {
                p[i] = '*';
            }
            else if ((sou_p[i*2] < 0x80) && (0 == sou_p[i*2+1]))
            {
                p[i] = sou_p[i*2];
                 /**/
                 if (2 == music_type)     //wav�ļ�
                 {        
                      if((p[i-1] =='v') &&(p[i-2] =='a')&&(p[i-3] =='w')&&(p[i-4] =='.'))
                      {                       
                         p[i] = 0;
                      }  
                 }
                 else if(1 == music_type )
                 {
                      if((p[i-1] =='3') &&(p[i-2] =='p')&&(p[i-3] =='m')&&(p[i-4] =='.'))
                      {                       
                         p[i] = 0;
                      }  
                 }  
                if((fs_msg.fname[8]== 'S')&&(fs_msg.fname[9]== 'M')&&(fs_msg.fname[10]== 'P'))
                {
                     if((p[i-1] == 'p') &&(p[i-2] =='m')&&(p[i-3] =='s')&&(p[i-4] =='.'))
                     {                   
                           p[i] = 0;
                     }
                }

            }
            else
            {
                p[i] = '*';
            }
        }
        else
        {
            p[i] = 0;
            break;
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
bool get_filename( u8 _xdata *p )
{
    bool flag;
    u8 i;

    my_memset(buffer, 0, 100);
    my_memset(p, 0, 100);
#if FILE_ENCRYPTION
    password_start(0);
#endif
    flag = fs_getfile_longname(buffer);
#if FILE_ENCRYPTION
    if((fs_msg.fname[8]== 'S')&&(fs_msg.fname[9]== 'M')&&(fs_msg.fname[10]== 'P'))
    {
        password_start(1);
    }
#endif
    if (flag)
    {
        deal_longname(p,buffer,90);
        p[99] = 1;
        flag =  1;
    }
    else
    {
        my_memcpy(p,(u8 _xdata *)(fs_msg.fname),12);
        p[99] = 0;
        flag = 0;
    }

    for (i = 0;i < 90 ;i++)
    {
        if (!p[i])
        {
            break;
        }
    }
    filename_len = i;
    return flag;
}

/*----------------------------------------------------------------------------*/
/**@brief   ��ȡ��һ���ļ��е��ļ�
   @param    cmd FIND_NEXT_DIR��������һ���ļ��е���ʼ�ļ�;FIND_PREV_DIR:������һ���ļ��е���ʼ�ļ���
   @return   ��/��һ�ļ��е���ʼ�ļ���
   @note     u16 get_dir_file(u8 cmd)
*/
/*----------------------------------------------------------------------------*/
u16 get_dir_file(u8 cmd)
{
	u8 fileNumber;

	if (cmd == FIND_NEXT_DIR)
	{
		fileNumber = fs_msg.fileTotalInDir + fs_msg.fileTotalOutDir + 1;

	    if (fileNumber > fs_msg.fileTotal)
		{
			fileNumber = 1;	
		}
	}
	else if(cmd == FIND_PREV_DIR)
	{
		fileNumber = fs_msg.fileTotalOutDir;
		if (fileNumber == 0)
		{
			fileNumber = fs_msg.fileTotal;
			return fileNumber;
		}
		stop_decode();
		fs_getfile_bynumber(fileNumber);
		fileNumber -= (fs_msg.fileTotalInDir - 1); 		
	}
	return fileNumber;		
}

