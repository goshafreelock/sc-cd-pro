/*--------------------------------------------------------------------------*/
/**@file     rtc_fun.c
   @brief    RTC�㷨���ܺ���
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#include "config.h"
#if RTC_ENABLE
#include "rtc_fun.h"


extern RTC_TIME curr_date;
extern ALM_TIME curr_alm;

extern xd_u8 rtc_set;
extern xd_u8 alm_set;
extern xd_u8 rtc_mode;
extern bool alm_sw;

_code const u8 month_tab1[] = {31,28,31,30,31,30,31,31,30,31,30,31};
_code const u8 month_tab2[] = {31,29,31,30,31,30,31,31,30,31,30,31};


//                        1  2  3  4   5   6   7   8   9   10  11  12
_code const u16 smonth_tab1[]={0,31,59,90,120,151,181,212,243,273,304,334,365};
_code const u16 smonth_tab2[]={0,31,60,91,121,152,182,213,244,274,305,335,366};



/*----------------------------------------------------------------------------*/
/**@brief �����ж�
   @param 	year�����
   @return  TRUE��FALSE
   @note u8 leapyear(u16 year)
*/
/*----------------------------------------------------------------------------*/
u8 leapyear(u16 year)
{
    if ((year%4==0 && year%100!=0)||(year%400==0))
        return TRUE;
    else
        return FALSE;
}

/*----------------------------------------------------------------------------*/
/**@brief ���������
   @param 	nyear���������
   @return  ��������
   @note u16 nowyear(u16 nyear)
*/
/*----------------------------------------------------------------------------*/
u16 nowyear(u16 nyear)
{
    return leapyear(nyear)+365;		//365��5Сʱ48��46�룬ÿ4���һ�죬���0.072�죬ÿ�İ����3��
}
/*----------------------------------------------------------------------------*/
/**@brief ���µ�����
   @param 	nmonth�������·ݣ�nyear���������
   @return  ��������
   @note u8 nowmonth(u8 nmonth,u16 nyear)
*/
/*----------------------------------------------------------------------------*/
u8 nowmonth(u8 nmonth,u16 nyear)
{
    if (leapyear(nyear))
        return month_tab2[nmonth-1];
    else
        return month_tab1[nmonth-1];

}

