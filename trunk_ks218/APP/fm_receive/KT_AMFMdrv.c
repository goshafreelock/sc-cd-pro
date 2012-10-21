/************************************************************************************/
/*公司名称：			KT Micro													*/
/*																					*/
/*模 块 名：			AM-FM驱动模块												*/
/*																					*/
/*创 建 人：								时间：									*/
/*修 改 人：Kanghekai						时间：2011-04-08						*/
/*功能描述：																		*/
/*其他说明： Revision History
//  Version	Date		Description
//  V1.0	2010-01-31	Initial draft.
//	V2.0	2010-05-07	Initial draft for KT0913.
//  V2.1	2010-05-21	Disable the feature of fast gain up in baseband AGC in KT_AMTune function.
//  V2.2	2010-05-25	Modified the "KT_FMGetSNR" function.
//  V2.3	2010-05-27	A pre-compile option DISABLE_FAST_GAIN_UP is added.
//						A pre-compile option AMSMUTE is added.
//  V2.4    2010-06-01	Modify the AGC_FAST_WIN<1:0> to 00 (fastest) in  KT_AMFMInit() function
//						Modify the KT0913_SwitchMode () in main.c to preserve channel when switching band.
//  V2.5    2010-07-20	A pre-compile option LOWFM_MODE is added.
//  V2.6    2010-07-22	A pre-compile option MANUAL_SEPARATION is added.
//						A patch about RFAGC is added in KT_AMFMInit function.
//  V2.7    2010-07-23	A pre-compile option AM_SOFTMUTE is added.
//  V2.8    2010-07-27	A pre-compile option FM_SOFTMUTE is added.
//						KT_AM_TUNING_LIGHT function is added.
//						KT_FM_TUNING_LIGHT function is added.
//						Fixed a bug about AM SOFTMUTE in main.c when switching band.
//  V2.9    2010-08-03	A pre-compile option AM_GAIN is added.
//  V3.0    2010-08-04	Modified the "KT_AMSetBW" function.
//  V3.1    2010-08-06	Fixed the define of I2C address in KT_AMFMdrv.h.
//						Clean up.
//  V3.2    2010-08-17	Modified the KT_FM_SOFTMUTE_SETTING function.
//	V3.3	2010-08-27	Added the "KT_FM_ST_Indicator()" function.
//						A pre-compile option AUDIO_LEVEL_BOOST is added.
//	V3.4	2010-09-09	Added the pre-compile option of X32P768K, R32P768K, X38K, R38K, R7P6M, R13M, R24M and R26M.
//	V3.5	2010-10-16	Added the pre-compile option of "AM_ABC"(Automatic Bandwidth Control).
//	V3.6	2010-11-11	Added the pre-compile option of "FM_HIGHLEVER_RF_SIGAL_INPUT".
//						Modified the "KT_AM_SOFTMUTE" and "KT_FM_SOFTMUTE" function.
//	V3.7	2010-11-17	Modified the "KT_FMTune", "KT_AMTune", "KT_FMGetFreq" and "KT_AMGetFreq" function.
//	V3.8	2010-12-09	Modified the "KT_FMTune", "KT_AMFMSetMode" and "KT_FMGetFreq" function..
//						Added the pre-compile option of "AM_SOFTMUTE_AFCMODE".
//	V3.9	2011-03-03	Clean up.
//						Added the SW mode for KT0915. Added the pre-compile option of "KT0915".
//	V4.0	2011-04-08	Clean up.
//						Deleted the "KT_AMSeekFromCurrentCh()" function.
//						Deleted the "KT_FMSeekFromCurrentCh()" function.
//						Modified the "KT_FMTune()" function.
//						Modified the "KT_FMGetAFC()" function.
//						Added the "KT_FMSeekTune()" function.
//						Added the "KT_AMFMSeekFromCurrentCh()" function.
/*版    本：V4.0																	*/
/************************************************************************************/
#include "Custom_config.h"

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "KT_AMFMdrv.h"
//#include <interface.h>
//#include <math.h>
#include "uart.h"
#include "gpio_if.h"
#include "fm_rev.h"

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------
char mem_afc[3];			  //Rememberred afc values for previous, current and next stations
xd_u16 mem_freq[3];			  //Rememberred channel frequencies for previous, current and next stations
//xd_u8 mem_vol;				  //Rememberred volume before mute
#ifdef SEEK_WITH_SNR
xd_u8 mem_snr[3];			  //Rememberred SNR values for previous, current and next stations
#endif

#ifdef RADIO_ST_INDICATOR
extern bool radio_st_ind;
#endif

Str_Band  Current_Band;


xd_u8 KT_FMGetST(void);

#ifdef UART_ENABLE
#define DEBUG_SW
#endif
#define I2C
#ifdef I2C

void KT_Bus_Write(xd_u8 Register_Address, xd_u16 Word_Data)
{
#if 1
	xd_u8 write_reg_data[2]={0};
	write_reg_data[1]=(u8)(Word_Data&0x00FF);
	write_reg_data[0]=(u8)(Word_Data>>8);
    	EA =0;
	iic_write((KTAMw_address),Register_Address,&write_reg_data,2);
    	EA =1;
#else
	I2C_Start();
	I2C_Senddata(device_address & 0xFE);
	I2C_Ack();
	if (Ack_Flag == 0)
	{
		I2C_Senddata(reg_add);
		I2C_Ack();
		if (Ack_Flag == 0)
		{
			I2C_Senddata(writedata);
			I2C_Ack();
		}
		else
			SCL = 0;			
	}
	else
		SCL = 0;
	I2C_Stop();
#endif	
	
}

//------------------------------------------------------------------------------------
//I2C Read Byte
//------------------------------------------------------------------------------------
xd_u16 KT_Bus_Read(xd_u8 Register_Address)
{
	xd_u16 readdata;
#if 1
    EA =0;
    iic_start();                    //I2C启动
    iic_sendbyte((KTAMw_address));             //写命令
    iic_sendbyte(Register_Address);         //写地址
    iic_start();                    //写转为读命令，需要再次启动I2C
    iic_sendbyte((KTAMw_address|0x01));             //读命令
    readdata = iic_revbyte(0);
    readdata=readdata<<8;
    readdata |= iic_revbyte(1);
    iic_stop();                     //I2C停止
    EA =1;
#else
	iic_start();
	IiC_Senddata(device_address & 0xFE);
	I2C_Ack();
	if (Ack_Flag == 0)	//7.12 edit
	{
		I2C_Senddata(reg_add);
		I2C_Ack();
		if (Ack_Flag == 0)	//7.12 edit
		{
			I2C_Start();
			I2C_Senddata(device_address | 0x01);
			I2C_Ack();
			if (Ack_Flag == 0)
			{
//	SDA pin is high Z
				readdata = I2C_Receivedata();
				I2C_Ack();
			}
			else
			{
				SCL=0;
				return(0x00);
			}
		}
		else
		{
			SCL = 0;
		 	return(0x00);
		}			
	}
	else
	{
		SCL = 0;
		return(0x00);
	}

	I2C_Stop();	
#endif	
	return(readdata);
}
#endif

