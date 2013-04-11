/*! \page1 TIMER ���ָ��
 *
 * \section intro_sec_timer TIMER ����
 * - AC209N һ����4��TIMER������TIMER0��TIMER1��8λ��TEMER;TIMER2��TIMER3��16λ��TIMER��
 * \section timera timer0(8λ) ����
 * - T0CON���ɶ���д\n
 *   Bit1~0��Timer0 Mode��00ΪDisable��01ΪTimer Mode��10ΪCapture Mode��11ΪPWM Mode\n
 *   Bit3~2����ʱԴѡ��00Ϊϵͳʱ�ӣ�01ΪP03�������أ���10ΪOSC��11ΪOSC32K\n
 *   Bit4��Capture�ж���������Ч\n
 *   Bit5��Timer�ж���������Ч\n
 *   Bit6��Capture Pending��Capture�¼������������ж�\n
 *   Bit7��Timer Pending��Timer��������������Pending
 * - T0SCA��ֻд\n
 *   Bit3~0��Timer Prescaler���üĴ���(1/(2^n))��0000����Prescale��0001��1Bit Prescale������\n
 *   Bit5~4��CaptureԴѡ��00��P02�����أ�01��IRDA�����أ�10��P02�½��أ�11��IRDA�½���\n
 *   Bit6��WatchDogEN��0����ֹ�����ܣ�1������Timer0���ʱ��λоƬ��
 * - T0CNT���ɶ���д\n
 *   Timer������
 * - T0PRD��ֻд\n
 *   Timer��������
 * - T0PWM��ֻд\n
 *   ��PWM Modeʱ���ڿ���PWM��ռ�ձȡ�T0PWM��ֵҪС��T0PRD������PWM���һֱ����1\n
 *   ��Capture Modeʱ����Capture��������ֵ��\n
*    (ע��PWMͨ��P30�������ǿ�������)

 * \section timerB timer1(8λ) ����
 * - T1CON���ɶ���д\n
 *   Bit1~0��Timer1 Mode��00ΪDisable��01ΪTimer Mode��10ΪCapture Mode��11ΪPWM Mode\n
 *   Bit3~2����ʱԴѡ��00Ϊϵͳʱ�ӣ�01ΪP05�������أ���10ΪOSC��11ΪOSC32K\n
 *   Bit4��Capture�ж���������Ч\n
 *   Bit5��Timer�ж���������Ч\n
 *   Bit6��Capture Pending��Capture�¼������������ж�\n
 *   Bit7��Timer Pending��Timer��������������Pending
 * - T1SCA��ֻд\n
 *   Bit3~0��Timer Prescaler���üĴ���(1/(2^n))��0000����Prescale��0001��1Bit Prescale������\n
 *   Bit5~4��CaptureԴѡ��00��P04�����أ�01��IRDA�����أ�10��P04�½��أ�11��IRDA�½���\n
 *   Bit6��WatchDogEN��0����ֹ�����ܣ�1������Timer0���ʱ��λоƬ��
 * - T1CNT���ɶ���д\n
 *   Timer������
 * - T1PRD��ֻд\n
 *   Timer��������
 * - T1PWM��ֻд\n
 *   ��PWM Modeʱ���ڿ���PWM��ռ�ձȡ�T0PWM��ֵҪС��T0PRD������PWM���һֱ����1\n
 *   ��Capture Modeʱ����Capture��������ֵ��\n
 *    (ע��PWMͨ��P03�������ǿ���)

 * \section timerC  timer2(16λ) ����
 * - T2CON���ɶ���д\n
 *   Bit1~0��Timer0 Mode��00ΪDisable��01ΪTimer Mode��10ΪCapture Mode��11ΪPWM Mode\n
 *   Bit3~2����ʱԴѡ��00Ϊϵͳʱ�ӣ�01ΪP13�������أ���10ΪOSC��11ΪOSC32K\n
 *   Bit4��Capture�ж���������Ч\n
 *   Bit5��Timer�ж���������Ч\n
 *   Bit6��Capture Pending��Capture�¼������������ж�\n
 *   Bit7��Timer Pending��Timer��������������Pending
 * - T2SCA��ֻд\n
 *   Bit2~0��Timer Prescaler���üĴ�����000����Prescale��001��1Bit Prescale������ \n
 *   Bit3��Reserved\n
 *   Bit5~4��CaptureԴѡ��00��P23�����أ�01��IRDA�����أ�10��P23�½��أ�11��IRDA�½���
 * - T2CNTL��T2CNTH���ɶ���д\n
 *   Timer������
 * - T2PRDL��T2PRDH��ֻд\n
 *   ͨ��T2PRD���ƣ���д��λ��д��λ\n
 *   Timer��������
 * - T2PWML��T2PWMH���ɶ���д\n
 *   ͨ��T2PWM���ƣ�д����ʱ��д��λ��д��λ��������ʱ�ȶ���λ�ٶ���λ\n
 *   ��PWM Modeʱ���ڿ���PWM��ռ�ձȡ�T2PWM��ֵҪС��T2PRD������PWM���һֱ����1\n
 *   ��Capture Modeʱ����Capture��������ֵ��\n
 *    (ע��PWMͨ��P05�������ǿ���)

 * \section timerD  timer3(16λ) ����
 * - T3CON���ɶ���д\n
 *   Bit1~0��Timer0 Mode��00ΪDisable��01ΪTimer Mode��10ΪCapture Mode��11ΪPWM Mode\n
 *   Bit3~2����ʱԴѡ��00Ϊϵͳʱ�ӣ�01ΪP22�������أ���10ΪOSC��11ΪOSC32K\n
 *   Bit4��Capture�ж���������Ч\n
 *   Bit5��Timer�ж���������Ч\n
 *   Bit6��Capture Pending��Capture�¼������������ж�\n
 *   Bit7��Timer Pending��Timer��������������Pending
 * - T3SCA��ֻд\n
 *   Bit2~0��Timer Prescaler���üĴ�����000����Prescale��001��1Bit Prescale������ \n
 *   Bit3��Reserved\n
 *   Bit5~4��CaptureԴѡ��00��VPP�����أ�01��IRDA�����أ�10��VPP�½��أ�11��IRDA�½���
 * - T3CNTL��T3CNTH���ɶ���д\n
 *   Timer������
 * - T3PRDL��T3PRDH��ֻд\n
 *   ͨ��T3PRD���ƣ���д��λ��д��λ\n
 *   Timer��������
 * - T3PWML��T3PWMH���ɶ���д\n
 *   ͨ��T3PWM���ƣ�д����ʱ��д��λ��д��λ��������ʱ�ȶ���λ�ٶ���λ\n
 *   ��PWM Modeʱ���ڿ���PWM��ռ�ձȡ�T2PWM��ֵҪС��T2PRD������PWM���һֱ����1\n
 *   ��Capture Modeʱ����Capture��������ֵ��\n
 *    (ע��PWMͨ��P34�������ǿ�������)


 */