/*--------------------------------------------------------------------------*/
/**@file      filesystem.c
   @brief     文件系统部分模块
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/

#include "filesystem.h"

extern u8 _xdata buffer[];  ///< 解码BUFF
_xdata u8 filename_buff[100] _at_ 0xd80;   ///< 文件名存放BUFF
/**存放有文件系统信息的结构体变量  */
FSAPIMSG _pdata fs_msg _at_ 0;
FIL _pdata fat_file _at_ 0x29; 
FIL _pdata * _xdata fat_file_maskrom _at_ (0x2380 + 0x62); 
FSINFO _xdata fs_info _at_ 0x5B0;
extern void stop_decode(void);
extern u8 music_type;
u16 filename_len;               ///< 最终提取的文件信息的长度
/*----------------------------------------------------------------------------*/
/**@brief     处理已经获取到的长文件名，当期转换为ASCII码
   @param   *p       ：  处理结果存放的BUFF的指针
   @param   *sou_p   ：  已经获取到的长文件名BUFF得指针
   @param    max_copy： 获取文件名长度的最大限制
   @return    无
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
                 if (2 == music_type)     //wav文件
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
/**@brief   获取文件名
   @param   *p : 提取的长文件名的存放BUFF的指针
   @return   1:获取的是长文件名；0：获取到的是短文件名
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
/**@brief   获取下一个文件夹的文件
   @param    cmd FIND_NEXT_DIR：查找下一个文件夹的起始文件;FIND_PREV_DIR:查找上一个文件夹的起始文件号
   @return   上/下一文件夹的起始文件号
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

