#include "Custom_config.h"
#include "iic.h"
#include "typedef.h"
#include "fm_rev.h"
#ifdef USE_RDAs_FM	

/***************************************************
RDA5807P interfaces
****************************************************/
void RDA5807P_PowerOnReset(void);
void RDA5807P_PowerOffProc(void);

void RDA5807P_SetMute(bool mute);
u16 RDA5807P_FreqToChan(u16 frequency);
void RDA5807P_SetFreq( u16 curf );
bool RDA5807P_ValidStop(u16 freq);
u8 RDA5807P_GetSigLvl();
void RDA5807P_SetVolumeLevel(u8 level);
bool RDA5807P_Intialization(void);

_xdata u16 gChipID = 0;
_xdata u8 RDA5807P_REG[10];

/***************************************************
RDA5807P
****************************************************/
_code u8 RDA5807P_initialization_reg[]={
#if defined(_SHARE_CRYSTAL_24MHz_)
    0xC0, 0x51, //02H:
#elif defined(_SHARE_CRYSTAL_12MHz_)
    0xC0, 0x11, //02H:
#elif defined(_SHARE_CRYSTAL_32KHz_)
    0xC0, 0x01, //02H:
#else
    0xC0, 0x01,
#endif

#if defined(_FM_STEP_50K_)
    0x00, 0x12,        
#else
    0x00, 0x10,
#endif
    0x04, 0x00,        
    0x86, 0xAF, //05H: 
    0x00, 0x00,        
    0x5E, 0xC6,        
    0x50, 0x96,        
    0x00, 0x00,        
    0x40, 0x00, //0AH: 
    0x00, 0x8F,        
    0x00, 0x00,        
    0x00, 0x00,        
    0x00, 0x00,        
    0x00, 0x00,        
    0xF0, 0x05, //10H: 
    0x90, 0x00,        
    0xF4, 0x84,        
    0x70, 0x01,        
    0x40, 0xF0,        
    0x21, 0x80, //15H: 
    0x7A, 0xD0,        
    0x3E, 0x40,        
    0x55, 0xA9,        
    0xE8, 0x48,        
    0x50, 0x80, //1AH: 
    0x00, 0x00,        
    0x37, 0xB6,        
    0x40, 0x0C,        
    0x07, 0x9B,        
    0x4C, 0x1D,        
    0x81, 0x11, //20H: 
    0x45, 0xC0,        
};
_code u8 RDA5807PE_initialization_reg[]={	//4 RDA5807SP   RDA5807PE
#if defined(_SHARE_CRYSTAL_24MHz_)
    0xc4, 0x51, //02H:
#elif defined(_SHARE_CRYSTAL_12MHz_)
    0xc4, 0x11, //02H:
#elif defined(_SHARE_CRYSTAL_32KHz_)
    0xc0, 0x01, //02H:
#else
    0xC0, 0x01,
#endif

#if defined(_FM_STEP_50K_)
    0x1b, 0x92,
    0x0C, 0x00,
#else       //Step 100K
    0x00, 0x10,
    0x04, 0x00,
#endif

    0x85, 0xAF, //05H:
    0x80, 0x00,
    0x76, 0xc6, //07H
    0x5e, 0xc6,
    0x00, 0x00,
    0x40, 0x6e, //0AH: 
    0x2d, 0x80,
    0x58, 0x03,
    0x58, 0x04,
    0x58, 0x04,
    0x58, 0x04,
    0x00, 0x47, //10H: 
    0x90, 0x00,
    0xF5, 0x87,
    0x7F, 0x0B, //13H:
    0x04, 0xF1,
    0x5E, 0xc0, //15H: 0x42, 0xc0
    0x41, 0xe0,
    0x50, 0x6f,
    0x55, 0x92,
    0x00, 0x7d,
    0x10, 0xC0,//1AH
    0x07, 0x80,
    0x41, 0x1d,//1CH,
    0x40, 0x04,
    0x1f, 0x9B,
    0x4c, 0x2b,//1FH. 
    0x81, 0x10, //20H: 
    0x45, 0xa0,// 21H

#if defined(_FM_STEP_50K_)
    0x55, 0x3F, //22H
#else
    0x19, 0x3F, //22H
#endif

    0xaf, 0x40, 
    0x06, 0x81,
    0x1b, 0x2a, //25H
    0x0D, 0x04,
    0x80, 0x9F, //0x80, 0x2F, 
    0x17, 0x8A,
    0xD3, 0x49,
    0x11, 0x42,
    0xA0, 0xC4, //2BH
    0x3E, 0xBB,
    0x00, 0x00,
    0x58, 0x04,
    0x58, 0x04, //2FH
    0x58, 0x04,
    0x00, 0x74,
    0x3D, 0x00,
    0x03, 0x0C,
    0x2F, 0x68,
    0x38, 0x77, //35H
};