/************************************************************************************/
/*函 数 名：KT_AMFMPreInit												 	 */
/*功能描述：芯片工作时钟初始化程序													*/
/*函数说明：																 */
/*调用函数：KT_Bus_Read()、KT_Bus_Write()									 */
/*全局变量：INIT_FAIL_TH													 */
/*输    入：无																 */
/*返    回：正确：1					错误：0									 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
#if 0
xd_u8 KT_AMFMPreInit(void)			  
//This function should be called just after power-up (within 50ms) when crystal is used
//or before reference clock is applied
{
	xd_u16 regx;
	xd_u8 i;

	deg_str("KT_AMFMPreInit  \n");

	for (i = 0;i < INIT_FAIL_TH;i++)
	{
		delay_10ms(10);
		regx = KT_Bus_Read(0x01);           			//Read Manufactory ID 

		printf_u16(regx,'D');

	  	if (regx != 0x4B54) continue;
		break;
	}

		deg_str("KT_AMFMPreInit end   \n");

    if (i == INIT_FAIL_TH) return(0);

#ifdef X32P768K
	KT_Bus_Write(0x16, 0x0002);       				//reference clock=32.768K; XTALD=0
#endif

#ifdef R32P768K
	KT_Bus_Write(0x16, 0x1002);       				//reference clock=32.768K; XTALD=1
#endif

#ifdef X38K
	KT_Bus_Write(0x16, 0x0902);       				//reference clock=38K; XTALD=0
#endif

#ifdef R38K
	KT_Bus_Write(0x16, 0x1902);       				//reference clock=38K; XTALD=1
#endif

#ifdef R7P6M
	KT_Bus_Write(0x16, 0x1202);       				//reference clock=7.6M; XTALD=1
#endif

#ifdef R12M
	KT_Bus_Write(0x16, 0x1302);       				//reference clock=12M; XTALD=1
#endif

#ifdef R13M
	KT_Bus_Write(0x16, 0x1402);       				//reference clock=13M; XTALD=1
#endif

#ifdef R24M
	KT_Bus_Write(0x16, 0x1702);       				//reference clock=24M; XTALD=1
#endif

#ifdef R26M
	KT_Bus_Write(0x16, 0x1802);       				//reference clock=26M; XTALD=1
#endif

#ifdef R12288KHz
	regx = KT_Bus_Read(0x16);
	KT_Bus_Write(0x16, regx | 0x1000);				//XTALD=1
	regx = KT_Bus_Read(0x16);
	KT_Bus_Write(0x16, regx | 0x2000);				//CTCLK=1
	regx = KT_Bus_Read(0x1E);
	KT_Bus_Write(0x1E, regx & 0x0000 | 0x0140);		//DIVIDERP<9:0>=320
	regx = KT_Bus_Read(0x1F);
	KT_Bus_Write(0x1F, regx & 0x0000 | 0x023A);		//DIVIDERN<9:0>=570
#endif
	return(1);
}
#endif
/************************************************************************************/
/*函 数 名：KT_AMFMInit													 	 */
/*功能描述：芯片初始化程序													 */
/*函数说明：																 */
/*调用函数：KT_Bus_Read()、KT_Bus_Write()									 */
/*全局变量：INIT_FAIL_TH													 */
/*输    入：无																 */
/*返    回：正确：1					错误：0									 */
/*设 计 者：Yangpei					时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
xd_u8 KT_pre_init(void)
{	
	xd_u16 regx;

#if 1
	regx = KT_Bus_Read(0x01);           			//Read Manufactory ID 
#ifdef DEBUG_SW    	
	printf("  ------->>FM ID  %x   \r\n ",regx);
#endif	
	if (regx != 0x4B54) return 0;
	
	//regx=KT_Bus_Read(0x12);						//Read power-up indicator
	//printf_u16(regx,'S');

	//if ((regx&0x8000)!=0x8000) return 0;
	//deg_str("KT_AMFMInit  \n");

#ifdef X32P768K
	KT_Bus_Write(0x16, 0x0002);       				//reference clock=32.768K; XTALD=0
#endif

#ifdef R32P768K
	KT_Bus_Write(0x16, 0x1002);       				//reference clock=32.768K; XTALD=1
#endif

	delay_10ms(120);
#endif
	return 1;
}
xd_u8 KT_AMFMInit(void)                            //0->Fail 1->Success
{
	xd_u16 regx;
#if 0
	//mem_vol= 30;
	//deg_str("KT_AMFMInit  \n");
	regx = KT_Bus_Read(0x01);           			//Read Manufactory ID 
	//printf_u16(regx,'D');
	if (regx != 0x4B54) return 0;
	
	//regx=KT_Bus_Read(0x12);						//Read power-up indicator
	//printf_u16(regx,'S');

	//if ((regx&0x8000)!=0x8000) return 0;
	//deg_str("KT_AMFMInit  \n");

#ifdef X32P768K
	KT_Bus_Write(0x16, 0x0002);       				//reference clock=32.768K; XTALD=0
#endif

#ifdef R32P768K
	KT_Bus_Write(0x16, 0x1002);       				//reference clock=32.768K; XTALD=1
#endif

	delay_10ms(120);
#endif


	KT_Bus_Write(0x10,0x6000);						// re-cali
	regx = KT_Bus_Read(0x0A);
	KT_Bus_Write(0x0A, regx & 0xFEFF);      		//Turn On FM AFC function
	regx = KT_Bus_Read(0x22);
	KT_Bus_Write(0x22, regx & 0xFCFF | 0x0200);		//AM AGC short win=slowest

	regx = KT_Bus_Read(0x0A);
	KT_Bus_Write(0x0a, regx | 0x0400);      		//LO_LDO= highest

	regx = KT_Bus_Read(0x22);
	KT_Bus_Write(0x22, regx & 0xF3FF);      		//AGC_FAST_WIN<1:0>=00(fastest)

	regx = KT_Bus_Read(0x23);
	KT_Bus_Write(0x23,regx | 0x1C00);				//Low_TH_B<2:0>=111

#ifdef LOWFM_MODE
	regx = KT_Bus_Read(0x0C);
	KT_Bus_Write(0x0C, regx | 0x0008);      		//LOWFM_MODE=1(32MHz-110MHz)
#endif

	regx = KT_Bus_Read(0x3F);
	KT_Bus_Write(0x3F, regx & 0xFF88 | 0x0013);		// RFAGC Patch

#ifdef RADIO_ST_INDICATOR	
	regx = KT_Bus_Read(0x05);
	KT_Bus_Write(0x05, (regx | 0x0200));			//
#endif

#ifdef AM_GAIN
	regx = KT_Bus_Read(0x22);
	KT_Bus_Write(0x22, (regx & 0xFFCF) | 0x0020 );	// AM_GAIN = 12dB
#endif

#ifdef AUDIO_LEVEL_BOOST
	regx = KT_Bus_Read(0x16);
	KT_Bus_Write(0x16, (regx & 0xFF3F) | 0x0040 );	// AU_GAIN = 6dB
#endif

#ifdef FM_HIGHLEVER_RF_SIGAL_INPUT
	regx = KT_Bus_Read(0x09);
	KT_Bus_Write(0x09, (regx & 0xFCFF));			//
#endif
	regx = KT_Bus_Read(0x0E); 
	KT_Bus_Write(0x0E, regx | 0x7700);			// strangthen AM cop
	
	regx = KT_Bus_Read(0x0F); 
	KT_Bus_Write(0x0F, regx & 0xFFE0);		//Write volume to 0

//	regx = KT_Bus_Read(0x0A);
//	KT_Bus_Write(0x0A, (regx | 0x6000));			//

//	regx = KT_Bus_Read(0x16);
//	KT_Bus_Write(0x16, (regx | 0x0001));			//

	return(1);
}
/************************************************************************************/
/*函 数 名：KT_AMSetBW													 	 */
/*功能描述：AM Channel Bandwidth设置程序									 */
/*函数说明：																 */
/*调用函数：KT_Bus_Read()、KT_Bus_Write()									 */
/*全局变量：AMBW															 */
/*输    入：xd_u8 AMBW														 */
/*返    回：设置完毕：1														 */
/*设 计 者：Yangpei					时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
xd_u8 KT_AMSetBW(xd_u8 AMBW)				//AM Channel Bandwidth=2 for 2KHz; 4 for 4KHz; 6 for 6KHz
{
	xd_u16 regx;
	regx = KT_Bus_Read(0x22);
	if (AMBW == 2) KT_Bus_Write(0x22, (regx & 0xFF3F) | 0x0000);
	else if (AMBW == 4) KT_Bus_Write(0x22, (regx & 0xFF3F) | 0x0080);
	else if (AMBW == 6) KT_Bus_Write(0x22, (regx & 0xFF3F) | 0x00C0);
	else return(0);
	return(1);
}

/************************************************************************************/
/*函 数 名：KT_AMFMSetMode												 	 */
/*功能描述：工作模式设置程序												 */
/*函数说明：																 */
/*调用函数：KT_Bus_Read()、KT_Bus_Write()									 */
/*全局变量：AMFM_MODE、FM_MODE、MW_MODE										 */
/*输    入：无																 */
/*返    回：设置完毕：1														 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
void KT_AMFMSetMode(xd_u8 AMFM_MODE)
{
	xd_u16 regx;

	KT_AMFMInit();

	if (AMFM_MODE == FM_MODE)
	{
#ifdef SW_GAIN_HW_CTRL	
		SW_GAIN_CTRL_GPIO_INIT();
		SW_GAIN_CTRL_DIS();
#endif		
		regx = KT_Bus_Read(0x16);
		KT_Bus_Write(0x16,regx&0x7FFF);				//AM_FM=0
	}
	else if (AMFM_MODE == MW_MODE)
	{
		Current_Band.Band =MW_MODE;
		
#ifdef SW_GAIN_HW_CTRL	
		SW_GAIN_CTRL_GPIO_INIT();
		SW_GAIN_CTRL_DIS();
#endif

		//KT_AMSetBW(4);
		regx = KT_Bus_Read(0x16);
		KT_Bus_Write(0x16,regx|0x8000);				//AM_FM=1

		KT_Bus_Write(0x18,0x0000);						//Enable cap

		KT_AMTune(900);
		regx = KT_Bus_Read(0x0F);       
		KT_Bus_Write(0x0F, regx & 0xFFE0);		//Write volume to 0
		delay_10ms(12);

		regx = KT_Bus_Read(0x0E); 
		KT_Bus_Write(0x0E, regx | 0x7700);	
		
   		regx = KT_Bus_Read(0x0a);
		KT_Bus_Write(0x0a, (regx&(~0x6000)));  		
	}
	else
	{
#ifdef SW_GAIN_HW_CTRL	
		SW_GAIN_CTRL_GPIO_INIT();
		SW_GAIN_CTRL_EN();
#endif
		Current_Band.Band =SW_MODE;

		regx = KT_Bus_Read(0x16);
		KT_Bus_Write(0x16,regx|0x8000);				//AM_FM=1

		KT_Bus_Write(0x18,0x0000);						//Enable cap
		KT_AMTune(900);
		regx = KT_Bus_Read(0x0F);       
		KT_Bus_Write(0x0F, regx & 0xFFE0);		//Write volume to 0
		delay_10ms(12);
		
		KT_Bus_Write(0x18,0x8000);					//Disable cap
		
   		regx = KT_Bus_Read(0x0a);
		KT_Bus_Write(0x0a, (regx|0x6000));  		
	}
	//return(1);
}

/************************************************************************************/
/*函 数 名：KT_AMFMStandby												 	 */
/*功能描述：Standby程序												 */
/*函数说明：																 */
/*调用函数：KT_Bus_Read()、KT_Bus_Write()									 */
/*全局变量：无																 */
/*输    入：xd_u8 AMBW														 */
/*返    回：设置完毕：1														 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
void  KT_AMFMStandby(void)					//0->Fail 1->Success
{
	xd_u16 regx;
	regx = KT_Bus_Read(0x0F);
	KT_Bus_Write(0x0F, regx | 0x1000);		//Write Standby bit to 1
	delay_10ms(20);
}

/************************************************************************************/
/*函 数 名：KT_AMFMWakeUp												 	 */
/*功能描述：唤醒程序													 */
/*函数说明：																 */
/*调用函数：KT_Bus_Read()、KT_Bus_Write()、KT_AMFMInit()					 */
/*全局变量：无																 */
/*输    入：xd_u8 AMBW														 */
/*返    回：KT_AMFMInit()													 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
xd_u8 KT_AMFMWakeUp(void) //0->Fail 1->Success
{
	xd_u16 regx;
	regx = KT_Bus_Read(0x0F);
	KT_Bus_Write(0x0F, regx & 0xEFFF);			//Write Standby bit to 0

	delay_10ms(20);	
#if 1
	if(KT_pre_init()==1){

		KT_AMFMInit();
		return 1;
	}
	return 0;
#else
	return (KT_AMFMInit());
#endif	
}
#if 0
/************************************************************************************/
/*函 数 名：KT_AMFMVolumeSet											 	 */
/*功能描述：音量设置程序													*/
/*函数说明：vol=0~31														 */
/*调用函数：KT_Bus_Read()、KT_Bus_Write()									 */
/*全局变量：mem_vol															 */
/*输    入：xd_u8 vol														 */
/*返    回：设置完毕：1														 */
/*设 计 者：Yangpei					时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
xd_u8 KT_AMFMVolumeSet(xd_u8 vol)			//Input: 0~31
{
	xd_u16 regx;
	mem_vol = vol;
	regx = KT_Bus_Read(0x0F); 
	KT_Bus_Write(0x0F, regx & 0xFFE0 | mem_vol);
	return(1);
}
#endif
/************************************************************************************/
/*函 数 名：KT_AMFMMute													 	 */
/*功能描述：静音程序													 */
/*函数说明：																 */
/*调用函数：KT_Bus_Read()、KT_Bus_Write()									 */
/*全局变量：无																 */
/*输    入：无																 */
/*返    回：设置完毕：1														 */
/*设 计 者：Yangpei					时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
#if 0
xd_u8 KT_AMFMMute(void)
{
	xd_u16 regx;
	regx = KT_Bus_Read(0x0F);       
	KT_Bus_Write(0x0F, regx & 0xFFE0 );		//Write volume to 0
	return(1);
}
#endif
#if 0

/************************************************************************************/
/*函 数 名：KT_AMFMUnMute												 	 */
/*功能描述：AMFM默认音频增益程序											 */
/*函数说明：																 */
/*调用函数：KT_Bus_Read()、KT_Bus_Write()									 */
/*全局变量：mem_vol															 */
/*输    入：无																 */
/*返    回：设置完毕：1														 */
/*设 计 者：Yangpei					时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
xd_u8 KT_AMFMUnMute(void)
{
	xd_u16 regx;
	regx=KT_Bus_Read(0x0f); 
	KT_Bus_Write(0x0f, regx & 0xFFE0 | mem_vol); 
	return(1);
}
#endif
#ifdef C32P768K

/*****************************************************************************/
/*函 数 名：KT_FMTune													 	 */
/*功能描述：FM Tune台程序													 */
/*函数说明：在Tune台的过程中静音											 */
/*调用函数：KT_AMFMMute()、KT_Bus_Read()、KT_Bus_Write()、delay_10ms()		 */
/*全局变量：无																 */
/*输    入：xd_u16 Frequency													 */
/*返    回：设置完毕：1														 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
xd_u8 KT_FMTune(xd_u16 Frequency) //87.5MHz-->Frequency=8750; Mute the chip and Tune to Frequency
{
	xd_u16 regx;
	
#ifdef RADIO_ST_INDICATOR
	radio_st_ind=0;
#endif

	//KT_AMFMMute();
	regx = KT_Bus_Read(0x0F);       
	KT_Bus_Write(0x0F, regx & 0xFFE0 );		//Write volume to 0
	
	KT_Bus_Write(0x1E, 0x0001);								//DIVIDERP<9:0>=1
	KT_Bus_Write(0x1F, 0x029C);								//DIVIDERN<9:0>=668
	regx = KT_Bus_Read(0x16);
	KT_Bus_Write(0x16, 	regx & 0xD0FF);       				//reference clock=32.768K;

	regx=KT_Bus_Read(0x0A);
	if(
		(Frequency == 8920) || (Frequency == 8780)	|| (Frequency == 9030)	|| (Frequency == 9440)	||
		(Frequency == 9850) || (Frequency == 10260) || (Frequency == 10670) || (Frequency == 9170)	|| (Frequency == 9580)	||
		(Frequency == 9660)	|| (Frequency == 9860)	|| (Frequency == 10230)	|| (Frequency == 10240)	|| (Frequency == 10520) ||
	       (Frequency == 9470) || (Frequency == 9480)	|| (Frequency == 9490)	|| (Frequency == 10570)	|| (Frequency == 10560)	||
		(Frequency == 10580) || (Frequency == 10590)	|| (Frequency == 8790)	|| (Frequency == 8810)	|| (Frequency == 9220)	||
		(Frequency == 10390)|| (Frequency == 10400) || (Frequency == 10540)|| (Frequency == 9710) || (Frequency == 10120)||
		(Frequency == 9640)|| (Frequency == 10480) || (Frequency == 8750)|| (Frequency == 8760)|| (Frequency == 8770)|| 
		(Frequency == 6700)|| (Frequency == 6710)|| (Frequency == 6720)|| (Frequency == 7110)|| (Frequency == 7120)||
		(Frequency == 7130)|| (Frequency == 6730)|| (Frequency == 6740)
	 )
	{
		KT_Bus_Write(0x0A, regx | 0x0040);
	}
	else
	{
		KT_Bus_Write(0x0A, regx & 0xFFBF);
	}
	regx=KT_Bus_Read(0x03);
	KT_Bus_Write(0x03, (regx & 0xF000) | 0x8000 | (Frequency / 5));	   		//set tune bit to 1

	delay_10ms(2);

	regx = KT_Bus_Read(0x0F);       
	KT_Bus_Write(0x0f, ((regx & 0xFFE0)|0x1E));		//Write volume to 0
	
	return(1);
}

#if 0
/*****************************************************************************/
/*函 数 名：KT_FMSeekTune												 	 */
/*功能描述：KT_FMValidStation函数调用的tune台程序									 */
/*函数说明：在Tune台的过程中静音											 */
/*调用函数：KT_AMFMMute()、KT_Bus_Read()、KT_Bus_Write()、delay_10ms()		 */
/*全局变量：无																 */
/*输    入：xd_u16 Frequency													 */
/*返    回：设置完毕：1														 */
/*设 计 者：Kanghekai				时间：2011-04-08								*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
xd_u8 KT_FMSeekTune(xd_u16 Frequency) //87.5MHz-->Frequency=8750; Mute the chip and Tune to Frequency
{
	xd_u16 reg3,regA,regx;

	KT_AMFMMute();

	regA = KT_Bus_Read(0x0A);
	KT_Bus_Write(0x0A, regA & 0xFFBF);

	KT_Bus_Write(0x1E, 0x0001);									//DIVIDERP<9:0>=1
	reg3 = KT_Bus_Read(0x03);
	regx = KT_Bus_Read(0x16);
	if((Frequency == 9660) || (Frequency == 9440) || (Frequency == 10260) || (Frequency == 10230) || (Frequency == 10240) || (Frequency == 10520))
	{
		KT_Bus_Write(0x1F, 0x029B);								//DIVIDERN<9:0>=669
		KT_Bus_Write(0x16, (regx & 0xD0FF) | 0x2000);			//CTCLK=1;reference clock=32.768K;
		KT_Bus_Write(0x03, 0x8000 | (Frequency + 15) / 5 );	   		//set tune bit to 1
	}
	else if((Frequency == 8620) || (Frequency == 8780))
	{
		KT_Bus_Write(0x1F, 0x029E);								//DIVIDERN<9:0>=670
		KT_Bus_Write(0x16, (regx & 0xD0FF) | 0x2000);			//CTCLK=1;reference clock=32.768K;
		KT_Bus_Write(0x03, 0x8000 | (Frequency - 25) / 5 );	   		//set tune bit to 1
	}
	else if((Frequency == 9030))
	{
		KT_Bus_Write(0x1F, 0x029F);								//DIVIDERN<9:0>=671
		KT_Bus_Write(0x16, (regx & 0xD0FF) | 0x2000);			//CTCLK=1;reference clock=32.768K;
		KT_Bus_Write(0x03, 0x8000 | (Frequency - 40) / 5 );	   		//set tune bit to 1
	}
	else if((Frequency == 9850) || (Frequency == 10670) || (Frequency == 9860))
	{
		KT_Bus_Write(0x1F, 0x029D);								//DIVIDERN<9:0>=669
		KT_Bus_Write(0x16, (regx & 0xD0FF) | 0x2000);			//CTCLK=1;reference clock=32.768K;
		KT_Bus_Write(0x03, 0x8000 | (Frequency - 15) / 5 );	   		//set tune bit to 1
	}
	else if((Frequency == 4380) || (Frequency == 4390))
	{
		KT_Bus_Write(0x1F, 0x029F);								//DIVIDERN<9:0>=671
		KT_Bus_Write(0x16, (regx & 0xD0FF) | 0x2000);			//CTCLK=1;reference clock=32.768K;
		KT_Bus_Write(0x03, 0x8000 | (Frequency - 20) / 5 );	   		//set tune bit to 1
	}
	else if((Frequency == 5260) || (Frequency == 5850))																		//58.5->672
	{
		KT_Bus_Write(0x1F, 0x029E);								//DIVIDERN<9:0>=670
		KT_Bus_Write(0x16, (regx & 0xD0FF) | 0x2000);			//CTCLK=1;reference clock=32.768K;
		KT_Bus_Write(0x03, 0x8000 | (Frequency - 15) / 5 );	   		//set tune bit to 1
	}
	else if((Frequency == 6570) || (Frequency == 6580) || (Frequency == 6590) || (Frequency == 7310) || (Frequency == 7890))	//73.1->669,78.9->671
	{
		KT_Bus_Write(0x1F, 0x029A);								//DIVIDERN<9:0>=666
		KT_Bus_Write(0x16, (regx & 0xD0FF) | 0x2000);			//CTCLK=1;reference clock=32.768K;
		KT_Bus_Write(0x03, 0x8000 | (Frequency + 20) / 5 );	   		//set tune bit to 1
	}
	else
	{
		KT_Bus_Write(0x1F, 0x029C);								//DIVIDERN<9:0>=668
		KT_Bus_Write(0x16, 	regx & 0xD0FF);       				//reference clock=32.768K;
		KT_Bus_Write(0x03, 0x8000 | (Frequency / 5) );	   		//set tune bit to 1
	}

	//delay_10ms(50);
	return(1);
}

#endif
/*****************************************************************************/
/*函 数 名：KT_AMTune													 	 */
/*功能描述：AM Tune台程序													 */
/*函数说明：在Tune台的过程中静音											 */
/*调用函数：KT_AMFMMute()、KT_Bus_Read()、KT_Bus_Write()、delay_10ms()		 */
/*全局变量：无																 */
/*输    入：xd_u16 Frequency													 */
/*返    回：设置完毕：1														 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/

xd_u8 KT_AMTune(xd_u16 Frequency) //1710KHz --> Frequency=1710; Mute the chip and Tune to Frequency
{
	xd_u16 regx;
	//KT_AMFMMute();
	regx = KT_Bus_Read(0x0F);       
	KT_Bus_Write(0x0F, regx & 0xFFE0 );		//Write volume to 0

	if(Current_Band.Band == MW_MODE){
		
		KT_Bus_Write(0x18,0x0000);						//Enable cap
	}
	
#ifdef DISABLE_FAST_GAIN_UP
	regx = KT_Bus_Read(0x23);
	KT_Bus_Write(0x23, regx & 0xDFFF);							//Enable fast gain function
#endif

/*	if((Frequency == 1368) || (Frequency == 1404))
	{
		KT_Bus_Write(0x1F, 0x029D);								//DIVIDERN<9:0>=669
		KT_Bus_Write(0x17, 0x8000 | (Frequency - 2));	   			//set tune bit to 1
	}
*/

	regx = KT_Bus_Read(0x0A);
	KT_Bus_Write(0x0A, regx & 0xFFBF);
			
 	if(Frequency ==1368) 
	{
		KT_Bus_Write(0x1E, 0x0001);        //DIVIDERP<9:0>=1
		KT_Bus_Write(0x1F, 0x029F);        //DIVIDERN<9:0>=670
		regx = KT_Bus_Read(0x16);
		KT_Bus_Write(0x16, (regx & 0xD0FF) | 0x2000);   //CTCLK=1;reference clock=32.768K;
		KT_Bus_Write(0x17, 0x8000 | (Frequency - 6));       //set tune bit to 1
	}
	else if(Frequency ==1370) 
	{
		KT_Bus_Write(0x1E, 0x0001);        //DIVIDERP<9:0>=1
		KT_Bus_Write(0x1F, 0x029F);        //DIVIDERN<9:0>=670
		regx = KT_Bus_Read(0x16);
		KT_Bus_Write(0x16, (regx & 0xD0FF) | 0x2000);   //CTCLK=1;reference clock=32.768K;
		KT_Bus_Write(0x17, 0x8000 | (Frequency - 6));       //set tune bit to 1
	}
	else if(Frequency ==1404) 
	{
		KT_Bus_Write(0x1E, 0x0001);        //DIVIDERP<9:0>=1
		KT_Bus_Write(0x1F, 0x0292);        //DIVIDERN<9:0>=670
		regx = KT_Bus_Read(0x16);
		KT_Bus_Write(0x16, (regx & 0xD0FF) | 0x2000);   //CTCLK=1;reference clock=32.768K;
		KT_Bus_Write(0x17, 0x8000 | (Frequency+21));       //set tune bit to 1
	}
	else if(Frequency ==1629)
	{
		KT_Bus_Write(0x1E, 0x0001);								//DIVIDERP<9:0>=1
		KT_Bus_Write(0x1F, 0x029E);								//DIVIDERN<9:0>=670
		regx = KT_Bus_Read(0x16);
		KT_Bus_Write(0x16, (regx & 0xD0FF) | 0x2000);			//CTCLK=1;reference clock=32.768K;
		KT_Bus_Write(0x17, 0x8000 | (Frequency-5));	   			//set tune bit to 1
	}
	else
	{
		KT_Bus_Write(0x1E, 0x0001);								//DIVIDERP<9:0>=1
		KT_Bus_Write(0x1F, 0x029C);								//DIVIDERN<9:0>=668
		regx = KT_Bus_Read(0x16);
		KT_Bus_Write(0x16, 	regx & 0xD0FF);       				//reference clock=32.768K;
		KT_Bus_Write(0x17, 0x8000 | Frequency);	   				//set tune bit to 1
#ifdef	KT0915
		if(Current_Band.Band >= SW_MODE){
			KT_Bus_Write(0x17, 0x8000 | Frequency);	   				//set tune bit to 1
		}
#endif
	}
	delay_10ms(2);