/*----------------------------------------------------------------------------*/
/**@brief ����ת����
   @param 	*now_time��ת�������ڣ�sec����Ҫд�������
   @return  void
   @note   void sec_date(RTC_TIME *now_time,u32 sec)
*/
/*----------------------------------------------------------------------------*/
void sec_date(RTC_TIME *now_time,u32 sec) large
{

    u16 temp_day;
    u32 temp_sec;
    u8 itmp;

    temp_day=sec/ONEDAY;							//��->��
    temp_sec=sec%ONEDAY;							//һ�����µ��� u32 second<24*60*60

    if (!temp_day)
    {
        now_time->date = SDATE;
    }
    else
    {
        now_time->date = (temp_day - 1)%7;
    }

    itmp = 0;
    while (temp_day>=nowyear(YEAR+itmp))				//��->��
    {
        temp_day-=nowyear(YEAR+itmp);				//��ȥ��������
        itmp++;
    }
    now_time->year=YEAR+itmp;

    itmp = 0;
    while (temp_day>=nowmonth(MONTH+itmp,now_time->year))			//��->��
    {
        temp_day-=nowmonth(MONTH+itmp,now_time->year);				//��ȥ��������
        itmp++;
    }
    now_time->month=MONTH+itmp;

    now_time->day=temp_day+DAY;

    //isecond=;
    now_time->second=SECOND+temp_sec%60;

    itmp=temp_sec/3600;
    now_time->hour=HOUR+itmp;

    //iminutes=temp_sec/60;
    //iminutes-=ihour*60;
    now_time->minutes=MINUTES + temp_sec/60 - itmp*60;
}
/*----------------------------------------------------------------------------*/
/**@brief ����ת����
   @param 	*time��ת�������ڣ�flag�����ӱ�־
   @return  void
   @note   u32 date_sec(RTC_TIME *time,bit flag)
*/
/*----------------------------------------------------------------------------*/
u32 date_sec(RTC_TIME *time,bit flag) large
{
    u16 y_i;
    u32 sec;
    u32 temp_day=0;
    for (y_i=YEAR;y_i<time->year;y_i++)			//��->�գ����겻�����ڣ�������<
    {
        temp_day+=nowyear(y_i);
    }

    if (leapyear(time->year))						//��->��
        temp_day+=smonth_tab2[time->month-1];
    else
        temp_day+=smonth_tab1[time->month-1];

    temp_day+=(time->day-1);						//��->��,���ղ������ڣ�������Ӧ�ü�1

    sec = temp_day*ONEDAY;						//��->��

    sec += (u32)time->hour*ONEHOUR;			//ʱ->��,
    sec += time->minutes*ONEMIN;				//��->��,
    if (flag)
    {
        sec+=time->second;
    }
    return sec;
}
/*----------------------------------------------------------------------------*/
/**@brief ��ǰ���ڵļӲ���
   @param 	void
   @return  void
   @note  void curr_date_plus()
*/
/*----------------------------------------------------------------------------*/
void curr_date_plus()
{
    if (rtc_set==0)
    {
        curr_date.year+=1;
        if (curr_date.year>2135)	curr_date.year=2000;
    }
    if (rtc_set==1)
    {
        curr_date.month+=1;
        if (curr_date.month>12)	curr_date.month=1;
    }
    if (rtc_set==2)
    {
        curr_date.day+=1;
        if (curr_date.day>nowmonth(curr_date.month,curr_date.year))	curr_date.day=1;
    }

    if (rtc_set==3)
    {
        curr_date.hour+=1;
        if (curr_date.hour>23)	curr_date.hour=0;
    }
    if (rtc_set==4)
    {
        curr_date.minutes+=1;
        if (curr_date.minutes>59)	curr_date.minutes=0;
    }
    if (rtc_set==5)
    {
        curr_date.second+=1;
        if (curr_date.second>59)	curr_date.second=0;
    }

}
/*----------------------------------------------------------------------------*/
/**@brief ��ǰ���ڵļ�����
   @param 	void
   @return  void
   @note   void curr_date_minus()
*/
/*----------------------------------------------------------------------------*/
void curr_date_minus()
{
    if (rtc_set==0)//if(lcd_x==YEARCOM)		//curr_date.year-=1;
    {
        curr_date.year-=1;
        if (curr_date.year<2000)	curr_date.year=2135;
    }
    if (rtc_set==1)//if(lcd_x==MONTHCOM)	//curr_date.month-=1;
    {
        curr_date.month-=1;
        if (curr_date.month<1)	curr_date.month=12;
    }
    if (rtc_set==2)//if(lcd_x==DAYCOM)	//curr_date.day-=1;
    {
        curr_date.day-=1;
        if (curr_date.day<1)	curr_date.day=nowmonth(curr_date.month,curr_date.year);
    }

    if (rtc_set==3)//if((lcd_x==HOURCOM)||(lcd_x==HOURACOM))		//curr_date.hour-=1;
    {
        if (curr_date.hour<1)	curr_date.hour=24;
        curr_date.hour-=1;
    }
    if (rtc_set==4)//if((lcd_x==MINCOM)||(lcd_x==MINACOM))	//curr_date.minutes-=1;
    {
        if (curr_date.minutes<1)	curr_date.minutes=60;
        curr_date.minutes-=1;
    }
    if (rtc_set==5)//if(lcd_x==SECCOM)	//curr_date.second-=1;
    {
        if (curr_date.second<1)	curr_date.second=60;
        curr_date.second-=1;
    }
}
/*----------------------------------------------------------------------------*/
/**@brief ���ӵļӲ���
   @param 	void
   @return  void
   @note   void alm_time_plus()
*/
/*----------------------------------------------------------------------------*/
void alm_time_plus()
{
    if (alm_set==1)
        alm_sw=~alm_sw;
    if (alm_set==2)//if((lcd_x==HOURCOM)||(lcd_x==(LCDCOLUMN-5*8)/2+0))		//curr_alm.hour+=1;
    {
        curr_alm.hour+=1;
        if (curr_alm.hour>23)	curr_alm.hour=0;
    }
    if (alm_set==3)//if((lcd_x==MINCOM)||(lcd_x==(LCDCOLUMN-5*8)/2+24))	//curr_alm.minutes+=1;
    {
        curr_alm.minutes+=1;
        if (curr_alm.minutes>59)	curr_alm.minutes=0;
    }

}
/*----------------------------------------------------------------------------*/
/**@brief ���ӵļ�����
   @param 	void
   @return  void
   @note void alm_time_minus()
*/
/*----------------------------------------------------------------------------*/
void alm_time_minus()
{

    if (alm_set==1)
        alm_sw=~alm_sw;
    if (alm_set==2)	//curr_alm.hour-=1;
    {
        if (curr_alm.hour<1)	curr_alm.hour=24;
        curr_alm.hour-=1;
    }
    if (alm_set==3)	//curr_alm.minutes-=1;
    {
        if (curr_alm.minutes<1)	curr_alm.minutes=60;
        curr_alm.minutes-=1;
    }
}
#endif