_code  u8 RDA5807PH_initialization_reg[]={
#if defined(_SHARE_CRYSTAL_24MHz_)
    0xC4, 0x51, //02H:
#elif defined(_SHARE_CRYSTAL_12MHz_)
    0xC4, 0x11, //02H:
#elif defined(_SHARE_CRYSTAL_32KHz_)
    0xC0, 0x01, //02H:
#else
    0xC0, 0x01,
#endif
    0x00,0x10,
    0x04,0x00,
    0x86,0xbF,  //05H   
    0x40,0x00,
    0x46,0xC6,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,  //0AH
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x06,  //10H
    0x00,0x19, //
    0x2A,0x11,
    0x00,0x2E,
    0x2A,0x30,
    0xB8,0x3C,  //15H
    0x90,0x00,
    0x2A,0x91,
    0x84,0x12,
    0x00,0xA8,
    0xC4,0x00,  //1AH
    0xE0,0x00,
    0x30,0x1D,//0x24,0x9D,1cH
    0x81,0x6A,
    0x46,0x08,
    0x00,0x86,  //1FH
    0x06,0x61,  //20H
    0x00,0x00,
    0x10,0x9E,
    0x25,0x4A,//   0x24,0x47,//0830//23H//2346   24c9  254a
    0x04,0x08,//0830
    0x06,0x08,  //0x06,0x08,//0830  //0X25H
    0xE1,0x05,
    0x3B,0x6C,
    0x2B,0xEC,
    0x09,0x0F,
    0x34,0x14,  //2AH
    0x14,0x50,
    0x09,0x6D,
    0x2D,0x96,
    0x01,0xDA,
    0x2A,0x7B,
    0x08,0x21,   //30H
    0x13,0xD5,
    0x48,0x91,
    0x00,0xbc,
    0x08,0x96,   //34H
    0x15,0x3C,  //35H
    0x0B,0x80,   //36H
    0x25,0xC7,   //37H
    0x00,0x00,   //38H
};
//uint8 RDA5807N_initialization_reg[]={
_code  u8 RDA5807N_initialization_reg[]={	// MP  FP
#if defined(_SHARE_CRYSTAL_24MHz_)
    0xC4, 0x51, //02H:
#elif defined(_SHARE_CRYSTAL_12MHz_)
    0xC4, 0x11, //02H:
#elif defined(_SHARE_CRYSTAL_32KHz_)
    0xC0, 0x01, //02H:
#else
    0xC0, 0x01,
#endif
    0x00, 0x10,
    0x04, 0x00,
    0xc5, 0xad, //05h
    0x60, 0x00,
    0x42, 0x16,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,  //0x0ah
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,  //0x10h
    0x00, 0x19,
    0x2a, 0x11,
    0xB0, 0x42,  
    0x2A, 0x11,  
    0xb8, 0x31,  //0x15h 
    0xc0, 0x00,
    0x2a, 0x91,
    0x94, 0x00,
    0x00, 0xa8,
    0xc4, 0x00,  //0x1ah
    0xF7, 0xcF,   
    0x2A, 0xDC,  //0x1ch
    0x80, 0x6F, 
    0x46, 0x08,
    0x00, 0x86,
    0x06, 0x61, //0x20H
    0x00, 0x00,
    0x10, 0x9E,
    0x23, 0xC8,
    0x04, 0x06,
    0x0E, 0x1C, //0x25H
};

#if defined(USE_FM_GPIO)
void RDA5807P_gpio_ctrl(bool fm_ctrl_flag,u8 gpio_num)
{
	u8 reg_temp=0;
	//printf(" --->RDA5807P_gpio_ctrl ---%x \r\n",(u16)fm_ctrl_flag);
	if(fm_ctrl_flag)
  	{
  		if(gpio_num == FM_GPIO_1)
		reg_temp = 0x03;
	  	else if(gpio_num == FM_GPIO_2)
		reg_temp = 0x0C;
	}
	else{
		reg_temp = 0x00;
	}
	RDA5807P_REG[5] &=0xc0;
	RDA5807P_REG[5] |=reg_temp;
	
	OperationRDAFM_2w(WRITE, &(RDA5807P_REG[0]), 6);
       delay_10ms(10);    //Dealy 50 ms	

} 
#endif
/**
 * @brief Reset RDA5807P while power on RDA5807P
 * @author RDA RDA Ri'an Zeng
 * @date 2008-11-05
 * @param void
 * @return void
 * @retval
 */
