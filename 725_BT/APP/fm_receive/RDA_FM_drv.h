#ifndef	_RDA_FM_DRV_H_
#define _RDA_FM_DRV_H_

void  RDA5807P_PowerOffProc(void);
void RDA5807P_SetFreq( int16 curFreq );
bool RDA5807P_ValidStop(u16 freq);
bool  RDA5807P_Intialization(void);
u16 RDA5807P_Get_ID();
void RDA5807P_SetMute(bool mute);
void RDA5807P_SetVolumeLevel(u8 level);

#endif
