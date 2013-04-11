/*! \page4 ADC ���ָ��
 *
 * \section adc ADC ����
 * - AC209N��һ��10bit�� ADC
 * - ADCCON\n
 *   Bit2~0 : ͨ��ѡ��.\n
 *   000    : ѡ��LDO��ѹԴ(1.2V)\n
 *   001    : ѡ��LDOIN��1/2\n
 *   010~111: ѡ��P02~P07\n
 *   \n
 *   Bit3   : ADC Enable\n
 *   Ӧ�ñ�֤ADC�Ĳ����ʹ�����1MHZ\n
 *   Bit5~4 : ������ѡ��\n
 *   00     : ϵͳʱ�ӳ�2\n
 *   01     : ϵͳʱ�ӳ�16\n
 *   10     : ϵͳʱ�ӳ�24\n
 *   11     : ϵͳʱ�ӳ�48\n
 *   \n
 *   Bit7   : ADCGO��д1����ADC��ADCת�������Զ����㡣\n
 *
 * - ADCDATH\n
 *   bit7~0 : ���ADC��������ĸ� 8 bit
 * - ADCDATL\n
 *   bit7~6 : ���ADC��������ĵ� 2 bit
 */