void  RDA5807P_PowerOnReset(void)
{
    RDA5807P_Intialization(); 
}
/**
 * @brief RDA5807P power off function
 * @author RDA Ri'an Zeng
 * @date 2008-11-05
 * @param void
 * @return void
 * @retval
 */
void  RDA5807P_PowerOffProc(void)
{
	///sys_printf(" FM SET RDA5807P_PowerOffProc ");

	RDA5807P_REG[1] &= (~1);
	OperationRDAFM_2w(WRITE, &(RDA5807P_REG[0]), 2);
       delay_10ms(5);    //Dealy 50 ms	
}
/**
 * @brief Set RDA5807P into mute mode
 * @author RDA Ri'an Zeng
 * @date 2008-11-05
 * @param bool mute: if mute is true,then set mute; if mute is false,then set no mute
 * @return void
 * @retval
 */
void RDA5807P_SetMute(bool mute)
{
	u8 reg_temp[2]={0};
	//sys_printf(" FM SET RDA5807P_SetMute ");
	reg_temp[0] = RDA5807P_REG[0];
	reg_temp[1] = RDA5807P_REG[1];

	if(mute)
	 	reg_temp[0] &=  ~(BIT(6));
	else
		reg_temp[0] |=BIT(6);

	OperationRDAFM_2w(WRITE, &(reg_temp[0]), 2);
       delay_10ms(5);    //Dealy 50 ms
}
/**
 * @brief Cover the frequency to channel value
 * @author RDA Ri'an Zeng
 * @date 2008-11-05
 * @param u16 frequency:covered frequency
 * @return u16: channel value
 * @retval
 */
u16 RDA5807P_FreqToChan(u16 frequency) 
{
	u8 channelSpacing;
	u16 bottomOfBand;
	u16 channel;

	if ((RDA5807P_REG[3] & 0x0c) == 0x00) 
		bottomOfBand = 870;
	else if ((RDA5807P_REG[3] & 0x0c) == 0x04)	
		bottomOfBand = 760;
	else if ((RDA5807P_REG[3] & 0x0c) == 0x08)	
		bottomOfBand = 760;	
	if ((RDA5807P_REG[3] & 0x03) == 0x00) 
		channelSpacing = 1;
	else if ((RDA5807P_REG[3] & 0x03) == 0x01) 
		channelSpacing = 2;

	channel = (frequency - bottomOfBand) / channelSpacing;
	return (channel);
}

bool RDA5807P_Get_ST(void)
{
	u16 Reg_0AH=0;
	//sys_printf(" FM SET RDA5807P_Get_ST ");

	RDAFM_read_data(0x0A,&Reg_0AH,2);

	//printf(" FM----->RDA5807P_Get_ST ---%x---",Reg_0AH);

	if((Reg_0AH&0x0400)>0){
		//sys_printf(" 11111111FM SET RDA5807P_Get_ST ");
		return 1;
	}
	else{
		//sys_printf(" 22222222222FM SET RDA5807P_Get_ST ");
		return 0;
	}
}
/**
 * @brief Set frequency function
 * @author RDA Ri'an Zeng
 * @date 2008-11-05
 * @param u16 curFreq:frequency value
 * @return void
 * @retval
 */
void RDA5807P_SetFreq( u16 curFreq )
{   
	u16 curChan;
	//sys_printf(" FM SET RDA5807P_SetFreq ");
	curChan=RDA5807P_FreqToChan(curFreq);
	
	//SetNoMute
	RDA5807P_REG[0] |=	BIT(6);
	RDA5807P_REG[2]=curChan>>2;
	RDA5807P_REG[3]=(((curChan&0x0003)<<6)|0x10) | (RDA5807P_REG[3]&0x0f);	//set tune bit
	
	OperationRDAFM_2w(WRITE, &(RDA5807P_REG[0]), 4);
	delay_10ms(5);     //Delay five ms
}

/**
 * @brief Station judge for auto search
 * @In auto search mode,uses this function to judge the frequency if has a station
 * @author RDA Ri'an Zeng
 * @date 2008-11-05
 * @param u16 freq:frequency value
 * @return bool: if return true,the frequency has a true station;otherwise doesn't have a station
 * @retval
 */