#ifdef DISABLE_FAST_GAIN_UP
	regx = KT_Bus_Read(0x23);
	KT_Bus_Write(0x23, regx & 0xDFFF | 0x2000);				//disable the function of fast up in baseband AGC, by chend, 2010-05-21
#endif

#ifdef RADIO_ST_INDICATOR
	radio_st_ind=0;
#endif

	regx = KT_Bus_Read(0x0F);       
	KT_Bus_Write(0x0f, ((regx & 0xFFE0)|0x1E));		//Write volume to 0
#if 0
#ifdef KT0915
		if(Current_Band.Band >= SW_MODE)
#endif
	{
		KT_Bus_Write(0x0f, ((regx & 0xFFE0)|0x1D));		//Write volume to 0
	}
	else{
		KT_Bus_Write(0x0f, ((regx & 0xFFE0)|0x1E));		//Write volume to 0
	}
#endif
	return(1);
}
#if 0
/*****************************************************************************/
/*函 数 名：KT_FMGetFreq												 	 */
/*功能描述：FM读当前电台频率程序													*/
/*函数说明：																 */
/*调用函数：KT_Bus_Read()、KT_Bus_Write()									 */
/*全局变量：无																 */
/*输    入：无																 */
/*返    回：xd_u16 Frequency													 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
xd_u16 KT_FMGetFreq(void)
{
	xd_u16 reg1F,regx;

	reg1F = KT_Bus_Read(0x1F);
	if(reg1F == 0x029B)
	{
		regx = KT_Bus_Read(0x13);
		return( regx * 5 - 15 );
	}
	else if(reg1F == 0x029F)
	{
		regx = KT_Bus_Read(0x13);
		if((regx * 5) == 8990)
		{
			return( regx * 5 + 40 );
		}
		else
		{
			return( regx * 5 + 20 );
		}
	}
	else if(reg1F == 0x029D)
	{
		regx = KT_Bus_Read(0x13);
		return( regx * 5 + 15 );
	}
	else if(reg1F == 0x029A)
	{
		regx = KT_Bus_Read(0x13);
		return(regx * 5 - 20);
	}
	else if(reg1F == 0x029E)
	{
		regx = KT_Bus_Read(0x13);
		if( ( (regx * 5) == 8755) || ( (regx * 5) == 8595) )
		{
			return( regx * 5 + 25 );
		}
		else
		{
			return( regx * 5 + 15 );
		}
	}
	else
	{
		regx= KT_Bus_Read(0x13);
		return( regx * 5 );
	}
}
/*****************************************************************************/
/*函 数 名：KT_AMGetFreq												 	 */
/*功能描述：AM读当前电台频率程														*/
/*函数说明：																 */
/*调用函数：KT_Bus_Read()、KT_Bus_Write()									 */
/*全局变量：无																 */
/*输    入：无																 */
/*返    回：xd_u16 Frequency													 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
xd_u16 KT_AMGetFreq(void)
{
	xd_u16 reg1F,regx;
	reg1F = KT_Bus_Read(0x1F);
	if(reg1F == 0x029E)
	{
		regx = KT_Bus_Read(0x13);
		if(regx == 1624)
		{
			return(regx + 5);
		}
		else
		{
			return(regx + 4);
		}
	}
	else
	{
		regx = KT_Bus_Read(0x13);
		return(regx);
	}
}
#endif

#else

/*****************************************************************************/
/*函 数 名：KT_FMTune													 	 */
/*功能描述：FM Tune台程序													 */
/*函数说明：在Tune台的过程中静音											 */
/*调用函数：KT_AMFMMute()、KT_Bus_Read()、KT_Bus_Write()、delay_10ms()		 */
/*全局变量：无																 */
/*输    入：xd_u16 Frequency													 */
/*返    回：设置完毕：1														 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
xd_u8 KT_FMTune(xd_u16 Frequency) //87.5MHz-->Frequency=8750; Mute the chip and Tune to Frequency
{
	xd_u16 reg3;

	KT_AMFMMute();

	reg3 = KT_Bus_Read(0x03);
	KT_Bus_Write(0x03, 0x8000 | (Frequency/5) );	   		//set tune bit to 1

	//delay_10ms(50);
	return(1);
}

/*****************************************************************************/
/*函 数 名：KT_FMSeekTune												 	 */
/*功能描述：KT_FMValidStation函数调用的tune台程序									 */
/*函数说明：																 */
/*调用函数：KT_FMTune()														 */
/*全局变量：无																 */
/*输    入：xd_u16 Frequency													 */
/*返    回：设置完毕：1														 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
xd_u8 KT_FMSeekTune(xd_u16 Frequency) //87.5MHz-->Frequency=8750; Mute the chip and Tune to Frequency
{
	return( KT_FMTune(Frequency) );
}

/*****************************************************************************/
/*函 数 名：KT_AMTune													 	 */
/*功能描述：AM Tune台程序													 */
/*函数说明：在Tune台的过程中静音											 */
/*调用函数：KT_AMFMMute()、KT_Bus_Read()、KT_Bus_Write()					 */
/*全局变量：无																 */
/*输    入：xd_u16 Frequency													 */
/*返    回：设置完毕：1														 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
xd_u8 KT_AMTune(xd_u16 Frequency) //1710KHz --> Frequency=1710; Mute the chip and Tune to Frequency
{
	xd_u16 reg23;
	KT_AMFMMute();

#ifdef DISABLE_FAST_GAIN_UP
	reg23 = KT_Bus_Read(0x23);
	KT_Bus_Write(0x23, reg23 & 0xDFFF);							//Enable fast gain function
#endif

	KT_Bus_Write(0x17, 0x8000 | Frequency);	   					//set tune bit to 1
#ifdef	KT0915
	if(Current_Band.Band >= SW_MODE)
		KT_Bus_Write(0x17, 0x8000 | Frequency);	   				//set tune bit to 1
#endif
	//delay_10ms(100);

#ifdef DISABLE_FAST_GAIN_UP
	reg23 = KT_Bus_Read(0x23);
	KT_Bus_Write(0x23, reg23 & 0xDFFF | 0x2000);				//disable the function of fast up in baseband AGC, by chend, 2010-05-21
#endif

	return(1);
}

/*****************************************************************************/
/*函 数 名：KT_FMGetFreq												 	 */
/*功能描述：FM读当前电台频率程														*/
/*函数说明：																 */
/*调用函数：KT_Bus_Read()													 */
/*全局变量：无																 */
/*输    入：无																 */
/*返    回：xd_u16 Frequency													 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
xd_u16 KT_FMGetFreq(void)
{
	xd_u16 regx;
	regx = KT_Bus_Read(0x13);
	return(regx * 5);
}

/*****************************************************************************/
/*函 数 名：KT_AMGetFreq												 	 */
/*功能描述：AM读当前电台频率程														*/
/*函数说明：																 */
/*调用函数：KT_Bus_Read()													 */
/*全局变量：无																 */
/*输    入：无																 */
/*返    回：xd_u16 Frequency													 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
xd_u16 KT_AMGetFreq(void)
{
	xd_u16 regx;
	regx = KT_Bus_Read(0x13);
#ifdef DEBUG_SW    	
	printf(" ----->KT_AMGetFreq  %u  \r\n ",regx);
#endif
	
	return( regx );
}
#endif

#ifdef SEMI_AUTO_SCAN_FUNC
extern xd_u8 cur_sw_fm_band;
extern u8 get_band_info_config();
void load_band_info(void)
{
#ifdef DEBUG_SW    	
	printf(" ----->load_band_info  %d  \r\n ",(u16)cur_sw_fm_band);
#endif

    if(cur_sw_fm_band==0){

		Current_Band.Band=FM_MODE;
		Current_Band.MAX_CH=FM_MAX_CH-1;
		Current_Band.Tune_Step=FM_100KHz_STEP;
		Current_Band.Seek_Step = FM_50KHz_STEP;	
		Current_Band.ValidStation_Step =FM_50KHz_STEP ;			
    }
    else if(cur_sw_fm_band==1){
		
		Current_Band.Band=MW_MODE;
		Current_Band.MAX_CH=AM_MAX_CH-1;

#ifdef GPIO_SEL_BAND_INFO_CONFIG
		if(get_band_info_config()==0){

			Current_Band.Tune_Step=AM_10KHz_STEP;
			Current_Band.Seek_Step = AM_10KHz_STEP;		
		}
		else
#endif			
		{
			Current_Band.Tune_Step=AM_9KHz_STEP;
			Current_Band.Seek_Step = AM_9KHz_STEP;		
		}

		Current_Band.ValidStation_Step =AM_2KHz_STEP;			
		Current_Band.AFCTH_Prev =MW_AFCTH_PREV;
		Current_Band.AFCTH_Next =MW_AFCTH_NEXT;
		Current_Band.AFCTH =MW_AFCTH;

    }
    else if(cur_sw_fm_band==2){
		
		Current_Band.Band=SW_MODE; 
		Current_Band.Seek_Step=	SM_5KHz_STEP;
		Current_Band.ValidStation_Step =SM_2KHz_STEP;					
		Current_Band.AFCTH_Prev =SW_AFCTH_PREV-2;
		Current_Band.AFCTH_Next =SW_AFCTH_NEXT-2;
		Current_Band.AFCTH =SW_AFCTH+2;
		Current_Band.RSSI_TH=SW_RSSI_TH;

    }	
    else if(cur_sw_fm_band==3){
		
		Current_Band.Band=SW_MODE; 
		Current_Band.Seek_Step=	SM_5KHz_STEP;
		Current_Band.ValidStation_Step =SM_3KHz_STEP;					
		Current_Band.AFCTH_Prev =SW_AFCTH_PREV-2;
		Current_Band.AFCTH_Next =SW_AFCTH_NEXT-2;
		Current_Band.AFCTH =SW_AFCTH+2;
		Current_Band.RSSI_TH=SW_RSSI_TH;
    }		
    else if(cur_sw_fm_band==4){
		
		Current_Band.Band=SW_MODE; 
		Current_Band.Seek_Step=	SM_5KHz_STEP;
		Current_Band.ValidStation_Step =SM_3KHz_STEP;					
		Current_Band.AFCTH_Prev =SW_AFCTH_PREV-2;
		Current_Band.AFCTH_Next =SW_AFCTH_NEXT-2;
		Current_Band.AFCTH =SW_AFCTH+4;
		Current_Band.RSSI_TH=SW_RSSI_TH;
		
    }
    else if(cur_sw_fm_band==5){
		
		Current_Band.Band=SW_MODE; 
		Current_Band.Seek_Step=	SM_5KHz_STEP;
		Current_Band.ValidStation_Step =SM_3KHz_STEP;					
		Current_Band.AFCTH_Prev =SW_AFCTH_PREV-3;
		Current_Band.AFCTH_Next =SW_AFCTH_NEXT-3;
		Current_Band.AFCTH =SW_AFCTH+8;
		Current_Band.RSSI_TH=SW_RSSI_TH;
		
    }	
    else if(cur_sw_fm_band==6){
		
		Current_Band.Band=SW_MODE; 
		Current_Band.Seek_Step=	SM_5KHz_STEP;
		Current_Band.ValidStation_Step =SM_3KHz_STEP;					
		Current_Band.AFCTH_Prev =SW_AFCTH_PREV-3;
		Current_Band.AFCTH_Next =SW_AFCTH_NEXT-3;
		Current_Band.AFCTH =SW_AFCTH+10;
		Current_Band.RSSI_TH=SW_RSSI_TH;
		
    }		
    else if(cur_sw_fm_band==7){
		
		Current_Band.Band=SW_MODE; 
		Current_Band.Seek_Step=	SM_5KHz_STEP;
		Current_Band.ValidStation_Step =SM_3KHz_STEP;					
		Current_Band.AFCTH_Prev =SW_AFCTH_PREV-3;
		Current_Band.AFCTH_Next =SW_AFCTH_NEXT-3;
		Current_Band.AFCTH =SW_AFCTH+10;
		Current_Band.RSSI_TH=SW_RSSI_TH;
		
    }	
    else if(cur_sw_fm_band==8){
		
		Current_Band.Band=SW_MODE; 
		Current_Band.Seek_Step=	SM_5KHz_STEP;
		Current_Band.ValidStation_Step =SM_3KHz_STEP;					
		Current_Band.AFCTH_Prev =SW_AFCTH_PREV-3;
		Current_Band.AFCTH_Next =SW_AFCTH_NEXT-3;
		Current_Band.AFCTH =SW_AFCTH+12;
		Current_Band.RSSI_TH=SW_RSSI_TH-3;
		
    }		
    else if(cur_sw_fm_band==9){
		
		Current_Band.Band=SW_MODE; 
		Current_Band.Seek_Step=	SM_5KHz_STEP;
		Current_Band.ValidStation_Step =SM_3KHz_STEP;					
		Current_Band.AFCTH_Prev =SW_AFCTH_PREV-3;
		Current_Band.AFCTH_Next =SW_AFCTH_NEXT-3;
		Current_Band.AFCTH =SW_AFCTH+14;
		Current_Band.RSSI_TH=SW_RSSI_TH-4;
		
    }		
    else{
		
		Current_Band.Band=SW_MODE; 
		Current_Band.Seek_Step=	SM_5KHz_STEP;
		Current_Band.ValidStation_Step =SM_3KHz_STEP;					
		Current_Band.AFCTH_Prev =SW_AFCTH_PREV-3;
		Current_Band.AFCTH_Next =SW_AFCTH_NEXT-3;
		Current_Band.AFCTH =SW_AFCTH+16;
		Current_Band.RSSI_TH=SW_RSSI_TH-4;
		
    }	
}

#if 1
/*****************************************************************************/
/*函 数 名：KT_FMGetST													 	 */
/*功能描述：FM 判断单声道OR立体声程序										 */
/*函数说明：																 */
/*调用函数：KT_Bus_Read()													 */
/*全局变量：TST_TH															 */
/*输    入：无																 */
/*返    回：立体声：1				单声道：0								 */
/*设 计 者：Yangpei					时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
xd_u8 KT_FMGetST(void)
{
	xd_u16 regx;
    regx = KT_Bus_Read(0x06);

#ifdef DEBUG_SW    	
	printf("  ------->>KT_FMGetST   %x \r\n",(regx & 0x7F00));
#endif	
	
	return ((regx & 0x7F00) < TST_TH);
}

/*****************************************************************************/
/*函 数 名：KT_FMReadRSSI												 	 */
/*功能描述：读FM RSSI程序													 */
/*函数说明：RSSI=-100(dbm) to -6(dbm)										 */
/*调用函数：KT_Bus_Read()													 */
/*全局变量：无																 */
/*输    入：char *RSSI														 */
/*返    回：设置完毕：1														 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
xd_u8 KT_FMReadRSSI(char *RSSI) //range from -100 to -6, unit is dbm
{
	xd_u16 regx;
	regx = KT_Bus_Read(0x12);
	*RSSI = -(100 - (((regx >> 3) & 0x001F) * 3));
#ifdef DEBUG_SW    	
	printf("  ------->>KT_FMReadRSSI   %x \r\n",*RSSI);
#endif	

	return(1);
}

/*****************************************************************************/
/*函 数 名：KT_AMReadRSSI												 	 */
/*功能描述：读AM RSSI程序													 */
/*函数说明：RSSI=-90(dbm) to -6(dbm)										 */
/*调用函数：KT_Bus_Read()													 */
/*全局变量：无																 */
/*输    入：无																 */
/*返    回：设置完毕：1														 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
xd_u8 KT_AMReadRSSI(char *RSSI) //range from -90 to -6, unit is dbm
{
	xd_u16 regx;
	regx = KT_Bus_Read(0x24);
	*RSSI = -(90 - (((regx >> 8) & 0x001F) * 3));
	
#ifdef DEBUG_SW    	
	printf(" ------------------>KT_AMReadRSSI [ **** ]  %d dB \r\n ",(u16)*RSSI);
#endif
	
	return(1);
}

/*****************************************************************************/
/*函 数 名：KT_FMGetSNR													 	 */
/*功能描述：FM 读SNR程序													 */
/*函数说明：																 */
/*调用函数：KT_Bus_Read()													 */
/*全局变量：无																 */
/*输    入：无																 */
/*返    回：xd_u16 SNR														 */
/*设 计 者：Yanpei				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
xd_u8 KT_FMGetSNR(void)
{
	xd_u16 regx;
	regx = KT_Bus_Read( 0x14 );
//	regx = (regx & 0x1FC0) >> 6;
//	regx = regx <<1;
	return( (regx & 0x1FC0) >> 5);								// 1)SNR>>6 2)SNR<<1
}
/*****************************************************************************/
/*函 数 名：KT_FMGetAFC														 */
/*功能描述：FM 读取AFC（自动频率控制）程序									 */
/*函数说明：Frequency=-127(KHz) to 127(KHz)									 */
/*调用函数：KT_Bus_Read()						 							 */
/*全局变量：无																 */
/*输    入：xd_u16 Frequency													 */
/*返    回：char afc_delta													 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
char KT_FMGetAFC(xd_u16 Frequency) // returned value's range is from -127 to 127, unit is KHz
{
	char afc_delta;
	xd_u16 regx;
	regx = KT_Bus_Read(0x3C);								// read AFC_DELTAF
	afc_delta = regx & 0x00FF;


	if((Frequency == 9440) || (Frequency == 10670))
	{
		if(afc_delta > 0)
			return (afc_delta - 8);	
		else if(afc_delta < 0)
			return (afc_delta + 8);	
	}
	else
		return (afc_delta);	
}

/*****************************************************************************/
/*函 数 名：KT_AMGetAFC														 */
/*功能描述：AM 读取AFC（自动频率控制）程序									 */
/*函数说明：Frequency=-127(KHz) to 127(KHz)									 */
/*调用函数：KT_Bus_Read()						 							 */
/*全局变量：无																 */
/*输    入：无																 */
/*返    回：char afc_delta													 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
char KT_AMGetAFC(void) // returned value's range is from -127 to 127, unit is KHz
{
	char afc_delta;
	xd_u16 regx;
	
	regx = KT_Bus_Read(0x25);								// read AFC_DELTAF
	afc_delta =(char) (regx & 0x00FF);
	return (afc_delta);	
}

xd_u16 KT_AMGetFreq(void)
{
	char rssi_reg,afc_reg;
	xd_u16 regx;
	regx = KT_Bus_Read(0x13);
#ifdef DEBUG_SW    	
	printf(" ------------------>KT_AMGetFreq  %u  \r\n ",regx);
#endif
	KT_AMReadRSSI(&rssi_reg);	
	afc_reg =KT_AMGetAFC();	
#ifdef DEBUG_SW    	
	printf(" ------------------>KT_AMGetAFC[ ***** ]  %d  \r\n ",(u16)afc_reg);
#endif

	return( regx );
}
#endif

#if 0

/*****************************************************************************/
/*函 数 名：KT_AMFMSeekFromCurrentCh									 	 */
/*功能描述：AMFM从指定频率搜台程序											 */
/*函数说明：																 */
/*调用函数：																 */
/*全局变量：FM_MODE、SEEKUP、Current_Band.Seek_Down_Limit、							*/
/*			Current_Band.Seek_Up_Limit、Current_Band.Seek_Step、					*/
/*			mem_afc、mem_freq														*/
/*输    入：xd_u16 seekDir, xd_u16 *Frequency									 */
/*返    回：正确：1					错误：0									 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
xd_u8 KT_AMFMSeekFromCurrentCh(xd_u16 seekDir, xd_u16 *Frequency)   //     seekDir: 0-->seek down 1-->seek up
{
	xd_u16 NextFreq;
	xd_u8 (*pValidStation)(xd_u16);

	mem_afc[0]=0;mem_afc[1]=0;mem_afc[2]=0;
	mem_freq[0]=0;mem_freq[1]=0;mem_freq[2]=0;
			
	if(Current_Band.Band == FM_MODE)
		*pValidStation = KT_FMValidStation;
	else
		*pValidStation = KT_AMValidStation;

	if (seekDir == SEEKUP)
	{
		if ((*Frequency) == Current_Band.Seek_Up_Limit)		//if current channel reaches 108MHz, then start seeking from 87.5MHz
			NextFreq = Current_Band.Seek_Down_Limit;
		else									//else start from 100KHz after current channel
			NextFreq = (*Frequency) + Current_Band.Seek_Step;
		while (1)
		{

			if ((*pValidStation)(NextFreq))	//if nextfreq is a true channel, then stop seeking and return seek success
			{
				*Frequency = NextFreq;
				return(1);
			}
			else
			{
				if (NextFreq == Current_Band.Seek_Up_Limit)	//if current channel reach 108MHz, then stop seeking and return seek fail
				{
					*Frequency = NextFreq;
					return(0);
				}
				NextFreq = NextFreq + Current_Band.Seek_Step;	//Try next channel
			}
		}
	}
	else
	{
		if ((*Frequency) == Current_Band.Seek_Down_Limit)		//if current channel reaches 87.5MHz, then start seeking from 108MHz
			NextFreq = Current_Band.Seek_Up_Limit;
		else									//else start from 100KHz before current channel
			NextFreq = (*Frequency) - Current_Band.Seek_Step;
		while (1)
		{
			if ((*pValidStation)(NextFreq))	//if nextfreq is a true channel, then stop seeking and return seek success
			{
				*Frequency = NextFreq;
				return(1);
			}
			else
			{
				if (NextFreq == Current_Band.Seek_Down_Limit)	//if current channel reach 87.5MHz, then stop seeking and return seek fail
				{
					*Frequency = NextFreq;
					return(0);
				}
				NextFreq = NextFreq - Current_Band.Seek_Step;	//Try next channel
			}
		}
	}
}
#endif
/*****************************************************************************/
/*函 数 名：KT_FMValidStation												 */
/*功能描述：FM 有效态判断程序											 */
/*函数说明：																 */
/*调用函数：Display_Channel()、KT_FMSeekTune()、KT_FMGetSNR()、KT_FMGetAFC() */
/*全局变量：Current_Band.ValidStation_Step									 */
/*输    入：xd_u16 Frequency													 */
/*返    回：正确：1					错误：0									 */
/*设 计 者：Yanpei					时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
xd_u8 KT_FMValidStation(xd_u16 Frequency) //0-->False Station 1-->Good Station //check AFC_DELTA only
{
	xd_u16 nextfreq;
	char afc[3];							//AFC value for previous, current and next channels
	xd_u16 freq[3];							//frequency values for previous, current and next channels
#ifdef SEEK_WITH_SNR
	xd_u8 snr[3];							//SNR for previous, current and next channels
#endif
	char i,j;
	xd_u8 snr2,snr3;

	afc[0]=0;afc[1]=0;afc[2]=0;				//initialize
	freq[0]=0;freq[1]=0;freq[2]=0;			//initialize
#ifdef SEEK_WITH_SNR
	snr[0]=0;snr[1]=0;snr[2]=0;				//initialize
#endif
	nextfreq = Frequency-Current_Band.ValidStation_Step;		
	//Get AFC values of previous, current and next channels
	for (i=0;i<3;i++)
	{
		//get AFC values for the last station's information
		for (j=0;j<3;j++)
			if (mem_freq[j]==nextfreq)
			{
				afc[i]=mem_afc[j];
				freq[i]=mem_freq[j];
#ifdef SEEK_WITH_SNR
				snr[i]=mem_snr[j];
#endif
			}
		//get new AFC value if the station information is not investigated in the last run
		if (!freq[i])
		{
		    KT_FMTune(nextfreq);
		    delay_10ms(2);			
			afc[i]=KT_FMGetAFC(nextfreq);
#ifdef SEEK_WITH_SNR
			snr[i]=KT_FMGetSNR();
#endif
			freq[i]=nextfreq;
		}
		nextfreq = nextfreq + Current_Band.ValidStation_Step;
	}
	//Record AFC values
	mem_afc[0]=afc[0];mem_afc[1]=afc[1];mem_afc[2]=afc[2];
	mem_freq[0]=freq[0];mem_freq[1]=freq[1];mem_freq[2]=freq[2];

#ifdef SEEK_WITH_SNR
	mem_snr[0]=snr[0];mem_snr[1]=snr[1];mem_snr[2]=snr[2];
#endif

	//Determine the validation of current station
	if ((afc[0]<afc[1]) && (afc[1]<afc[2]) && (afc[0]<-FM_AFCTH_PREV) && (afc[1]>-FM_AFCTH) && (afc[1]<FM_AFCTH) && (afc[2]>FM_AFCTH_NEXT)) {

#ifdef SEEK_WITH_SNR
		KT_FMTune(Frequency);
		delay_10ms(2);
		snr2=KT_FMGetSNR();
		if ((snr[1]>=FM_SNR_TH) && (snr2>=FM_SNR_TH)) return(1);
		if ((snr[1]<FM_SNR_TH) && (snr2<FM_SNR_TH)) return(0);
		delay_10ms(1);
		snr3=KT_FMGetSNR();
		if (snr3>=FM_SNR_TH) return(1);
		else return(0);
#else
		return(1);
#endif

	}
	else
		return(0);
}
#if 0
/*****************************************************************************/
/*函 数 名：KT_AMSeekFromCurrentCh										 	 */
/*功能描述：AM从指定频率搜台程序											 */
/*函数说明：																 */
/*调用函数：KT_AMValidStation()												 */
/*全局变量：KT_AMValidStation、Current_Band.Seek_Down_Limit、KT_FMValidStation			 
/*			Current_Band.Seek_Up_Limit、Current_Band.Seek_Step、SEEKUP		 */
/*输    入：xd_u16 seekDir, xd_u16 *Frequency									 */
/*返    回：xd_u16 Frequency													 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
/*
xd_u8 KT_AMSeekFromCurrentCh(xd_u16 seekDir, xd_u16 *Frequency)   //	seekDir: 0-->seek down 1-->seek up
{
	xd_u16 NextFreq;
	mem_afc[0]=0;mem_afc[1]=0;mem_afc[2]=0;
	mem_freq[0]=0;mem_freq[1]=0;mem_freq[2]=0;

	if (seekDir == SEEKUP)
	{
		if ((*Frequency) == Current_Band.Seek_Up_Limit)			//if current channel reaches 1710KHz, then start seeking from 504KHz
			NextFreq = Current_Band.Seek_Down_Limit;
		else											//else start from 9KHz after current channel
			NextFreq = (*Frequency) + Current_Band.Seek_Step;
		while (1)
		{
			if (KT_AMValidStation(NextFreq))			//if nextfreq is a true channel, then stop seeking and return seek success
			{
				*Frequency = NextFreq;
				return(1);
			}
			else
			{
				if (NextFreq ==  Current_Band.Seek_Up_Limit)		//if current channel reach 108MHz, then stop seeking and return seek fail
				{	
					*Frequency = NextFreq;
					return(0);
				}
				NextFreq = NextFreq + Current_Band.Seek_Step;	//Try next channel
			}
		}
	}
	else
	{
		if ((*Frequency) == Current_Band.Seek_Down_Limit)			//if current channel reaches 504KHz, then start seeking from 1710KHz
			NextFreq = Current_Band.Seek_Up_Limit;
		else											//else start from 9kHz before current channel
			NextFreq = (*Frequency) - Current_Band.Seek_Step;
		while (1)
		{
			if (KT_AMValidStation(NextFreq))			//if nextfreq is a true channel, then stop seeking and return seek success
			{
				*Frequency = NextFreq;
				return(1);
			}
			else
			{
				if (NextFreq == Current_Band.Seek_Down_Limit)		//if current channel reach 504KHz, then stop seeking and return seek fail
				{
 					*Frequency = NextFreq;
					return(0);
				}
				NextFreq = NextFreq - Current_Band.Seek_Step;	//Try next channel
			}
		}
	}
}
*/
#endif
/*****************************************************************************/
/*函 数 名：KT_AMValidStation												 */
/*功能描述：AM 有效态判断程序											 */
/*函数说明：																 */
/*调用函数：Display_Channel_AM()、KT_AMGetAFC() 							 */
/*全局变量：Current_Band.ValidStation_Step、Current_Band.AFCTH_Prev			 */
/*输    入：xd_u16 Frequency													 */
/*返    回：正确：1					错误：0									 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
xd_u8 KT_AMValidStation(xd_u16 Frequency) //0-->False Station 1-->Good Station //check AFC_DELTA only
{
	char AM_afc[3];							//AFC value for previous, current and next channels
	AM_afc[0] = 0;AM_afc[1] = 0;AM_afc[2] = 0;	//initialize

    //Display_Channel_AM(Frequency);			//display current channel frequency

	KT_AMTune( Frequency - Current_Band.ValidStation_Step );
	delay_10ms(20);
	
	AM_afc[0] = KT_AMGetAFC();

	if( AM_afc[0] < -Current_Band.AFCTH_Prev )
	{

		KT_AMTune( Frequency );
		delay_10ms(22);				
		AM_afc[1] = KT_AMGetAFC();

		if( (AM_afc[1] >= -Current_Band.AFCTH) && (AM_afc[1] <= Current_Band.AFCTH) )
		{
			KT_AMTune( Frequency + Current_Band.ValidStation_Step );
			delay_10ms(22);
			AM_afc[2] = KT_AMGetAFC();

			if( AM_afc[2] > Current_Band.AFCTH_Next )
			{
				if ( (AM_afc[0] < AM_afc[1]) && (AM_afc[1] < AM_afc[2]) ){
					return(1);
				}
				else
					return(0);
			}
			else
			{
				return(0);
			}
		}
		else
		{
			return(0);
		}
	}
	else
	{
		return(0); 
	}
}

xd_u8 KT_SMValidStation(xd_u16 Frequency) //0-->False Station 1-->Good Station //check AFC_DELTA only
{
	char AM_afc[3],rssi_reg[3];
	static char last_rssi_reg=0;							//AFC value for previous, current and next channels
	AM_afc[0] = 0;AM_afc[1] = 0;AM_afc[2] = 0;	//initialize
	rssi_reg[0] = 0;rssi_reg[1] = 0;rssi_reg[2] = 0;	//initialize

    //Display_Channel_AM(Frequency);			//display current channel frequency
#ifdef DEBUG_SW    
	printf(" KT %d    AM  SW  %u  \r\n ",(u16)cur_sw_fm_band,Frequency);
#endif	
	
	KT_AMTune( Frequency - Current_Band.ValidStation_Step );
	delay_10ms(16);
	AM_afc[0] = KT_AMGetAFC();
#ifdef DEBUG_SW    	
	printf(" ----------------------------->KT  AM_afc  [ 0000 ]  %d  \r\n ",(u16)AM_afc[0]);
#endif
	KT_AMReadRSSI(&rssi_reg[0]);	

	if( (AM_afc[0] <= -Current_Band.AFCTH_Prev)/*||((last_rssi_reg-rssi_reg[0])<=-Current_Band.RSSI_TH )*/)
	{

		KT_AMTune(Frequency);
		delay_10ms(22);		
		AM_afc[1] = KT_AMGetAFC();
#ifdef DEBUG_SW    
		printf(" ----------------------------------------->KT  AM_afc  [ 1111 ]  %d  \r\n ",(u16)AM_afc[1]);
#endif
		KT_AMReadRSSI(&rssi_reg[1]);	

		if( (AM_afc[1] >= -Current_Band.AFCTH) && (AM_afc[1] <= Current_Band.AFCTH) )
		{
			KT_AMTune( Frequency + Current_Band.ValidStation_Step );
			delay_10ms(22);

			
			AM_afc[2] = KT_AMGetAFC();
#ifdef DEBUG_SW    
			printf(" ---------------------------------------------------> KT  AM_afc  [ 2222 ]  %d  \r\n ",(u16)AM_afc[2]);
#endif
			KT_AMReadRSSI(&rssi_reg[2]);	

			if(( AM_afc[2] >= Current_Band.AFCTH_Next)||((rssi_reg[0]-rssi_reg[1])<=-Current_Band.RSSI_TH ))
			{
#ifdef DEBUG_SW    
			printf(" ---------------------------------------------------> KT  (rssi_reg[0]-rssi_reg[1])  [ 3333 ]  %d   (?)  -%d \r\n ",(u16)(rssi_reg[0]-rssi_reg[1]),(u16)Current_Band.RSSI_TH);
#endif
			
				//if ( (AM_afc[0] < AM_afc[1]) && (AM_afc[1] < AM_afc[2]) ){
					return(1);
				//}
				//else
				//	return(0);
			}
			else
			{
				return(0);
			}
		}
		else
		{
			return(0);
		}
	}
	else
	{
		last_rssi_reg= rssi_reg[0];
		return(0); 
	}
}
#if 0
/*****************************************************************************/
/*函 数 名：KT_AM_TUNING_LIGHT												 */
/*功能描述：AM tuning灯指示程序（包含AM带宽控制程序）								*/
/*函数说明：																 */
/*调用函数：KT_Bus_Read()、KT_AMGetAFC()、KT_AMSetBW()						 */
/*全局变量：												 */
/*输    入：无																 */
/*返    回：正确：1					错误：0									 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
bit KT_AM_TUNING_LIGHT(void)
{
	xd_u16 reg1C;
	char AM_AFC;
	reg1C = KT_Bus_Read(0x1C);
	reg1C = reg1C & 0x3F00;
	reg1C = reg1C>>8;
	AM_AFC = KT_AMGetAFC();

#ifdef AM_ABC
	if( ( reg1C <= AM_TUNING_LIGHT_TH ) && ( AM_AFC > -2 ) && ( AM_AFC < 2 ) )
		KT_AMSetBW(4);
	else
		KT_AMSetBW(2);
#endif

	if( ( reg1C <= AM_TUNING_LIGHT_TH ) && ( AM_AFC > -MW_AFCTH ) && ( AM_AFC < MW_AFCTH ) )
		return(1);
	else
		return(0);
}

/*****************************************************************************/
/*函 数 名：KT_FM_TUNING_LIGHT												 */
/*功能描述：FM tuning灯指示程序														*/
/*函数说明：																 */
/*调用函数：KT_FMGetAFC()													 */
/*全局变量：FM_SNR_TH														 */
/*输    入：无																 */
/*返    回：正确：1					错误：0									 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
bit KT_FM_TUNING_LIGHT(void)
{
	char FM_AFC;
	FM_AFC = KT_FMGetAFC(0);
	if( ( KT_FMGetSNR() >= FM_SNR_TH ) && ( FM_AFC > -FM_AFCTH ) && ( FM_AFC < FM_AFCTH ) ) 
		return(1);
	else
		return(0);
}

/*****************************************************************************/
/*函 数 名：KT_FM_ST_Indicator												 */
/*功能描述：FM立体声指示程序												 */
/*函数说明：																 */
/*调用函数：KT_FMReadRSSI()、KT_Bus_Read()、KT_FMGetST()、CODE_Send()		 */
/*全局变量：												 */
/*输    入：无																 */
/*返    回：立体声：1				单声道：0								 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
bit KT_FM_ST_Indicator(void)
{
#ifdef MANUAL_SEPARATION
	xd_u16 reg5;
	char xdata rssi_value;

	KT_FMReadRSSI(&rssi_value);

	if(rssi_value >= SEPARATION_POINT)
	{
		reg5 = KT_Bus_Read(0x05);					// BLEND DISABLE
		KT_Bus_Write(0x05, reg5 | 0x0020);
		if ( KT_FMGetST() )
	 	{
			return(1);
		}
		else 
		{
			CODE_Send(0x98);					// LCD显示"mono"符号
			return(0);
		}
	}
	if(rssi_value <= BLEND_POINT)
	{
		reg5 = KT_Bus_Read(0x05);					// BLEND ENABLE
		KT_Bus_Write(0x05, reg5 & 0xFFDF);
		return(0);
	}
#else
	if ( KT_FMGetST() )
	{
		return(1);
	}
	else 
	{
		return(0);
	}
#endif
}
#endif

void KT_Mute_Ctrl(bool m_f)
{
	u16 regx=0;

	regx = KT_Bus_Read(0x0F);       
	if(m_f){
		KT_Bus_Write(0x0F, regx & 0xFFE0 );		//Write volume to 0
	}
	else{
		KT_Bus_Write(0x0f, regx & 0xFFE0 | 0x1D); 
	}
}
#ifdef RADIO_ST_INDICATOR
void KT_Radio_ST_Check()
{
	static u8 st_check_timer=0;
	u16 regx=0;
	char rssi_value;
	
	if((st_check_timer++>3)&&(cur_sw_fm_band==FM_MODE)){

		st_check_timer=0;
		
#ifdef MANUAL_SEPARATION

		KT_FMReadRSSI(&rssi_value);

		if(rssi_value >= SEPARATION_POINT)
		{
			regx = KT_Bus_Read(0x05);					// BLEND DISABLE
			KT_Bus_Write(0x05, regx | 0x0020);
			if ( KT_FMGetST() )
		 	{
				radio_st_ind=1;
			}
			else 
			{
				radio_st_ind=0;
			}
		}
		
		if(rssi_value <= BLEND_POINT)
		{
			regx = KT_Bus_Read(0x05);					// BLEND ENABLE
			KT_Bus_Write(0x05, regx & 0xFFDF);
			radio_st_ind=0;
		}
#else	
		if(KT_FMGetST()>0){
			radio_st_ind=1;
		}
		else{
			radio_st_ind=0;
		}
#endif		
	}
}
#endif

/*****************************************************************************/
/*函 数 名：KT_AM_SOFTMUTE												 	 */
/*功能描述：AM 自动静音程序												  	 */
/*函数说明：																 */
/*调用函数：KT_AMValidStation()、KT_Bus_Read()、KT_Bus_Write()、			 */
/*			KT_AM_SOFTMUTE_SETTING()		 								 */
/*全局变量：无																 */
/*输    入：xd_u16 Frequency													 */
/*返    回：无																 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
#ifdef AM_SOFTMUTE
void KT_AM_SOFTMUTE(xd_u16 Frequency)
{
	xd_u16 reg4;

	reg4 = KT_Bus_Read(0x04);

	if(KT_AMValidStation(Frequency))
	{
//		KT_AM_SOFTMUTE_SETTING(2,3,2,5);					// SMUTEA=4,SMUTER=60ms,AM_SMTH=3,VOLUMET=5,SMMD=RSSI mode
		KT_Bus_Write(0x04,reg4 | 0x4000);					// AM Softmute Disable
	}
	else
	{
		KT_AM_SOFTMUTE_SETTING(0,3,7,5);					// SMUTEA=16,SMUTER=60ms,AM_SMTH=8,VOLUMET=5,SMMD=RSSI mode
		KT_Bus_Write(0x04,reg4 & 0xBFFF);					// AM Softmute Enable
	}
//	reg4=KT_Bus_Read(0x04);									// AM Softmute Enable
//	KT_Bus_Write(0x04,reg4 & 0xBFFF);
}

/*****************************************************************************/
/*函 数 名：KT_AM_SOFTMUTE_SETTING										 	 */
/*功能描述：AM 自动静音设置程序											  	 */
/*函数说明：																 */
/*调用函数：KT_Bus_Read()、KT_Bus_Write()									 */
/*全局变量：无																 */
/*输    入：xd_u8 SMUTEA, xd_u8 SMUTER, xd_u8 AM_SMTH, xd_u8 VOLUMET		 */
/*返    回：无																 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
void KT_AM_SOFTMUTE_SETTING(xd_u8 SMUTEA, xd_u8 SMUTER, xd_u8 AM_SMTH, xd_u8 VOLUMET)
{
	xd_u16 reg2E;
	reg2E = KT_Bus_Read(0x2E);
	KT_Bus_Write(0x2E,(reg2E & 0x0007) | (SMUTEA<<14) | (SMUTER<<12) | (AM_SMTH<<9) | (VOLUMET<<4) | 0x0000 );
//									SMUTEA=4,SMUTER=120ms,AM_SMTH=3,VOLUMET=1,SMMD=RSSI mode
}
#endif

/*****************************************************************************/
/*函 数 名：KT_FM_SOFTMUTE												 	 */
/*功能描述：FM 自动静音程序												  	 */
/*函数说明：																 */
/*调用函数：KT_FMValidStation()、KT_Bus_Read()、KT_Bus_Write()、			 */
/*			KT_FM_SOFTMUTE_SETTING()		 								 */
/*全局变量：无																 */
/*输    入：xd_u16 Frequency													 */
/*返    回：无																 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
#ifdef FM_SOFTMUTE
void KT_FM_SOFTMUTE(xd_u16 Frequency)
{
	xd_u16 reg4;

	reg4 = KT_Bus_Read(0x04);

	if(KT_FMValidStation(Frequency))
	{
//		KT_FM_SOFTMUTE_SETTING(2,3,4,5);					// SMUTEA=4,SMUTER=60ms,SMMD=SNR mode,FM_SMTH=9,VOLUMET=5
		KT_Bus_Write(0x04,reg4 | 0x8000);					// FM Softmute Disable

	}
	else
	{
		KT_FM_SOFTMUTE_SETTING(0,3,7,5);					// SMUTEA=16,SMUTER=60ms,SMMD=SNR mode,FM_SMTH=12,VOLUMET=5
		KT_Bus_Write(0x04,reg4 & 0x7FFF);					// FM Softmute Enable
	}
//	reg4=KT_Bus_Read(0x04);									// FM Softmute Enable
//	KT_Bus_Write(0x04,reg4 & 0x7FFF);
}

/*****************************************************************************/
/*函 数 名：KT_FM_SOFTMUTE_SETTING										 	 */
/*功能描述：FM 自动静音设置程序											  	 */
/*函数说明：																 */
/*调用函数：KT_Bus_Read()、KT_Bus_Write()									 */
/*全局变量：无																 */
/*输    入：xd_u8 SMUTEA, xd_u8 SMUTER, xd_u8 FM_SMTH, xd_u8 VOLUMET		 */
/*返    回：无																 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
void KT_FM_SOFTMUTE_SETTING(xd_u8 SMUTEA, xd_u8 SMUTER, xd_u8 FM_SMTH, xd_u8 VOLUMET)
{
	xd_u16 reg2E;
	reg2E = KT_Bus_Read(0x2E);
	KT_Bus_Write(0x2E,(reg2E & 0x0E00) | (SMUTEA<<14) | (SMUTER<<12) | (VOLUMET<<4) | 0x0008 | FM_SMTH );
//									SMUTEA=4,SMUTER=120ms,VOLUMET=1,SMMD=SNR mode,FM_SMTH=3
}
#endif

/*****************************************************************************/
/*函 数 名：KT_AM_SOFTMUTE_SETTING										 	 */
/*功能描述：AM 自动静音设置程序											  	 */
/*函数说明：																 */
/*调用函数：KT_Bus_Read()、KT_Bus_Write()									 */
/*全局变量：无																 */
/*输    入：xd_u8 SMUTEA, xd_u8 SMUTER, xd_u8 AM_SMTH, xd_u8 VOLUMET		 */
/*返    回：无																 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
#ifdef AM_SOFTMUTE_AFCMODE
void KT_AM_SOFTMUTE_SETTING(xd_u8 SMUTEA, xd_u8 SMUTER, xd_u8 AM_SMTH, xd_u8 VOLUMET)
{
	xd_u16 reg2E;
	reg2E = KT_Bus_Read(0x2E);
	KT_Bus_Write(0x2E,(reg2E & 0x0007) | (SMUTEA<<14) | (SMUTER<<12) | (AM_SMTH<<9) | (VOLUMET<<4) | 0x0000 );
}
/*****************************************************************************/
/*函 数 名：KT_AM_SOFTMUTE_AFCMODE										 	 */
/*功能描述：AM 软件静音AFC（自动频率控制）模式							  	 */
/*函数说明：																 */
/*调用函数：KT_Bus_Read()、KT_AMGetAFC()、KT_AMReadRSSI()、KT_Bus_Write()	 */
/*全局变量：无																 */
/*输    入：bit Valid														 */
/*返    回：无																 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
/*
//-----------AM SOFTMUTE AFC MODE A:-----------// 
void KT_AM_SOFTMUTE_AFCMODE(bit Valid)
{
	char xdata rssi_value;
	xd_u8 am_afc;
	xd_u16 reg4,regF;

	reg4=KT_Bus_Read(0x04);
 	am_afc = abs(KT_AMGetAFC());

	if(Valid == 1)
	{
		KT_AMReadRSSI(&rssi_value);
		if(rssi_value>-60)
		{
			KT_Bus_Write(0x04,reg4 | 0x4000);					// AM Softmute Disable
			KT_AM_AFC_VolumeSet(am_afc);
		}
		if(rssi_value<-66)
		{
			if((reg4 & 0x4000) == 0x4000)						// if softmute disable
			{
				KT_AM_SOFTMUTE_SETTING(2,2,7,15);				// SMUTEA=4,SMUTER=30ms,AM_SMTH=8,VOLUMET=15,SMMD=RSSI mode
				KT_Bus_Write(0x04,reg4 & 0xBFFF);				// AM Softmute Enable
			}
		}
		CODE_Send(0x9B);										// 显示B0-B3
		DATA_Send('V');
		DATA_Send('C');
	}
	else
	{
		KT_Bus_Write(0x04,reg4 | 0x4000);						// AM Softmute Disable
		if(am_afc<13)
		{
			KT_AMReadRSSI(&rssi_value);
			if(rssi_value<-72)
			{
				regF=KT_Bus_Read(0x0F); 
				KT_Bus_Write(0x0F, regF & 0xFFE0 | 0x000F); 
			}
			if(rssi_value>-63)
			{
				KT_AMFMUnMute();
			}
		}
		else
		{
			KT_AM_AFC_VolumeSet(am_afc);
		}		
		CODE_Send(0x9B);										// 显示B0-B3
		DATA_Send(' ');
		DATA_Send(' ');
	}
}


void KT_AM_AFC_VolumeSet(xd_u8 afc)			//Input: 0~128
{
	xd_u16 regx;
	char afc_volume;

	regx=KT_Bus_Read(0x0F); 

	if(afc >= 60)					//+-8
	{
		KT_AMFMMute();
	}
	if( (afc < 60) && (afc >= 52) )	//+-7
	{
		afc_volume = mem_vol-28;
		if(afc_volume<15)
			afc_volume = 15;
		KT_Bus_Write(0x0F, regx & 0xFFE0 | afc_volume);
	}
	if( (afc < 52) && (afc >= 44) )	//+-6
	{
		afc_volume = mem_vol-24;
		if(afc_volume<15)
			afc_volume = 15;
		KT_Bus_Write(0x0F, regx & 0xFFE0 | afc_volume);
	}
	if( (afc < 44) && (afc >= 36) )	//+-5
	{
		afc_volume = mem_vol-20;
		if(afc_volume<15)
			afc_volume = 15;
		KT_Bus_Write(0x0F, regx & 0xFFE0 | afc_volume);
	}
	if( (afc < 36) && (afc >= 28) )	//+-4
	{
		afc_volume = mem_vol-16;
		if(afc_volume<15)
			afc_volume = 15;
		KT_Bus_Write(0x0F, regx & 0xFFE0 | afc_volume);
	}
	if( (afc < 28) && (afc >= 20) )	//+-3
	{
		afc_volume = mem_vol-6;
		if(afc_volume<15)
			afc_volume = 15;
		KT_Bus_Write(0x0F, regx & 0xFFE0 | afc_volume);
	}
	if( (afc < 20) && (afc >= 12) )//+-2
	{
		afc_volume = mem_vol-5;
		if(afc_volume<15)
			afc_volume = 15;
		KT_Bus_Write(0x0F, regx & 0xFFE0 | afc_volume);
	}
	if( (afc < 12) && (afc >= 4) )	//+-1
	{
		afc_volume = mem_vol-1;

		if(afc_volume<15)
			afc_volume = 15;
		KT_Bus_Write(0x0F, regx & 0xFFE0 | afc_volume);
	}
	if(afc < 4)						//+-0
	{
		KT_Bus_Write(0x0F, regx & 0xFFE0 | mem_vol);
	}
}
//-----------AM SOFTMUTE AFC MODE A END-----------// 
*/

