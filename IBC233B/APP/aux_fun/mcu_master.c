/*--------------------------------------------------------------------------*/
/**@file     mcu_master.c
   @brief    mcu_master任务模块
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#include "Custom_config.h"
#include "mcu_master.h"
#include "fat_memory.h"
#include "voice_time.h"
#include "lcdsegment.h"

#ifdef USE_CD_MCU_MASTER_FUNC			

#include "iic_drv.h"

bool mcu_master_tranceive_tick=0;

xd_u8 cd_play_status=0xFF;

extern xd_u16 given_file_number;
extern xd_u16 cfilenum;
extern xd_u8 curr_menu;
extern xd_u8 play_mode;
extern xd_u8  return_cnt;
extern bit aux_online;
extern xd_u8 work_mode;
extern _idata u16 dac_cnt;
extern bool key_voice_disable;
extern void chk_date_err(void);
extern u8 xdata last_work_mode;
extern bool alarm_on;
extern xd_u8 play_disp_scr;
extern xd_u8 my_music_vol;
extern bool IR_key_det,adkey_detect;

bool next_prev_key=0,play_key=0;
TOC_TIME cur_time;
bool toc_flag=0,send_buf_cmd=0,fisrt_time_op=0,door_open=0;
xd_u8 fast_fr_release_cnt=0;
xd_u16 send_buf=0;
xd_u8 rev_buf[10]={0};


#ifdef USE_PROG_PLAY_MODE
bool play_prog_mode=0,prog_icon_bit=0,prog_disp_srn=0;
xd_u8 prog_total_num=0,prog_cur_num=0,prog_first_num=0;

#endif
void prog_play_clear(void);

void master_push_cmd(u8 cmd)
{
	u16 cmd_reg=0;
	
	send_buf_cmd=1;

	cmd_reg=cmd;
	if(send_buf==0x0000){
		send_buf=cmd_reg;
	}
	else if(((send_buf&0xFF00)==0)&&((send_buf&0x00FF)>0)){

		send_buf|=cmd_reg<<8;
	}	
#ifdef CD_UART_ENABLE	
	printf("----------------->> master_push_cmd   %x    \r\n",send_buf);
#endif
}
u8 master_pop_cmd(void)
{
	u8 r_reg=0;
	
	if(send_buf>0){
		r_reg = (u8)(send_buf&0x00FF);
		send_buf = send_buf>>8;			

		if(send_buf==0){
			send_buf_cmd=0;
		}
#ifdef CD_UART_ENABLE	
	   	printf("----------------->> master_pop_cmd   %x     \r\n",(u16)r_reg);	
#endif
	}
	return r_reg;
}
void clr_rev_buf()
{
    my_memset(&rev_buf[0], 0x0, 10);
}
static u8 info_timer_1=0,info_timer_2=0,info_timer_3=0;
static u8 info_timer_play=0,info_timer_stop=0,info_timer_4=0;
void clr_flick_timer()
{

 	info_timer_1=0;
	//info_timer_2=0;
	info_timer_3=0;

	info_timer_play=0;
	info_timer_stop=0;
	info_timer_4=0;

	cfilenum=0;	
	rev_buf[5]=0;
}
void mcu_master_init()
{
	cd_play_status=0;
	send_buf_cmd=0;
	send_buf = 0x0000;
	toc_flag=0;
	cur_time.MIN=0;
	cur_time.SEC=0;
	cfilenum =0;
	given_file_number=1;

	next_prev_key=0;
	prog_first_num =0;
	
 	info_timer_1=0;
	info_timer_2=0;
	info_timer_3=0;

	info_timer_play=0;
	info_timer_stop=0;
	info_timer_4=0;
	
    	play_mode=REPEAT_OFF;
    	master_push_cmd(REP_OFF_CMD);	
	clr_rev_buf();		
}
void mcu_master_info_hdlr()
{
	static u8 info_dispatch_div=0;
	u8 rev_loop=0;
	if(fast_fr_release_cnt>0){

		fast_fr_release_cnt--;
		if(fast_fr_release_cnt==0){
			master_push_cmd(FFR_OFF_CMD);
		}
	}

	info_dispatch_div++;
	if(info_dispatch_div==5){
		info_dispatch_div=0;

//4 TOC 
		if((rev_buf[1]&(BIT(2)))==0){

			if((!door_open)){
#ifdef DISP_TOC_BAR
				if(curr_menu != DISP_SCAN_TOC){
					Disp_Con(DISP_SCAN_TOC);
				}
				else{
					Disp_Con(DISP_NULL);
				}
#else
				if(curr_menu != DISP_SCAN_DISK){
					Disp_Con(DISP_SCAN_DISK);
				}
#endif			
				cfilenum=0;
				toc_flag=0;
			}
		}
		else{

			door_open=0;
			toc_flag=1;
		}
		
//4 get cur play status 	
		if(toc_flag){
			
			if((rev_buf[0]&0x03)==0x02){

					info_timer_play++;
					
					if(info_timer_play>2){
						
						if((cd_play_status == MUSIC_PAUSE)&&(next_prev_key)){
							
							master_push_cmd(PAUSE_CMD);
						}	
						else{
							
						if(cd_play_status!=MUSIC_PLAY)
						      Mute_Ext_PA(UNMUTE);							
							cd_play_status=MUSIC_PLAY;
						}
					}
			}
			else if((rev_buf[0]&0x03)==0x00){

				info_timer_stop++;
				
				if(info_timer_stop>2){
					if(cd_play_status!=MUSIC_STOP){
						cd_play_status=MUSIC_STOP;
					}
				}
				
				//info_timer_3++;
				//if((curr_menu != DISP_STOP)&&(info_timer_3>12))
				//	Disp_Con(DISP_STOP);			
			}
		}
#ifdef USE_PROG_PLAY_MODE
		if((rev_buf[0]&(BIT(5)))>0){

			info_timer_3++;
			if(info_timer_3>2){
				prog_icon_bit=1;
			}
		}
		else{

 			 info_timer_3=0;
			 prog_icon_bit=0;

		}
#endif


//4 DOOR STATUS
		if((rev_buf[1]&(BIT(1)))>0){

			if((curr_menu != DISP_OPEN)&&(info_timer_2>2)){

				Disp_Con(DISP_OPEN);
				cfilenum=0;
				door_open=1;
				info_timer_2=0;
			   	Mute_Ext_PA(MUTE);
				toc_flag=0;
				prog_play_clear();
				prog_icon_bit=0;
				play_prog_mode=0;
				clr_flick_timer();
			    	play_mode=REPEAT_OFF;
			    	master_push_cmd(REP_OFF_CMD);					
			}
			else{
				info_timer_2++;
			}
		}
		else{

			door_open=0;			
			info_timer_2=0;
		}

//4 check disk
		if((rev_buf[1]&(BIT(4)))>0){

			info_timer_1++;
			
			if((curr_menu != DISP_NODEVICE)&&(info_timer_1>2))
				Disp_Con(DISP_NODEVICE);
		}
		else{
			info_timer_1=0;
		}
		
//4 curent playing file index
		if((rev_buf[2]>0)&&(rev_buf[2]!=0xFF)){

			if(given_file_number!=rev_buf[2]){
				given_file_number=rev_buf[2];
				
#ifdef USE_PROG_PLAY_MODE
				if(play_prog_mode){
					if(prog_cur_num!=rev_buf[2]){
						prog_cur_num=rev_buf[2];
			                    		Disp_Con(DISP_PROG_FILENUM);
					}
				}
				else
#endif			
				{
					//if(play_key){
					//	play_key=0;
					//}
					//else 
					if(toc_flag){
						
						if(cd_play_status!=MUSIC_STOP||(next_prev_key)){

							play_disp_scr=0;
							//if(next_prev_key)next_prev_key=0;
			                    		Disp_Con(DISP_FILENUM);	
						}
					}
				}
			}
		}

//4 curent total file
		if((rev_buf[5]>0)&&(rev_buf[5]!=0xFF)){

			if(((rev_buf[0]&0x03)==0x00)&&toc_flag){

				if(cfilenum!=rev_buf[5]){
					cfilenum=rev_buf[5];
					if(cfilenum>0)
	                    		Disp_Con(DISP_DWORD_NUMBER);

				}
			}
		}
#ifdef USE_PROG_PLAY_MODE
		if(play_prog_mode&&(rev_buf[0]&(BIT(6)))){

			if(rev_buf[8]!=0xff){
				if(rev_buf[8]!=prog_total_num){
					prog_total_num = rev_buf[8];
					Disp_Con(DISP_PROG_FILENUM);				
				}
			}
			
			if(rev_buf[9]!=0xff){			
				if(rev_buf[9]!=prog_cur_num){
					prog_disp_srn=0;					
					prog_cur_num = rev_buf[9];
					if(prog_cur_num>0)
					Disp_Con(DISP_PROG_FILENUM);				
				}
			}
		}
#endif
//4 total file time
		if(rev_buf[6]<59){
			//printf("--------------6666------------%x   \r\n",(u16)rev_buf[6]);
		}
		if(rev_buf[7]<59){
			//printf("--------------7777------------%x   \r\n",(u16)rev_buf[7]);
		}
		
//4 get cur play time		
		if(rev_buf[3]<59){
			cur_time.MIN=rev_buf[3];
			//printf("--------------MIN------------%u  \r\n",(u16)cur_time.MIN);
			
		}
		if(rev_buf[4]<59){
			cur_time.SEC=rev_buf[4];
			//printf("--------------SEC------------%u   \r\n",(u16)cur_time.SEC);
			
		}
//4 end  get cur play time		
		
	//for(rev_loop=0;rev_loop<9;rev_loop++)
		//printf("----------------------------------%x  ----rev  %x \r\n",(u16)rev_loop,(u16)rev_buf[rev_loop]);
		//printf("----------------------------------%x  ----rev  %x \r\n",(u16)rev_loop,(u16)rev_buf[0]);
		//printf("----------------------------------9  ----rev  %u \r\n",(u16)rev_buf[8]);
		//printf("---------------------------------A  ----rev  %u \r\n",(u16)rev_buf[9]);
		//printf("---------------------------------B ----rev  %x \r\n",(u16)rev_buf[0]);
		//clr_rev_buf();
	}
}
void mcu_master_rev()
{
	u8 rev_loop=0,rev_reg=0;
	//bool ack1,ack2;
	//EA =0;
	iic_drv_start();
	iic_drv_sendbyte(MASTER_READ_CMD);
	_nop_();_nop_();_nop_();
	while(rev_loop<10){

    		rev_reg = iic_drv_revbyte(1);
		rev_buf[rev_loop]=rev_reg;
		rev_loop++;
	}
	iic_drv_stop();	
	//EA =1;
	//for(rev_loop=0;rev_loop<9;rev_loop++)
		//printf("----------------------------------%x  ----rev  %x \r\n",(u16)rev_loop,(u16)rev_buf[rev_loop]);

	//for(rev_loop=0;rev_loop<9;rev_loop++)
		//printf("----------------------------------%x  ----rev  %x \r\n",(u16)rev_loop,(u16)rev_buf[rev_loop]);

    	//printf("---111-mcu_master_rev   %x \r\n",(u16)ack1);
    	//printf("-----------222-mcu_master_rev   %x \r\n",(u16)ack2);		
}
void mcu_master_send()
{	
	u8 send_reg=0;
	bool ack1,ack2;

	if(send_buf_cmd){

		send_reg=master_pop_cmd();
		//EA =0;
		iic_drv_start();
		ack1=iic_drv_sendbyte(MASTER_SEND_CMD);
		ack2=iic_drv_sendbyte(send_reg);
		iic_drv_stop();	
		//EA =1;
#if 0		
	    	printf("---111-drv_rev_ack   %x \r\n",(u16)ack1);
	    	printf("----------222-drv_rev_ack   %x \r\n",(u16)ack2);		
#endif			

	}

}
#ifdef USE_PROG_PLAY_MODE
void prog_play_init()
{
	prog_total_num=1;
	prog_cur_num=0;	
	play_prog_mode=1;
	prog_disp_srn=1;
	prog_first_num =0;
	//my_memset(&prog_file_tab[0], 0x0, 20);
	
	master_push_cmd(STOP_CMD);
	master_push_cmd(MEM_CMD);
	Disp_Con(DISP_PROG_FILENUM);

}
void prog_play_clear(void)
{
	prog_total_num=1;
	prog_cur_num=0;	
	play_prog_mode=0;
	prog_disp_srn=1;	
}
void prog_hdlr(u8 key)
{
	switch(key)
	{
		// case INFO_STOP| KEY_SHORT_UP :
			//master_push_cmd(MEM_CMD);
			//master_push_cmd(STOP_CMD);
		//	break;
	        case INFO_PROG| KEY_SHORT_UP :
			if(prog_total_num>=20)break;
			if((prog_cur_num!=0)&&(prog_disp_srn==0)){
				//prog_cur_num=0;	
				if(prog_first_num==0){

					prog_first_num=rev_buf[9];
				}
				prog_disp_srn=1;
				master_push_cmd(MEM_CMD);
			}
			break;		
	        case INFO_NEXT_FIL | KEY_SHORT_UP:
				
			master_push_cmd(NEXT_FILE_CMD);
			Disp_Con(DISP_PROG_FILENUM);
			break;
	        case INFO_PREV_FIL | KEY_SHORT_UP:
				
			master_push_cmd(PREV_FILE_CMD);
			Disp_Con(DISP_PROG_FILENUM);			
			break;
	        case INFO_POWER| KEY_SHORT_UP:
			play_prog_mode=0;				
			break;
	}
}
#endif

#ifdef USE_ERP_2_HDLR
static xd_u16 erp_timer=0;
void erp_2_timer_hdlr()
{
	if((cd_play_status == MUSIC_STOP)||(cd_play_status == MUSIC_PAUSE)){

		erp_timer++;
		if(erp_timer>=10*60*2){

    			CD_PWR_GPIO_OFF();
		    	Disp_Con(DISP_POWER_OFF);
#ifdef USE_POWER_KEY				
			sys_power_down();
#endif
		}
	}
	else{
		erp_timer = 0;
	}
}
#endif
/*----------------------------------------------------------------------------*/
/**@brief  mcu_hdlr 消息处理
   @param  无
   @return 无
   @note   void mcu_hdlr( void )
*/
/*----------------------------------------------------------------------------*/
void mcu_hdlr( void )
{
    u8 key;

    Mute_Ext_PA(MUTE);
	
    while (1)
    {
		//suspend_sdmmc();
  	dac_out_select(DAC_AMUX1);
		
	key = get_msg();
#if 0	
	if(key!= 0xff)
	    	printf("------->-get_msg   %x \r\n",(u16)key);		
#endif	
	if(dac_cnt > 20)
	{
		dac_sw(0);
	}
	if(mcu_master_tranceive_tick){
		mcu_master_tranceive_tick =0;
		mcu_master_send();
		mcu_master_rev();
		mcu_master_info_hdlr();
	}

#ifdef USE_PROG_PLAY_MODE
	if(play_prog_mode){
		
		prog_hdlr(key);
		if((key==INFO_HALF_SECOND)||(key==(INFO_PROG | KEY_SHORT_UP))||(key==(INFO_NEXT_FIL | KEY_SHORT_UP))||(key==(INFO_PREV_FIL | KEY_SHORT_UP))){
			key = 0xFF;
		}		
	}
#endif	
	switch (key)
	{

	        case INFO_NEXT_SYS_MODE:
#if 0
			if(cd_play_status!= MUSIC_STOP){
				//cd_play_status=MUSIC_STOP;			
				master_push_cmd(STOP_CMD);
				put_msg_lifo(INFO_NEXT_SYS_MODE); 
				break;
			}
#endif			
			return;
	        case INFO_PLAY | KEY_SHORT_UP :

			if((door_open)&&(curr_menu == DISP_OPEN)){

				info_timer_2=0;
			        Disp_Con(DISP_VERSION);	
			}
			
			if(!toc_flag)break;		//2 TOC  NOT READY

#ifdef USE_PROG_PLAY_MODE
			if(play_prog_mode){

			       play_mode = REPEAT_OFF;
				master_push_cmd(REP_OFF_CMD);				   
				play_prog_mode=0;
				if(prog_first_num>0)
				prog_icon_bit=1;
			}
#endif			
			if(cd_play_status== MUSIC_PLAY){

			      Mute_Ext_PA(MUTE);
				cd_play_status=MUSIC_PAUSE;
				master_push_cmd(PAUSE_CMD);

		              Disp_Con(DISP_FILENUM);	
			}
			else if(cd_play_status == MUSIC_PAUSE){

			      Mute_Ext_PA(UNMUTE);
				cd_play_status=MUSIC_PLAY;

				play_disp_scr=0;
				
		              Disp_Con(DISP_FILENUM);	
				master_push_cmd(PLAY_RESUME_CMD);
			}
			else if(cd_play_status== MUSIC_STOP){

			       Mute_Ext_PA(UNMUTE);
				cd_play_status=MUSIC_PLAY;

				play_disp_scr=0;

				if(next_prev_key){
					next_prev_key=0;					
				}
				else if(prog_icon_bit||play_prog_mode){
					rev_buf[2]=0;
					given_file_number =prog_first_num;						
				}
				else{
					rev_buf[2]=0;
					given_file_number =1;	
				}
		              Disp_Con(DISP_FILENUM);					
				master_push_cmd(PLAY_RESUME_CMD);
			}			
			break;
			
	        case INFO_STOP| KEY_SHORT_UP :
			if(toc_flag)
			{
#ifdef USE_PROG_PLAY_MODE
				play_prog_mode=0;	
				prog_cur_num=0;	
#endif
				play_disp_scr=0;
				next_prev_key=0;					
			       play_mode = REPEAT_OFF;
			      Mute_Ext_PA(MUTE);
				cd_play_status=MUSIC_STOP;			
				master_push_cmd(STOP_CMD);
	                    	Disp_Con(DISP_DWORD_NUMBER);		
			}
			break;
	        case INFO_NEXT_FIL | KEY_SHORT_UP:
			next_prev_key=1;				
			master_push_cmd(NEXT_FILE_CMD);

			if(cd_play_status == MUSIC_PAUSE){
			
				master_push_cmd(PAUSE_CMD);
			}

			set_sys_vol(my_music_vol);
			
			break;
	        case INFO_PREV_FIL | KEY_SHORT_UP:
			next_prev_key=1;								
			master_push_cmd(PREV_FILE_CMD);

			if(cd_play_status == MUSIC_PAUSE){
			
				master_push_cmd(PAUSE_CMD);	
			}

			set_sys_vol(my_music_vol);
			
			break;			
	        case INFO_NEXT_FIL | KEY_HOLD:
			fast_fr_release_cnt=3;
			master_push_cmd(FAST_F_CMD);
			break;
	        case INFO_PREV_FIL | KEY_HOLD:
			fast_fr_release_cnt=3;			
			master_push_cmd(FAST_R_CMD);
			break;
#ifdef USE_PROG_PLAY_MODE
	    	case INFO_PROG| KEY_SHORT_UP:
			if((cd_play_status== MUSIC_STOP)	&&toc_flag){
				prog_play_init();
			}
			break;
#endif			
#ifdef MASTER_SEL_CD_PLAY_MODE
	    	//case INFO_MODE | KEY_SHORT_UP:
        	case INFO_PLAY_MODE :
			if(toc_flag==0)break;

#if 0
			if(prog_icon_bit||play_prog_mode){
				break;
			}
#endif
			
#ifdef USE_INTRO_MODE_FUNC			
			if(play_mode==REPEAT_INTRO){
				master_push_cmd(INTRO_OFF_CMD);
			}
			else
#endif
			 if(play_mode==REPEAT_RANDOM){
				master_push_cmd(REP_RAND_OFF_CMD);
			}
			else{
				
				master_push_cmd(REP_OFF_CMD);
			}
			 
			play_mode++;
#ifdef USE_INTRO_MODE_FUNC
			if(play_mode>REPEAT_INTRO)
				play_mode=REPEAT_ALL;
#else			
			if(play_mode>REPEAT_END)
				play_mode=REPEAT_HEAD;
			
#endif
			if(play_mode==REPEAT_ALL){

				master_push_cmd(REP_ALL_CMD);
			}
			else if(play_mode==REPEAT_ONE){
				master_push_cmd(REP_ONE_CMD);
			}
			else if(play_mode==REPEAT_RANDOM){
				master_push_cmd(REP_RANDOM_CMD);
			}
			else if(play_mode==REPEAT_OFF){
				master_push_cmd(REP_OFF_CMD);
			}			
#ifdef USE_INTRO_MODE_FUNC			
			else if(play_mode==REPEAT_INTRO){
				master_push_cmd(INTRO_ON_CMD);
			}		
#endif
#ifdef CD_UART_ENABLE
	    		printf("------->> play_mode   %x---%x \r\n",(u16)send_buf,(u16)play_mode);
#endif			
			break;			
#endif				
		 case INFO_HALF_SECOND :

			 if(fisrt_time_op&&toc_flag){

					master_push_cmd(STOP_CMD);
					fisrt_time_op=0;
			 }

	   		 if(adkey_detect){
	   	    		adkey_detect=0;
	   			set_sys_vol(my_music_vol);
	   		 }
#if ((USE_DEVICE == MEMORY_STYLE)&&(FAT_MEMORY))           
	            updata_fat_memory();
#endif

#if defined(USE_BAT_MANAGEMENT)
		     bmt_hdlr();
#endif
#if defined(USE_TIMER_POWER_OFF_FUNC)
		       timer_pwr_off_hdlr();
#endif
#ifdef USE_ERP_2_HDLR
			erp_2_timer_hdlr();
#endif
	            if (DISP_OPEN== curr_menu){
				break;
		     }
				
	            if (DISP_PLAY == curr_menu)
	            {
	            		 if(toc_flag)
	                		disp_file_time();
	            }
				
	            set_brightness_fade_out();
	            if (return_cnt < RETURN_TIME)
	            {
	                return_cnt++;
	            }

	            if (RETURN_TIME == return_cnt)
	            {
	                if (DISP_DWORD_NUMBER == curr_menu)
	                {
	                    cfilenum = 0;
	                }

			  if(cd_play_status== MUSIC_PLAY){
					
	                	if (DISP_PLAY != curr_menu)
	                    		Disp_Con(DISP_PLAY);
			  }
			  else if(cd_play_status== MUSIC_PAUSE){
					
	                	if (DISP_PLAY != curr_menu)
	                		disp_file_time();
			  }			  
			  else if(cd_play_status== MUSIC_STOP){
	                	if ((DISP_DWORD_NUMBER != curr_menu)&&(toc_flag))
	                    		Disp_Con(DISP_DWORD_NUMBER);
			  }
	            }
	            break;
		
#if RTC_ENABLE 
			case INFO_ALM_BELL :					   ///<闹钟响消息，进入闹钟循环
				dac_out_select(DAC_DECODE);
				alm_bell_mode();
#ifdef PWR_CTRL_IN_IDLE_MODE
#if defined(PWR_CTRL_WKUP)
				wkup_pin_ctrl(1);
#else
				PWR_CTRL_GPIO_INIT();
				PWR_CTRL_EN();
#endif
#endif
				break;
#endif
	        default :
	            if (!ap_handle_hotkey(key))
	                return;
	}
    }
}

/*----------------------------------------------------------------------------*/
/**@brief  mcu_main_hdlr功能入口
   @param  无
   @return 无
   @note   void mcu_main_hdlr(void)
*/
/*----------------------------------------------------------------------------*/
void mcu_main_hdlr(void)
{
    Disp_Con(DISP_SCAN_TOC);
    Mute_Ext_PA(MUTE);
    CD_PWR_GPIO_CTRL_INIT();
    CD_PWR_GPIO_ON();
    sysclock_div2(1);
    flush_low_msg();
    mcu_master_init();
    set_max_vol(MAX_ANALOG_VOL, MAX_DIGITAL_VOL);			//设置AUX模式的音量上限
    mcu_hdlr();
    prog_play_clear();	
    main_vol_set(0, CHANGE_VOL_NO_MEM);
    CD_PWR_GPIO_OFF();	
#ifdef USE_PROG_PLAY_MODE
	prog_total_num=1;
	prog_cur_num=0;	
	play_prog_mode=0;
	prog_icon_bit=0;
	
#endif	
    	play_mode=REPEAT_OFF;
}
#endif

