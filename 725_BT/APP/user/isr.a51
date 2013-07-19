/*--------------------------------------------------------------------------*/
/**@file    isr.a51
   @brief   中断入口模块
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/

#include "AC209N.h"
#include "lcd.h"
#include "typedef.h"
#include "key.h"
#include "clock.h"
#include "config.h"
                     
	extrn code (?C?UIDIV)
	extrn code (?C?OFFXADD)
	extrn idata (irBuffer)
	extrn idata (irStep)
	extrn idata (user_code)
//	extrn data (currPage)
//	extrn data (updata_page_len)
//	extrn bit (lcd_busy)
//	extrn code (_LCD_SET)
//	extrn xdata (LCDBuff)
    extrn data (keyBack)
    extrn data (keyCnt)
    extrn data (key_value)
    extrn code (keyDetect)
    extrn code (key_tone)
    extrn code (_put_msg_fifo)
	extrn code (_mainclock_select)
	extrn code (timer1isr)
	extrn code (key_power)
	extrn code (flush_low_msg)
#if VOICE_TIME_ENABLE
    extrn data (otp_music_addr)
#endif

#if OTP_MUSIC_ENABLE
	extrn code (music_file)
#endif
	extrn code (_decode_user_exit)

	cseg at VECTOR(TIMER3_INT)
		jmp timer3isr
/*		
	cseg at VECTOR(DMA_INT)
		jmp spi_dmaisr	
*/		
	cseg at VECTOR(TIMER1_INT)
		jmp timer1_isr

//----------------------------------------------------------------------------------------
//timer0 
//----------------------------------------------------------------------------------------
	?pr?timer1_isr?remote segment code
	RSEG  ?PR?timer1_isr?remote
timer1_isr:
	push    acc
	push    b
	push    dpl
	push    dph
	push    psw
	mov     psw, #0x08		//using 1
	jb      IP0.1,timer1_isr_1
	mov     psw, #0x10		//using 2
timer1_isr_1:
	call    timer1isr
	pop     psw
	pop     dph
	pop     dpl
	pop     b
	pop     acc	 	
	reti
		
/*		
//----------------------------------------------------------------------------------------
//LCD 
//----------------------------------------------------------------------------------------
	?pr?spi_dmapro?remote segment code
	RSEG  ?PR?spi_dmapro?remote
	public  spi_dmapro
spi_dmapro:		
    LCD_CS_H
	mov    a,updata_page_len
	jnz    spi_dmaisr_1
	clr    lcd_busy
	SPIDMAISRDIS
	sjmp    spi_dmapro_exit
	
spi_dmaisr_1:
	mov    r7, currPage
	mov    r5, #0
	call   _LCD_SET 
	
	LCD_A0_H
	nop
	nop
	nop 
	LCD_CS_L
	
	mov     b,#LCDCOLUMN
	mov     a,currPage
	mov     dptr,#LCDBuff
	call    ?C?OFFXADD
	mov     SPIDMAADR, dph
	mov     SPIDMAADR,dpl
	mov     SPIDMACNT, #LCDCOLUMN >> 1
	inc     currPage
	dec     updata_page_len
spi_dmapro_exit:
	ret			
		
//----------------------------------------------------------------------------------------
//LCD 
//----------------------------------------------------------------------------------------
	?pr?spi_dmaisr?remote segment code
	RSEG  ?PR?spi_dmaisr?remote
spi_dmaisr:		
	push   acc
	push   b
	push   psw
	push   dpl
	push   dph
	push   dpcon
	mov    psw,#0x08
	mov    dpcon, #0
	call   spi_dmapro
	pop    dpcon
	pop    dph
	pop    dpl
	pop    psw
	pop    b
	pop    acc	
	reti
*/	
//----------------------------------------------------------------------------------------
//红外遥控 
/*/----------------------------------------------------------------------------------------*/

	?pr?timer3isr?remote segment code
	RSEG  ?PR?timer3isr?remote
timer3isr:
	//** 固件架构相关代码 start ***//
    PUSH 	ACC
	PUSH 	B
	PUSH 	PSW
	MOV  	PSW,#08H
	PUSH 	DPCON
	MOV  	DPCON,#00H
    //** 固件架构相关代码 end ***//
	MOV  	A,T3CON
	JNB  	ACC.6,timer3isr_1    ;///<no capture pending jmp to timer3isr_1
	MOV  	A,T3PWM              ;///<get the capture cnt HSB
    MOV     R6,A
	MOV  	R7,T3PWM             ;///<get the capture cnt LSB
    ANL     T3CON, #~02
	CLR  	A
	MOV  	T3CNTL,A             ;///<CLR the capture cnt HSB
    MOV  	T3CNTH,A             ;///<CLR the capture cnt LSB
    ORL     T3CON,#0x2           ;///<SET TIMER WORK AT CAPTURE MODE
	ANL  	T3CON,#0BFH          ;///<SET TIMER WORK AT CAPTURE MODE
	MOV  	R4,#0		//01H    ;///<set a ms paramete  HSB
	MOV  	R5,#33		//077H   ;///<set a ms paramete  LSB
	CALL	?C?UIDIV             ;///<调用库函数获取一个capturn到的毫秒数 结果存放在R7中
	SJMP 	timer3isr_2          ;///<跳转到 timer3isr_2
