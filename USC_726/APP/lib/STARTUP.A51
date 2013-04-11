/*--------------------------------------------------------------------------*/
/**@file     startup.a51
   @brief    Æô¶¯´úÂë
   @details
   @author
   @date   2010-12-13
   @note
*/
/*----------------------------------------------------------------------------*/
$NOMOD51		  
#include "AC209N.h"


                NAME    ?C_STARTUP


?C_C51STARTUP   SEGMENT   CODE


				EXTRN CODE (pll_init)
                EXTRN CODE (?C_START)
				extrn code (dac_on)
                PUBLIC  ?C_STARTUP

                CSEG    AT      0x0
?C_STARTUP:     LJMP    STARTUP1

                RSEG    ?C_C51STARTUP

STARTUP1:
////////////////////////////////////////
				MOV     LVDCON, #0x60

				MOV     SP,#0x00
				MOV     SPH,#0x03

				CLR     A
				MOV     PSW, A
                MOV     DPCON, A
				
				MOV     LVDCON, #0x65		//LVDµçÑ¹2.4V(0x61:2.2V, 0x65:2.4V, 0x69:2.6V, 0x6D:2.8V)
				CALL	dac_on
				CALL    pll_init

                MOV		DPCON, #0x20		//DPTR0 AUTO DEC
                MOV     DPTR,#0x23FF		//9k
XDATALOOP:
                CLR     A
      			MOVX    @DPTR,A

				MOV     A,DPL
				XRL     A,#0xFF
				JNZ     XDATALOOP
				MOV     A,DPH
				XRL     A,#0xFF
				JNZ     XDATALOOP

				MOV     SP,#0x00
				MOV     SPH,#0x03

				CLR     A
				MOV     PSW, A
                MOV     DPCON, A   

                LJMP    ?C_START


                END