bool RDA5807P_ValidStop(u16 freq)
{
	u8 RDA5807P_reg_data[4]={0};	
	u8 falseStation = 0;
	u8 i=0,j=0;
	u16 curChan;
	u16 rssi =0;
     //sys_printf(" FM SET RDA5807P_ValidStop ");

	curChan=RDA5807P_FreqToChan(freq);
	
	//SetNoMute
	//RDA5807P_REG[0] |=	(1<<7);
	
	RDA5807P_reg_data[0]=RDA5807P_REG[0];
	RDA5807P_reg_data[1]=RDA5807P_REG[1];
	RDA5807P_reg_data[2]=curChan>>2;
	RDA5807P_reg_data[3]=(((curChan&0x0003)<<6)|0x10) | (RDA5807P_REG[3]&0x0f);
	OperationRDAFM_2w(WRITE,&(RDA5807P_reg_data[0]), 4);
	
#if defined(VOL_TUNE_FREQ_VOL)||defined(VOL_TUNE_FREQ_ONLY)||defined(VOL_TUNE_NEW_VOLUME_KEY_FEATURE)
	if( get_radio_freq_tune_protect()){
		
	}
	else
#endif	
	{
	
#if defined(K302_ZhengYang_ZY302_V001	)||defined(FAST_BAND_SCAN_SPEED)
	delay_10ms(2);    //Dealy 25 ms
#elif defined(MID_SPEED_BAND_SCAN)
	delay_10ms(8);    //Dealy 25 ms
#else
	delay_10ms(15);    //Dealy 25 ms
#endif	
	}

	do
	{
		//read REG0A&0B	
		OperationRDAFM_2w(READ,&(RDA5807P_reg_data[0]), 4);	
		if(i++>5) break; 
		delay_10ms(1);
	}while((RDA5807P_reg_data[0] &0x40)==0);
	
	delay_10ms(1);
	
	OperationRDAFM_2w(READ,&(RDA5807P_reg_data[0]), 4);	

	rssi = ((RDA5807P_reg_data[2])>>1)&0x7F;
	//check FM_TURE

#ifndef RDA5807_DISENABLE_RSSI

   	if (gChipID == 0x5804)
	{
		if(((RDA5807P_reg_data[2] &0x01)==0)||(rssi<50))
		{
			 falseStation=1;
		}
    	}
    	else if(gChipID == 0x5801)
	{

		if(((RDA5807P_reg_data[2] &0x01)==0))
		{
			 falseStation=1;
		}
	}
	else
#endif			
	{
		if(((RDA5807P_reg_data[2] &0x01)==0))
		{
			 falseStation=1;
		}
	}

	if((freq==960)&&((RDA5807P_reg_data[2] &0x01)==0)&&((RDA5807P_reg_data[0]&0x04)==0)) return 0;
	if((freq==961)&&((RDA5807P_reg_data[2] &0x01)==0)&&((RDA5807P_reg_data[0]&0x04)==0)) return 0;

#ifndef RDA5807SP_DISENABLE_ST
   	if (gChipID == 0x5804)
	{
		if((falseStation!=1)&&(RDA5807P_reg_data[0]&0x04))
			return 1;   
		else 
			return 0;
   	}
    	else if(gChipID == 0x5801)
	{

		if ((falseStation!=1))
			return 1;   
		else 
			return 0;
    	}
	else
#endif		
	{
		if (falseStation!=1)
			return 1;   
		else 
			return 0;
	}
	return 0;		
}
/**
 * @brief Get the signal level(RSSI) of the current frequency
 * @author RDA Ri'an Zeng
 * @date 2008-11-05
 * @param u16 curf:frequency value
 * @return u8: the signal level(RSSI)
 * @retval
 */
u8 RDA5807P_GetSigLvl()
{    
    u8 RDA5807P_reg_data[4]={0};	

    OperationRDAFM_2w(READ,&(RDA5807P_reg_data[0]), 4);
	delay_10ms(50);    //Dealy 50 ms
    return  (RDA5807P_reg_data[2]>>1);  /*·µ»Ørssi*/
}
/**
 * @brief Set FM volume
 * @It has better use the system volume operation to replace this function
 * @author RDA Ri'an Zeng
 * @date 2008-11-05
 * @param u8 level: volume value
 * @return void
 * @retval
 */
