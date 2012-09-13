/*--------------------------------------------------------------------------*/
/**@file     AC209N.h
   @brief    AC209NÍ·ÎÄ¼þ
   @author
   @date   2010-12-13
   @note   AC209N
*/
/*----------------------------------------------------------------------------*/
#ifndef _AC209N_
#define _AC209N_

sfr P0 = 0x80;
sfr SP = 0x81;
sfr DP0L = 0x82;
sfr DP0H = 0x83;
sfr CLKGAT = 0x85;
sfr CLKCON = 0x86;
sfr PCON = 0x87;
sfr DPCON = 0x88;
sfr SPH = 0x89;
sfr DP1L = 0x8A;
sfr DP1H = 0x8B;
sfr WKUPCON = 0x8C;
sfr P4DIR = 0x8D;
sfr P4PU = 0x8E;
sfr P1 = 0x90;
sfr LVDCON = 0x91;
sfr P0PD = 0x92;
sfr P1PDA = 0x93;
sfr P3PD = 0x94;
sfr P3HD = 0x95;
sfr IRTCON = 0x96;
sfr IRTBUF = 0x97;
sfr IICSTA = 0x98;
sfr ADCDATL = 0x99;
sfr ADCDATH = 0x9A;
sfr ADCCON = 0x9B;
sfr P1PDB = 0x9C;
sfr IICCON = 0x9D;
sfr IICBAUD = 0x9E;
sfr IICBUF = 0x9F;
sfr P2 = 0xA0;
sfr T0CON = 0xA1;
sfr T0SCA = 0xA2;
sfr T0CNT = 0xA3;
sfr T0PRD = 0xA4;
sfr T0PWM = 0xA5;
sfr T2CON = 0xA6;
sfr T2SCA = 0xA7;
sfr IE0 = 0xA8;
sfr IE1 = 0xA9;
sfr T2CNTL = 0xAA;
sfr T2CNTH = 0xAB;
sfr T2PRD = 0xAC;
sfr DACCON0 = 0xAD;
sfr T2PWM = 0xAE;
sfr DACCON1 = 0xAF;
sfr P3 = 0xB0;
sfr T1CON = 0xB1;
sfr T1SCA = 0xB2;
sfr T1CNT = 0xB3;
sfr T1PRD = 0xB4;
sfr T1PWM = 0xB5;
sfr T3CON = 0xB6;
sfr T3SCA = 0xB7;
sfr IP0 = 0xB8;
sfr IP1 = 0xB9;
sfr T3CNTL = 0xBA;
sfr T3CNTH = 0xBB;
sfr T3PRD = 0xBC;
sfr DACCON2 = 0xBD;
sfr T3PWM = 0xBE;
sfr ADSPPND = 0xC0;
sfr SDBAUD = 0xC1;
sfr CRCREG = 0xC4;
sfr USBCON0 = 0xC8;
sfr CRCFIFO = 0xCA;
sfr P0IE = 0xCC;
sfr P1IE = 0xCD;
sfr IRCON0 = 0xCE;
sfr IRCON1 = 0xCF;
sfr PSW = 0xD0;
sfr SPICON = 0xD1;
sfr SPIBAUD = 0xD2;
sfr SPIBUF = 0xD3;
sfr SPIDMACON = 0xD4;
sfr SPIDMACNT = 0xD5;
sfr SPIDMAADR = 0xD6;
sfr CPUOBUF = 0xD7;
sfr ACC = 0xE0;
sfr P3IE = 0xE1;
sfr P0DIR = 0xE4;
sfr P1DIR = 0xE5;
sfr P2DIR = 0xE6;
sfr P3DIR = 0xE7;
sfr P4 = 0xE8;
sfr WKUPPND = 0xE9;
sfr UTCON = 0xEA;
sfr UTSTA = 0xEB;
sfr UTBAUD = 0xEC;
sfr UTBUF = 0xED;
sfr MPCON3 = 0xEF;
sfr B = 0xF0;
sfr EMICON = 0xF1;
sfr EMIBUF = 0xF2;
sfr ADSPCMD = 0xF3;
sfr P0PU = 0xF4;
sfr P1PU = 0xF5;
sfr P2PU = 0xF6;
sfr P3PU = 0xF7;
sfr ADSPINT = 0xF8;