timer3isr_1:
	ANL  	T3CON,#07FH          ;///<clr Timer Pending
	MOV  	R7,#00H              ;///<在没有CAPTURE CAPTURE PENDING的情况下将得到的的MS清零
timer3isr_2:
	MOV  	A,#08H                        ;///<
	CLR  	C
	SUBB 	A,R7                          ;///8-r7
	JNC  	timer3isr_3                   ;///c != 1 跳转
	MOV  	A,R7
	CLR  	C
	SUBB 	A,#032H                       ;///<
	JC   	?timer3isr?_exit_timer3       ;///c == 1 跳转
timer3isr_3:
	MOV  	A,#050H
	CLR  	C
	SUBB 	A,R7
	JNC  	timer3isr_4
	MOV  	A,R7
	CLR  	C
	SUBB 	A,#06EH
	JC   	?timer3isr?_exit_timer3
timer3isr_4:
	MOV  	R1,#irBuffer                ///<irBuffer右移一位
	MOV  	A,@R1
	CLR  	C
	RRC  	A
	MOV  	@R1,A
	INC  	R1
	MOV  	A,@R1
	RRC  	A
	MOV  	@R1,A

	MOV  	R0,#irStep
	INC  	@R0

	CJNE 	R7,#01H,timer3isr_5        ///<r7 != 1 跳转，r7=1时当前BIT为0
	SJMP 	?timer3isr?_exit_timer3

timer3isr_5:

	CJNE 	R7,#02H,timer3isr_6       ///<r7 != 2 跳转，r7=2时当前BIT为1
	MOV  	R0,#irBuffer
	MOV  	A,@R0
	ORL  	A,#080H
	MOV  	@R0,A
	SJMP 	?timer3isr?_exit_timer3
timer3isr_6:
	CLR     A
	MOV  	R0,#irStep
	MOV  	@R0,A
	MOV  	R0,#irBuffer
	MOV  	@R0,A
	INC  	R0
	MOV  	@R0,#0FFH
	
?timer3isr?_exit_timer3:
/*  get L8 bit user code 
	MOV  	R0,#irStep
	MOV  	A,@R0
	CJNE 	A,#010H,timer3isr_7
	MOV  	R0,#irBuffer+01H
	MOV  	A,@R0
	MOV  	R0,#user_code
	MOV  	@R0,A
*/
///<get 16 bit user code  
	MOV  	R0,#irStep                          ;///<获取16位用户码
	MOV  	A,@R0
	CJNE 	A,#010H,timer3isr_7                 ;///<irStep == 16 时，用户码提取完成
	MOV  	R0,#irBuffer
	MOV  	A,@R0
	MOV  	R0,#user_code
	MOV  	@R0,A
	MOV  	R0,#irBuffer+01H
	MOV  	A,@R0
	MOV  	R0,#user_code+01H
	MOV  	@R0,A
timer3isr_7:
    //** 固件架构相关代码 start ***//
	POP  	DPCON
	POP  	PSW
	POP  	B
	POP  	ACC
    //** 固件架构相关代码 end ***//
	RETI
#if 0
//----------------------------------------------------------------------------------------
//按键检测
//----------------------------------------------------------------------------------------
    ?PR?keyScan?KEY segment code
    public keyScan
    RSEG  ?PR?keyScan?KEY
keyScan:
	CALL	keyDetect
	MOV  	R5,#NO_KEY
	MOV  	A,R7
	CPL  	A
	JNZ  	?C0008
    CALL    key_power
	MOV  	A,R7
	CPL  	A
	JNZ  	?C0008
    MOV     R7,key_value
	//MOV  	R7,ADCDATH
	//ORL  	ADCCON,#080H
	MOV  	A,R7
	SETB 	C
	SUBB 	A,#ADKEY_RES_NOKEY
	JC   	?C0009
	MOV  	R7,#ADKEY_NOKEY
	SJMP 	?C0008
?C0009:
	MOV  	A,R7
	SETB 	C
	SUBB 	A,#ADKEY_RES_7
	JC   	?C0011
	MOV  	R7,#ADKEY_8
	SJMP 	?C0008
?C0011:
	MOV  	A,R7
	SETB 	C
	SUBB 	A,#ADKEY_RES_6
	JC   	?C0013
	MOV  	R7,#ADKEY_7
	SJMP 	?C0008
?C0013:
	MOV  	A,R7
	SETB 	C
	SUBB 	A,#ADKEY_RES_5
	JC   	?C0015
	MOV  	R7,#ADKEY_6
	SJMP 	?C0008
?C0015:
	MOV  	A,R7
	SETB 	C
	SUBB 	A,#ADKEY_RES_4
	JC   	?C0017
	MOV  	R7,#ADKEY_5
	SJMP 	?C0008