/*****************************************************************************/
/*函 数 名：KT_AM_SOFTMUTE_AFCMODE										 	 */
/*功能描述：AM 自动静音AFC（自动频率控制）模式							  	 */
/*函数说明：																 */
/*调用函数：KT_Bus_Read()、KT_AMGetAFC()、KT_AMReadRSSI()、KT_Bus_Write()、	 */
/*			KT_AM_TUNING_LIGHT()、KT_AM_SOFTMUTE_SETTING()、CODE_Send()、	 */
/*			KT_AM_AFC_VolumeSet()、DATA_Send()								 */
/*全局变量：无																 */
/*输    入：无																 */
/*返    回：无																 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
//-----------AM SOFTMUTE AFC MODE B-----------// 
void KT_AM_SOFTMUTE_AFCMODE(void)
//void KT_AM_SOFTMUTE_AMTUNINGLIGHTMODE(bit Valid)
{
	char xdata rssi_value;
	xd_u8 am_afc;
	xd_u16 reg4;

	reg4=KT_Bus_Read(0x04);
 	am_afc = abs(KT_AMGetAFC());

	if(KT_AM_TUNING_LIGHT())
	{
		KT_AMReadRSSI(&rssi_value);
		if(rssi_value > -60)
		{
			KT_Bus_Write(0x04,reg4 | 0x4000);					// AM Softmute Disable
		}
		if(rssi_value < -66)
		{
			if((reg4 & 0x4000) == 0x4000)						// if softmute disable
			{
				KT_AM_SOFTMUTE_SETTING(2,2,7,15);				// SMUTEA=4,SMUTER=30ms,AM_SMTH=8,VOLUMET=15,SMMD=RSSI mode
				KT_Bus_Write(0x04,reg4 & 0xBFFF);				// AM Softmute Enable
			}
		}
		CODE_Send(0x9B);										// 显示B0-B3
		DATA_Send('V');
		DATA_Send('C');
	}
	else
	{
		KT_Bus_Write(0x04,reg4 | 0x4000);						// AM Softmute Disable
		KT_AM_AFC_VolumeSet(am_afc);
		
		CODE_Send(0x9B);										// 显示B0-B3
		DATA_Send(' ');
		DATA_Send(' ');
	}
}

/*****************************************************************************/
/*函 数 名：KT_AM_AFC_VolumeSet											 	 */
/*功能描述：AM AFC音量设置程序											  	 */
/*函数说明：																 */
/*调用函数：KT_Bus_Read()、KT_AMFMMute()、KT_Bus_Write()					 */
/*全局变量：无																 */
/*输    入：xd_u8 afc														 */
/*返    回：无																 */
/*设 计 者：Kanghekai				时间：											*/
/*修 改 者：Kanghekai				时间：2011-04-08								*/
/*版    本：V4.0																	*/
/************************************************************************************/
void KT_AM_AFC_VolumeSet(xd_u8 afc)			//Input: 0~128
{
	xd_u16 regx;
	char afc_volume;

	regx = KT_Bus_Read(0x0F); 

	if(afc >= 60)					//+-8
	{
		KT_AMFMMute();
	}
	if( (afc < 60) && (afc >= 52) )	//+-7
	{
		afc_volume = mem_vol - 28;
		if(afc_volume < 15)
			afc_volume = 15;
		KT_Bus_Write(0x0F, regx & 0xFFE0 | afc_volume);
	}
	if( (afc < 52) && (afc >= 44) )	//+-6
	{
		afc_volume = mem_vol - 24;
		if(afc_volume < 15)
			afc_volume = 15;
		KT_Bus_Write(0x0F, regx & 0xFFE0 | afc_volume);
	}
	if( (afc < 44) && (afc >= 36) )	//+-5
	{
		afc_volume = mem_vol - 20;
		if(afc_volume < 15)
			afc_volume = 15;
		KT_Bus_Write(0x0F, regx & 0xFFE0 | afc_volume);
	}
	if( (afc < 36) && (afc >= 28) )	//+-4
	{
		afc_volume = mem_vol - 16;
		if(afc_volume < 15)
			afc_volume = 15;
		KT_Bus_Write(0x0F, regx & 0xFFE0 | afc_volume);
	}
	if( (afc < 28) && (afc >= 20) )	//+-3
	{
		afc_volume = mem_vol - 6;
		if(afc_volume < 15)
			afc_volume = 15;
		KT_Bus_Write(0x0F, regx & 0xFFE0 | afc_volume);
	}
	if( (afc < 20) && (afc >= 12) )//+-2
	{
		afc_volume = mem_vol - 5;
		if(afc_volume < 15)
			afc_volume = 15;
		KT_Bus_Write(0x0F, regx & 0xFFE0 | afc_volume);
	}
	if( (afc < 12) && (afc >= 4) )	//+-1
	{
		afc_volume = mem_vol - 1;

		if(afc_volume < 15)
			afc_volume = 15;
		KT_Bus_Write(0x0F, regx & 0xFFE0 | afc_volume);
	}
	if(afc < 4)						//+-0
	{
		afc_volume = 15;
		KT_Bus_Write(0x0F, regx & 0xFFE0 | afc_volume);
	}
}
//-----------AM SOFTMUTE AFC MODE B END-----------// 
#endif

#endif

