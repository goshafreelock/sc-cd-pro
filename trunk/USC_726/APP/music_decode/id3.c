/*--------------------------------------------------------------------------*/
/**@file    id3.c
   @brief   ID3V2等文件信息头
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/

#include "id3.h"

/** 存放ID3V2信息的结构体变量 */
ID3V2_PARSE _pdata s_id3v2 _at_ 0xd0;

extern u8 _xdata buffer[];


#define id3_buffer (buffer + 512)	///<ID3中间BUFF


extern void ff_cal_id3v2_tag_len(const u8 _xdata * buf);
extern bool ff_id3v2_parse(u8 _xdata *buf, ID3V2_PARSE _pdata *id3);
extern bool ff_id3v2_check(u8 _xdata * buf, ID3V2_PARSE _pdata *id3);
extern u32 ff_get_id3v2_tag_len(void);
extern void ff_id3v2_get_tag(u8 *buf, u32 len);
extern u16 f_read (u8 _xdata *buffer, u16 len);
extern bool get_filename( u8 _xdata *p );
extern MAD_DECODE_INFO _pdata mad_decode_dsc;
extern u16 filename_len;
extern _xdata u8 filename_buff[];
extern u8 music_type;
extern FIL _pdata fat_file; 
#define MAX_ID3   3 ///<获取的ID3V2项目总数

static u8 id3_get;      ///<获取的ID3V2的标记变量，标记获取到了那些文件

u8 _code id3_tag[3][5] =
{
    {"TIT2"}, ///<标题
    {"TPE1"}, ///<艺术家
    {"TYER"} ///<年代
};   ///<需要查找获取的ID3V2信息

/*----------------------------------------------------------------------------*/
/**@brief    比较获取的帧头是否是需要查找的内容
   @param    * buf 需要比较的帧头
   @return   0：比较失败；其他（比较成功）1：标题  2：艺术家 3：年代
   @note     u8 compare_str(u8  * buf)
*/
/*----------------------------------------------------------------------------*/
u8 compare_str(u8  * buf) large
{
    u8 i,j;
    for (j = 0;j < MAX_ID3; j++)
    {
        for (i = 0;i < 4;i++)
        {
            if ( id3_tag[j][i] != buf[i] )
            {
                break;
            }
            else	if (i == 3)
            {
                id3_get |= BIT(j);
                //putchar(buf[0]);
                //putchar(buf[1]);
                //putchar(buf[2]);
                //putchar(buf[3]);
                return (j+1);
            }
        }
    }
    return 0;

}



/*----------------------------------------------------------------------------*/
/**@brief   文件系统偏移函数
   @param   offset    type
   @return  0:成功；其他：失败
   @note    u8 f_seek(u8 type, u32 offset)
*/
/*----------------------------------------------------------------------------*/
u8 f_seek(u8 type, u32 offset) large
{
    fs_seek_offset(offset);
    return fs_seek(type);
}


/*----------------------------------------------------------------------------*/
/**@brief   查找ID3V2的帧 ，并提起其中的有效信息
   @param   *buf:存放文件内容的BUF
   @param   len ： ID3V2的总长度
   @return  无
   @note    void ff_id3v2_get_tag(u8 *buf, u32 len)
*/
/*----------------------------------------------------------------------------*/
void ff_id3v2_get_tag(u8 *buf, u32 len)
{

#if 0
    u8 c;
    u8 t;
    u8 i;
    u8 cnt;
    u8 * _xdata p;
    //deg("%s:",buf);
    cnt = compare_str(buf);
    if (!cnt)// (buf[0] != 'T')    //not 'T' frame
    {
        f_seek(SEEK_CUR, len);
        return;
    }

    if (len > 80)
        t = 80;
    else
        t = len;

    f_read(buf, t);


    if ((t == 1) || (buf[0] != 0)) //内容长度不够  或则 内容格式不支持
    {
        id3_get &= ~BIT(cnt-1);
    }
    else
    {
        p = id3_buffer + ( (cnt-1) * 100 );
        cnt = 0;

        for (i = 1,cnt = 1; i < t; i++ )
        {
            c = buf[i];
            if ((c <= 0x7e) && (c >= 0x20))
            {
                p[cnt] = c;
            }
            else if (c > 0x80)
            {
                p[cnt] = '*';
                i++;
            }
            else
            {
                p[cnt] = '*';
            }
            cnt++;
        }
        p[0] = 	cnt -1;

    }
    f_seek(SEEK_CUR, len - t);
#endif	
}

/*----------------------------------------------------------------------------*/
/**@brief   获取文件信息 提取并拼接 ID3V2,当ID3V2有没有标题时，提起文件名
   @param   * buffer 提供给查找文件信息使用的BUFF
   @return  无
   @note    void get_filetag(u8 _xdata * buffer)
*/
/*----------------------------------------------------------------------------*/
void get_filetag(u8 _xdata * buffer)
{
#if 0
    u8 * _xdata p;
    id3_get = 0;
    my_memset(filename_buff, 0, 100);
    if (1 == music_type)	 ///<如果是MP3文件则 查找ID3V2
    {
        //printf("get id3v2\n");
        f_seek(SEEK_SET, 0);
        while (ff_id3v2_check(buffer, &s_id3v2)) //检测获取ID3头的头信息
        {
//	        printf("Find ID3 V2:%s\n",buffer);

            ff_cal_id3v2_tag_len(buffer);
            while (ff_id3v2_parse(buffer, &s_id3v2))
            {
                ff_id3v2_get_tag(buffer, s_id3v2.tlen);
            }
        }
        my_memset((u8 _xdata *)&mad_decode_dsc, 0, sizeof(MAD_DECODE_INFO)-7);
    }
	fat_file.flag = 0;
    if (!(id3_get & BIT(0))) ///标题
    {
        get_filename(filename_buff);
    }
    else
    {
        p = id3_buffer;
        my_memcpy(filename_buff,p+1,p[0]);
        filename_len = p[0];
    }


    if (id3_get & BIT(1))  //艺术家
    {
        p = id3_buffer + 100;
        if ((p[0] + filename_len + 1) > 98)
        {
            return;
        }
        filename_buff[filename_len] = '-';
        filename_len++;
        my_memcpy(filename_buff+filename_len,p+1,p[0]);
        filename_len += p[0];
    }

    if (id3_get & BIT(2))	//年代
    {
        p = id3_buffer + 200;
        if ((p[0] + filename_len + 1) > 98)
        {
            return;
        }
        filename_buff[filename_len] = '-';
        filename_len++;
        my_memcpy(filename_buff + filename_len,p+1,p[0]);
        filename_len += p[0];
    }
#endif
}