/*! \page1 TIMER 编程指南
 *
 * \section intro_sec_timer TIMER 介绍
 * - AC209N 一共有4个TIMER，其中TIMER0和TIMER1是8位的TEMER;TIMER2和TIMER3是16位的TIMER。
 * \section timera timer0(8位) 介绍
 * - T0CON：可读可写\n
 *   Bit1~0：Timer0 Mode，00为Disable，01为Timer Mode，10为Capture Mode，11为PWM Mode\n
 *   Bit3~2：计时源选择，00为系统时钟，01为P03（上升沿），10为OSC，11为OSC32K\n
 *   Bit4：Capture中断允许，高有效\n
 *   Bit5：Timer中断允许，高有效\n
 *   Bit6：Capture Pending，Capture事件发生，产生中断\n
 *   Bit7：Timer Pending，Timer计数结束产生的Pending
 * - T0SCA：只写\n
 *   Bit3~0：Timer Prescaler设置寄存器(1/(2^n))：0000：无Prescale，0001：1Bit Prescale，……\n
 *   Bit5~4：Capture源选择，00：P02上升沿，01：IRDA上升沿，10：P02下降沿，11：IRDA下降沿\n
 *   Bit6：WatchDogEN，0：禁止本功能，1：允许Timer0溢出时复位芯片。
 * - T0CNT：可读可写\n
 *   Timer计数器
 * - T0PRD：只写\n
 *   Timer计数上限
 * - T0PWM：只写\n
 *   在PWM Mode时用于控制PWM的占空比。T0PWM的值要小于T0PRD，否则PWM输出一直都是1\n
 *   在Capture Mode时用于Capture计数器的值。\n
*    (注：PWM通过P30输出，有强输出可用)

 * \section timerB timer1(8位) 介绍
 * - T1CON：可读可写\n
 *   Bit1~0：Timer1 Mode，00为Disable，01为Timer Mode，10为Capture Mode，11为PWM Mode\n
 *   Bit3~2：计时源选择，00为系统时钟，01为P05（上升沿），10为OSC，11为OSC32K\n
 *   Bit4：Capture中断允许，高有效\n
 *   Bit5：Timer中断允许，高有效\n
 *   Bit6：Capture Pending，Capture事件发生，产生中断\n
 *   Bit7：Timer Pending，Timer计数结束产生的Pending
 * - T1SCA：只写\n
 *   Bit3~0：Timer Prescaler设置寄存器(1/(2^n))：0000：无Prescale，0001：1Bit Prescale，……\n
 *   Bit5~4：Capture源选择，00：P04上升沿，01：IRDA上升沿，10：P04下降沿，11：IRDA下降沿\n
 *   Bit6：WatchDogEN，0：禁止本功能，1：允许Timer0溢出时复位芯片。
 * - T1CNT：可读可写\n
 *   Timer计数器
 * - T1PRD：只写\n
 *   Timer计数上限
 * - T1PWM：只写\n
 *   在PWM Mode时用于控制PWM的占空比。T0PWM的值要小于T0PRD，否则PWM输出一直都是1\n
 *   在Capture Mode时用于Capture计数器的值。\n
 *    (注：PWM通过P03输出，无强输出)

 * \section timerC  timer2(16位) 介绍
 * - T2CON：可读可写\n
 *   Bit1~0：Timer0 Mode，00为Disable，01为Timer Mode，10为Capture Mode，11为PWM Mode\n
 *   Bit3~2：计时源选择，00为系统时钟，01为P13（上升沿），10为OSC，11为OSC32K\n
 *   Bit4：Capture中断允许，高有效\n
 *   Bit5：Timer中断允许，高有效\n
 *   Bit6：Capture Pending，Capture事件发生，产生中断\n
 *   Bit7：Timer Pending，Timer计数结束产生的Pending
 * - T2SCA：只写\n
 *   Bit2~0：Timer Prescaler设置寄存器：000：无Prescale，001：1Bit Prescale，…… \n
 *   Bit3：Reserved\n
 *   Bit5~4：Capture源选择，00：P23上升沿，01：IRDA上升沿，10：P23下降沿，11：IRDA下降沿
 * - T2CNTL、T2CNTH：可读可写\n
 *   Timer计数器
 * - T2PRDL、T2PRDH：只写\n
 *   通过T2PRD控制，先写高位再写低位\n
 *   Timer计数上限
 * - T2PWML、T2PWMH：可读可写\n
 *   通过T2PWM控制，写操作时先写高位再写低位，读操作时先读高位再读低位\n
 *   在PWM Mode时用于控制PWM的占空比。T2PWM的值要小于T2PRD，否则PWM输出一直都是1\n
 *   在Capture Mode时用于Capture计数器的值。\n
 *    (注：PWM通过P05输出，无强输出)

 * \section timerD  timer3(16位) 介绍
 * - T3CON：可读可写\n
 *   Bit1~0：Timer0 Mode，00为Disable，01为Timer Mode，10为Capture Mode，11为PWM Mode\n
 *   Bit3~2：计时源选择，00为系统时钟，01为P22（上升沿），10为OSC，11为OSC32K\n
 *   Bit4：Capture中断允许，高有效\n
 *   Bit5：Timer中断允许，高有效\n
 *   Bit6：Capture Pending，Capture事件发生，产生中断\n
 *   Bit7：Timer Pending，Timer计数结束产生的Pending
 * - T3SCA：只写\n
 *   Bit2~0：Timer Prescaler设置寄存器：000：无Prescale，001：1Bit Prescale，…… \n
 *   Bit3：Reserved\n
 *   Bit5~4：Capture源选择，00：VPP上升沿，01：IRDA上升沿，10：VPP下降沿，11：IRDA下降沿
 * - T3CNTL、T3CNTH：可读可写\n
 *   Timer计数器
 * - T3PRDL、T3PRDH：只写\n
 *   通过T3PRD控制，先写高位再写低位\n
 *   Timer计数上限
 * - T3PWML、T3PWMH：可读可写\n
 *   通过T3PWM控制，写操作时先写高位再写低位，读操作时先读高位再读低位\n
 *   在PWM Mode时用于控制PWM的占空比。T2PWM的值要小于T2PRD，否则PWM输出一直都是1\n
 *   在Capture Mode时用于Capture计数器的值。\n
 *    (注：PWM通过P34输出，有强输出可用)


 */