?C0017:
	MOV  	A,R7
	SETB 	C
	SUBB 	A,#ADKEY_RES_3
	JC   	?C0019
	MOV  	R7,#ADKEY_4
	SJMP 	?C0008
?C0019:
	MOV  	A,R7
	SETB 	C
	SUBB 	A,#ADKEY_RES_2
	JC   	?C0021
	MOV  	R7,#ADKEY_3
	SJMP 	?C0008
?C0021:
	MOV  	A,R7
	SETB 	C
	SUBB 	A,#ADKEY_RES_1
	MOV  	R7,#ADKEY_1
	JC   	?C0008
	MOV  	R7,#ADKEY_2

?C0008:
	MOV  	A,R7
	CPL  	A
	JZ   	?C0026
	MOV  	A,R7
	XRL  	A,keyBack
	JZ   	?C0025
?C0026:
	MOV  	A,keyCnt
	CLR  	C
	SUBB 	A,#KEY_LONG_CNT
	JC   	?C0027
	MOV  	A,keyBack
	ORL  	A,#KEY_LONG_UP
	;//////在长按抬起的情况下，清消息池/////////
	push    acc
	CALL    flush_low_msg
	pop     acc
	////////////////////////////////////////
	MOV  	R5,A
	SJMP 	?C0028
?C0027:
	MOV  	A,keyCnt
	CLR  	C
	SUBB 	A,#KEY_BASE_CNT
	JC   	?C0028
	MOV  	A,keyBack
	ORL  	A,#KEY_SHORT_UP
	MOV  	R5,A
?C0028:
	CLR  	A
	MOV  	keyCnt,A
	MOV  	keyBack,R7
	
	SJMP 	?C0030
?C0025:
	INC  	keyCnt
	MOV  	A,keyCnt
	CJNE 	A,#KEY_BASE_CNT,?C0031
	CALL	key_tone
	MOV  	R5,keyBack
	SJMP 	?C0030
?C0031:
	MOV  	A,keyCnt
	CJNE 	A,#KEY_LONG_CNT,?C0033
	//CALL	key_tone
	MOV  	A,keyBack
	ORL  	A,#KEY_LONG
	MOV  	R5,A
	SJMP 	?C0030
?C0033:
	MOV  	A,keyCnt
	CJNE 	A,#(KEY_LONG_CNT + KEY_HOLD_CNT),?C0030
	//CALL	key_tone
	MOV  	A,keyBack
	ORL  	A,#KEY_HOLD
	MOV  	R5,A
	MOV  	keyCnt,#KEY_LONG_CNT
?C0030:
	MOV  	A,R5
	CPL  	A
	JZ   	?C0037
	XCH  	A,R7
	MOV  	A,R5
	XCH  	A,R7
	MOV		A,R7
/*	PUSH	ACC
//通过按键强制控制解码读接口
	MOV		A,#INFO_NEXT_FIL
	ORL		A,#KEY_SHORT_UP
	XRL		A,R5
	JNZ		normal_key
	MOV		R7,#1
	LCALL	_decode_user_exit
normal_key:
	MOV		A,#INFO_PREV_FIL
	ORL		A,#KEY_SHORT_UP
	XRL		A,R5
	JNZ		normal_key1
	MOV		R7,#1
	LCALL  	_decode_user_exit
normal_key1:
	POP		ACC
*/	
	MOV		R7,A
	CALL	_put_msg_fifo
?C0037:
	RET    	
#endif 
	
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#if ((VOICE_TIME_ENABLE== 1) ||(OTP_MUSIC_ENABLE== 1))
        ?PR?_code_otp_read?CODE_ENTRY SEGMENT CODE 
        RSEG     ?PR?_code_otp_read?CODE_ENTRY
        PUBLIC   _code_otp_read
_code_otp_read:
    
        PUSH    DPCON
        MOV     DPCON,#0x0                                 
    //  MOV     DPTR,  #otp_music_addr
#if VOICE_TIME_ENABLE
        MOV     A, otp_music_addr
        MOV     DPH,a
        MOV     A, otp_music_addr+1
        MOV     DPL,A
#else
        MOV     DPTR, #music_file
#endif
        MOV     A, R3
        CLR     C
        RLC     A
        ADDC    A,DP0H
        MOV     DP0H,A
        MOV     DPCON, #0x58

        MOV     DP1L, R7
        MOV     DP1H, R6

        MOV     R7,#0
        MOV     R6,#0x02
_code_otp_read_loop:
        CLR     A
        MOVC    A,@A+DPTR
        MOVX    @DPTR,A
        DJNZ    R7,_code_otp_read_loop
        DJNZ    R6,_code_otp_read_loop

        MOV     R7, #0

        POP     DPCON
        RET
#endif

        ?PR?read_crc?CODE_ENTRY SEGMENT CODE 
        RSEG     ?PR?read_crc?CODE_ENTRY
        PUBLIC   read_crc
read_crc:
        mov a,CRCREG		   
        mov r6,a
        mov a,CRCREG		   
        mov r7,a
		ret

		end
