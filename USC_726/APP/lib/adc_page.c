/*! \page4 ADC 编程指南
 *
 * \section adc ADC 介绍
 * - AC209N有一个10bit的 ADC
 * - ADCCON\n
 *   Bit2~0 : 通道选择.\n
 *   000    : 选择LDO电压源(1.2V)\n
 *   001    : 选择LDOIN的1/2\n
 *   010~111: 选择P02~P07\n
 *   \n
 *   Bit3   : ADC Enable\n
 *   应该保证ADC的波特率工作在1MHZ\n
 *   Bit5~4 : 波特率选择，\n
 *   00     : 系统时钟除2\n
 *   01     : 系统时钟除16\n
 *   10     : 系统时钟除24\n
 *   11     : 系统时钟除48\n
 *   \n
 *   Bit7   : ADCGO，写1启动ADC。ADC转换完则自动清零。\n
 *
 * - ADCDATH\n
 *   bit7~0 : 存放ADC采样结果的高 8 bit
 * - ADCDATL\n
 *   bit7~6 : 存放ADC采样结果的低 2 bit
 */