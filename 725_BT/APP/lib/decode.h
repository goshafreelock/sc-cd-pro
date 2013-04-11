/*--------------------------------------------------------------------------*/
/**@file     decode.h
   @brief    ����ͷ�ļ�
   @details
   @author
   @date   2010-12-13
   @note
*/
/*----------------------------------------------------------------------------*/

#ifndef __DEOCDE_H__
#define __DEOCDE_H__


#ifdef __C51__
#include "config.h"

/** @brief �����ж� */
#define	enable_softint()		(IE0 |= BIT(6))

/** @brief �ر����ж� */
#define	disable_softint()		(IE0 &= ~BIT(6))

/** @brief ���ý����жϸ����ȼ� */
#define decodeint_hi_pro()    (IP1 |= BIT(5))

/** @brief �������ж� */
#define enable_decodeint()    (IE1 |= BIT(5))

/** @brief �رս����ж� */
#define disable_decodeint()   (IE1 &= ~BIT(5))

/** @brief ����DSP�жϸ����ȼ� */
#define dsp_hi_pro()        (IP1 |= BIT(6))

/** ��DSP�ж� */
#define enable_dspint()    (IE1 |= BIT(6))

/** @brief ��DSP�ж� */
#define disable_dspint()   (IE1 &= ~(BIT(6)))

/*----------------------------------------------------------------------------*/
/**@brief ��õ�ǰ�����ŵ�MP3�ļ���Ƶ�գ���WAV�ļ���Ч��������ͣʱҲ��Ч
   @param buf:���������ָ�룬bufΪ u16 buf[10]���õ�����10��Ƶ���ߣ�buf[0]~buf[9]Ϊ�ӵ͵���
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void freqavg(u16 _xdata *buf);

/** MAD_DECODE_INFO */
typedef struct _MAD_DECODE_INFO
{
    u8  err;                 /**< ���� */
    u8  err_counter;         /**< ������� */
    u32 frame_counter;       /**< */
    u32 total_time;          /**< ��ʱ�� */
    u32 id3v2_len;           /**< ID3V2�ĳ��� */
    u32 average_bit_rate;    /**< ƽ�������� */
    s32 target_frame;        /**< ~ */
    u32 this_frame;          /**< ~ */
    u32 next_frame;          /**< ~ */
    u8 * eq_table;           /**<������Ч��ָ�� */ ///<
}
/** �������� MAD_DECODE_INFO */
MAD_DECODE_INFO;

/** �������ö�� */
enum 							//config command
{
    MAD_DECODE_INIT = 0,          /**< �����ʼ�� */
    MAD_DECODE_RESET,             /**< ���븴λ */
    MAD_FAST_FORWARD_REVERASE,    /**< ������� */
    MAD_STOP,                     /**< ����ֹͣ */
    MAD_PLAY,                     /**< ������Ŀ���� */
    MAD_PAUSE,                    /**< ������ͣ */
    MAD_GET_TOTAL_TIME,           /**< ��ȡ��ǰ�ļ���ʱ�� */
    MAD_GET_PLAY_TIME,            /**< ��ȡ��ǰ�ļ�ʵ���Ѿ����ŵ�ʱ�� */
    MAD_GET_STATUS,               /**< ��ȡ����״̬ */
    MAD_GET_FRAME,				/**< get current frame counter */
    MAD_SET_FRAME,				/**< set target frame counter */
    MAD_IDLE,
};          //�������ö��

enum
{
    MAD_ERROR_NONE	   = 0x00,	/**< no error */
    MAD_ERROR_BUFFER_LEN         = 0x01,

    MAD_ERROR_NEXT_FRAME      = 0x02,

    MAD_ERROR_LOSTSYNC	   = 0x11,	/**< lost synchronization */
    MAD_ERROR_BADLAYER	   = 0x12,	/**< reserved header layer value */
    MAD_ERROR_BADBITRATE	   = 0x13,	/**< forbidden bitrate value */
    MAD_ERROR_BADSAMPLERATE  = 0x14,	/**< reserved sample frequency value */
    MAD_ERROR_BADEMPHASIS	   = 0x15,	/**< reserved emphasis value */

    MAD_ERROR_BADCRC	   = 0x21,	/**< CRC check failed */
    MAD_ERROR_BADBITALLOC	   = 0x022,	/**< forbidden bit allocation value */
    MAD_ERROR_BADSCALEFACTOR = 0x23,	/**< bad scalefactor index */
    MAD_ERROR_BADMODE        = 0x24,	/**< bad bitrate/mode combination */
    MAD_ERROR_BADFRAMELEN	   = 0x25,	/**< bad frame length */
    MAD_ERROR_BADBIGVALUES   = 0x26,	/**< bad big_values count */
    MAD_ERROR_BADBLOCKTYPE   = 0x27,	/**< reserved block_type */
    MAD_ERROR_BADSCFSI	   = 0x28,	/**< bad scalefactor selection info */
    MAD_ERROR_BADDATAPTR	   = 0x29,	/**< bad main_data_begin pointer */
    MAD_ERROR_BADPART3LEN	   = 0x2A,	/**< bad audio data length */
    MAD_ERROR_BADHUFFTABLE   = 0x2B,	/**< bad Huffman table select */
    MAD_ERROR_BADHUFFDATA	   = 0x2C,	/**< Huffman data overrun */
    MAD_ERROR_BADSTEREO	   = 0x2D,	/**< incompatible block_type for JS */

    MAD_ERROR_FILE_END         = 0x40,
    MAD_ERROR_DISK_ERR         = 0x41,
    MAD_ERROR_SYNC_LIMIT       = 0x42,
    MAD_ERROR_FF_FR_FINISH     = 0x43,
    MAD_ERROR_FF_FR_END        = 0x44,
    MAD_ERROR_FF_FR_START      = 0x45,
    MAD_ERROR_LIMIT            = 0x46,
	MAD_USER_EXIT			   = 0x47,	/*�û������˳�����*/
};                 ///<������ö��

typedef struct _PLAYPOINT_TIME
{
	u8 last_device;
	u32 next_frame;
	u32 frame_cnt;
}
PLAYPOINT_TIME;

void save_playpoint(u8 cnt);
void load_playpoint(void);
void write_dev_playtime(u8 dev) large;
void clean_playpoint(u8 dev);
void write_playpoint_info(u8 dev);
u16 read_playpoint_info(u8 dev);
void clean_playpoint_info(u8 dev);
void update_playpoint(PLAYPOINT_TIME *ptr);
void write_playtime(PLAYPOINT_TIME *ptr);
#endif


#endif