#include "Custom_config.h"

#if defined(BLUE_TOOTH_UART_FUNC)
#include "blue_tooth.h"
#include "uart.h"

void blue_tooth_uart_init()
{
	sysclock_div2(1);
	uartInit();
	PUART=1;
	delay_10ms(1);
}
void blue_tooth_uart_release()
{
	UTCON = 0x00;
	PUART=0;
}
bool bt_frame_rev_finished=0;
xd_u8  uart_rev=0;
xd_u8 rev_phase=0,rev_length=0;
extern xd_u8 rev_cmd[7];

void uart_isr()  interrupt  7
{
    _push_(DPCON);
    _push_(DP1L);
    _push_(DP1H);
    DPCON = 0x0;

    uart_rev = UTBUF;

    	if((uart_rev==0xAA)&&(rev_phase==0)){

		rev_phase=1;
    	}
	
	if(rev_phase>0){
		
	   rev_cmd[rev_phase-1]=uart_rev;
	   
	   rev_phase++;

	   if(rev_phase>7){	   	
	   	rev_phase=0;
		bt_frame_rev_finished=1;
	   }
   	}
	else{
	   	rev_phase=0;
	}

    UTSTA &= ~BIT(6);

    _pop_(DP1H);
    _pop_(DP1L);
    _pop_(DPCON);
}

xd_u8 rev_cmd[7]={0};

#define REV_CMD_LEN		BT_STATUS_MAX

u8 _code BT_REV_CMD[REV_CMD_LEN][7] =
{
	0xAA ,0x00 ,0x03 ,0x01 ,0x02 ,0x00 ,0xFA,				//Power on
	0xAA ,0x00 ,0x03 ,0x01 ,0x01 ,0x00 ,0xFB,				// In pairing mode
	0xAA ,0x00 ,0x03 ,0x01 ,0x03 ,0x00 ,0xF9,				//Pairing completed
  	0xAA ,0x00 ,0x03 ,0x01 ,0x06 ,0x00 ,0xF6,				//|| A connect A2DP
  	0xAA ,0x00 ,0x03 ,0x01 ,0x0B ,0x00 ,0xF1,				//|| A connect AVRCP
  	0xAA ,0x00 ,0x03 ,0x01 ,0x08 ,0x00 ,0xF4, 			//	A disconnect A2DP
  	0xAA ,0x00 ,0x03 ,0x01 ,0x0C ,0x00 ,0xF0, 			//	A disconnect AVRCP
  	0xAA ,0x00 ,0x02 ,0x00 ,0x02 ,0xFC ,0xAA,				//|| ACK
	0xAA ,0x00 ,0x03 ,0x0F ,0x0F ,0x00 ,0xDF,				// DEVICE START PLAY
};

#define AT_CMD_LEN		(BT_CMD_SET_MAX)

u8 _code BT_AT_CMD[AT_CMD_LEN][7] =
{
	0xAA ,0x00 ,0x03 ,0x02 ,0x00 ,0x32 ,0xC9,				//PLAY
  	0xAA ,0x00 ,0x03 ,0x02 ,0x00 ,0x34 ,0xC7,				// NEXT
  	0xAA ,0x00 ,0x03 ,0x02 ,0x00 ,0x35 ,0xC6,				//PREV	
  	0xAA ,0x00 ,0x03 ,0x02 ,0x00 ,0x30 ,0xCB,				//V +
  	0xAA ,0x00 ,0x03 ,0x02 ,0x00 ,0x31 ,0xCA,				//V -
	0xAA ,0x00 ,0x03 ,0x02 ,0x00 ,0x36 ,0xC5,				//FF 
	0xAA ,0x00 ,0x03 ,0x02 ,0x00 ,0x37 ,0xC4,				// FR
	0xAA ,0x00 ,0x03 ,0x02 ,0x00 ,0x33 ,0xC8,				//STOP
	0xAA ,0x00 ,0x03 ,0x02 ,0x00 ,0x3B ,0xC0,				// DISCONNEXT
	0xAA, 0x00, 0x03, 0x02, 0x00 ,0x50 ,0xAB,				// entern piaring mode
 	0xAA, 0x00, 0x03, 0x02, 0x00 ,0x5D ,0x9E,				// FAST PAIRING MODE
};

#ifdef BLUETOOTH_CONFIG_DEV_NAME
/*
AA 00 06 05 49 50 30 30 32 CA
checksum: 0xCA =0 - (00+0x06+0x05+0x49+0x50+0x30+0x30+0x32)
*/
u8 const _code bt_dev_name[]=
{
#if 1//def JK_IBT40_1094A_V001
	0xAA ,0x00 ,0x08 ,0x05 ,0x49 ,0x42 ,0x43 ,0x32 ,0x33,0x33,0x42,0x4B,	//IBC233B
#endif
};
void config_dev_name(void)
{
	u8 i=0;
	for(i=0;i<(sizeof(bt_dev_name));i++){
		putbyte(bt_dev_name[i]);
	}
}
#endif

void promt_bt_cmd(AT_PROMPT_CMD cmd)
{
	u8 i=0;
	if(cmd<AT_CMD_LEN){

		//UTCON = 0x01;

		for(i=0;i<7;i++){
			putbyte(BT_AT_CMD[cmd][i]);
		}

		//UTCON = 0x09;		
	}
}
xd_u8 parse_i=0;
u8 bluetooth_cmd_parse(void)
{
	u8 i=0xFF,j=0;

	
	UTCON = 0x01;
#if 0
	sys_printf("   ");
	for(j=0;j<7;j++)
		printf("bluetooth_cmd_parse    cmd_key %x \r\n",(u16)rev_cmd[j]);
#endif
	parse_i=rev_cmd[2]+1;
	for(i=0;i<REV_CMD_LEN;i++){

		for(j=0;j<parse_i;){


			if(rev_cmd[2+j]!=BT_REV_CMD[i][2+j]){
				j=0;
				break;
			}
			j++;
		}
		if(j==parse_i){
			break;
		}
	}

	if(j==0){
		i= 0xFF;
	}
		
	UTCON = 0x09;

	return i;
}
#endif