void RDA5807P_SetVolumeLevel(u8 level)
{
    u8 RDA5807P_reg_data[8];
    u8 i = 0;

	for (i=0;i<8;i++)
        RDA5807P_reg_data[i] = RDA5807P_REG[i];
	
	if(level==0)
	 	RDA5807P_reg_data[0] &=  ~(BIT(6));
	else
		RDA5807P_reg_data[0] |=(BIT(6));	

	RDA5807P_reg_data[7]=(( RDA5807P_reg_data[7] & 0xf0 ) | (level & 0x0f)); 

	  RDA5807P_REG[0] =RDA5807P_reg_data[0];
	  RDA5807P_REG[7] =RDA5807P_reg_data[7];
	   
    	 RDA5807P_reg_data[3] &= (~(0x10));

	 OperationRDAFM_2w(WRITE, &(RDA5807P_reg_data[0]), 8);
        delay_10ms(5);    //Dealy 50 ms
}
/**
 * @brief Initialize RDA5807P
 * @author RDA Ri'an Zeng
 * @date 2008-11-05
 * @param void
 * @return bool:if true,the operation is successful;otherwise is failed
 * @retval
 */
bool  RDA5807P_Intialization(void)
{
    u16 error_ind = 0;
    u8 RDA5807P_REGR[10]={0x0};
    u8 i = 0;

    RDA5807P_REG[0] = 0x00;
    RDA5807P_REG[1] = 0x02;

	error_ind = OperationRDAFM_2w(WRITE, (u8 *)&RDA5807P_REG[0], 2);
	delay_10ms(6);

	error_ind = OperationRDAFM_2w(READ, (u8 *)&RDA5807P_REGR[0], 10);
	delay_10ms(1);

    gChipID = RDA5807P_REGR[8];
    gChipID = ((gChipID<<8) | RDA5807P_REGR[9]);

    if (gChipID == 0x5804)
    {
        for (i=0;i<8;i++)
            RDA5807P_REG[i] = RDA5807PE_initialization_reg[i];
     
        error_ind = OperationRDAFM_2w(WRITE, (u8 *)&RDA5807PE_initialization_reg[0], 2);
        delay_10ms(20); 
	    error_ind = OperationRDAFM_2w(WRITE, (u8 *)&RDA5807PE_initialization_reg[0], sizeof(RDA5807PE_initialization_reg));
    }
    else if (gChipID == 0x5808)
    {
        for (i=0;i<8;i++)
            RDA5807P_REG[i] = RDA5807N_initialization_reg[i];
     
        error_ind = OperationRDAFM_2w(WRITE, (u8 *)&RDA5807N_initialization_reg[0], 2);
        delay_10ms(20); 
	    error_ind = OperationRDAFM_2w(WRITE, (u8 *)&RDA5807N_initialization_reg[0], sizeof(RDA5807N_initialization_reg));
    }
    else if (gChipID == 0x5801)
    {
        for (i=0;i<8;i++)
            RDA5807P_REG[i] = RDA5807PH_initialization_reg[i];
     
        error_ind = OperationRDAFM_2w(WRITE, (u8 *)&RDA5807PH_initialization_reg[0], 2);
        delay_10ms(6); 
	    error_ind = OperationRDAFM_2w(WRITE, (u8 *)&RDA5807PH_initialization_reg[0], sizeof(RDA5807PH_initialization_reg));
    }
    else
    {
        gChipID = RDA5807P_REGR[4];
        gChipID = ((gChipID<<8) | RDA5807P_REGR[5]);
    }
	
    if ((gChipID == 0x5802) || (gChipID == 0x5803))
    {
        gChipID = 0x5802;

        for (i=0;i<8;i++)
            RDA5807P_REG[i] = RDA5807P_initialization_reg[i];


        error_ind = OperationRDAFM_2w(WRITE, (u8 *)&RDA5807P_initialization_reg[0], 2);
        delay_10ms(60); 
        error_ind = OperationRDAFM_2w(WRITE, (u8 *)&RDA5807P_initialization_reg[0], sizeof(RDA5807P_initialization_reg));

    }

	delay_10ms(2);         //Dealy 50 ms
	//RDA5807P_SetMute(1);
	
	if (error_ind )
	   return 0;
	else
	   return 1;
}
u16 RDA5807P_Get_ID()
{
	u16 error_ind = 0,ChipID = 0xFFFF;
	u8 RDA5807P_REGR[10]={0x0};

    	RDA5807P_REG[0] = 0x00;
    	RDA5807P_REG[1] = 0x02;

	error_ind = OperationRDAFM_2w(WRITE, (u8 *)&RDA5807P_REG[0], 2);
	delay_10ms(20);

	error_ind = OperationRDAFM_2w(READ, (u8 *)&RDA5807P_REGR[0], 10);
	//delay_10ms(5);

    	ChipID = RDA5807P_REGR[8];
    	ChipID = ((ChipID<<8) | RDA5807P_REGR[9]);
	printf(" ChipID %x \r\n",ChipID);
	 return ChipID;

}
#endif