#define VECTOR(n)	(n*8 + 3)
#define TIMER0_INT	0
#define TIMER1_INT	1
#define TIMER2_INT	2
#define TIMER3_INT	3
#define SPI_INT	    4
#define IIC_INT	    5
#define SOFT_INT	  6
#define UART_INT	  7
#define PORT_INT	  8
#define DMA_INT	    11		/*for SPI & EMI*/
#define LVD_RTC_INT	15



//PSW
sbit CY    = 0xD7;
sbit AC    = 0xD6;
sbit F0    = 0xD5;
sbit RS1   = 0xD4;
sbit RS0   = 0xD3;
sbit OV    = 0xD2;
sbit P     = 0xD0;

/*port*/
sbit P00 = P0 ^ 0;
sbit P01 = P0 ^ 1;
sbit P02 = P0 ^ 2;
sbit P03 = P0 ^ 3;
sbit P04 = P0 ^ 4;
sbit P05 = P0 ^ 5;
sbit P06 = P0 ^ 6;
sbit P07 = P0 ^ 7;

sbit P10 = P1 ^ 0;
sbit P11 = P1 ^ 1;
sbit P12 = P1 ^ 2;
sbit P13 = P1 ^ 3;
sbit P14 = P1 ^ 4;
sbit P15 = P1 ^ 5;
sbit P16 = P1 ^ 6;
sbit P17 = P1 ^ 7;


sbit P20 = P2 ^ 0;
sbit P21 = P2 ^ 1;
sbit P22 = P2 ^ 2;
sbit P23 = P2 ^ 3;
sbit P24 = P2 ^ 4;
sbit P25 = P2 ^ 5;
sbit P26 = P2 ^ 6;
sbit P27 = P2 ^ 7;


sbit P30 = P3 ^ 0;
sbit P31 = P3 ^ 1;
sbit P32 = P3 ^ 2;
sbit P33 = P3 ^ 3;
sbit P34 = P3 ^ 4;
sbit P35 = P3 ^ 5;
sbit P36 = P3 ^ 6;
sbit P37 = P3 ^ 7;


sbit P40 = P4 ^ 0;
sbit P41 = P4 ^ 1;
sbit P42 = P4 ^ 2;
sbit P43 = P4 ^ 3;
sbit P44 = P4 ^ 4;
sbit P45 = P4 ^ 5;
sbit P46 = P4 ^ 6;
sbit P47 = P4 ^ 7;

///////////
sbit DPSEL = DPCON ^ 0;
sbit DSP_TOGGLE = DPCON ^3;




/*  IE   */
sbit ET0  = IE0 ^0;
sbit ET1  = IE0 ^1;
sbit ET2  = IE0 ^2;
sbit ET3  = IE0 ^3;
sbit ESPI = IE0 ^4;
sbit EIIC = IE0 ^5;
sbit ESFT = IE0 ^6;
sbit EA   = IE0 ^7;

/*  IP   */
sbit PT0  = IP0 ^0;
sbit PT1  = IP0 ^1;
sbit PT2  = IP0 ^2;
sbit PT3  = IP0 ^3;
sbit PSPI = IP0 ^4;
sbit PIIC = IP0 ^5;
sbit PSFT = IP0 ^6;
sbit PUART = IP0 ^7;


sbit IIC_STA_BIT  = IICSTA^3;
sbit IIC_SEND_ACK  = IICSTA^4;
sbit IIC_GET_ACK  = IICSTA^5;
sbit IIC_END_PND  = IICSTA^6;
sbit IIC_NOR_PND  = IICSTA^7;


#endif
