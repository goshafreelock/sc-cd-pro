/*! \page HB001_CPU CPU 编程指南
*
* - memory
*   -# XDATA 0x0 ~ 0x21FF\n
堆栈   0X300 ~ 0X37F(128B)\n
文件系统缓存  0X600 ~ 0X7FF(512B)\n
音频解码缓存  0X800 ~ 0XBFF(1024B)\n
DAC缓存       0X1A00 ~ 0X21FF(2048B)\n
APP:   0XC00 ~ 0X19FF\n
BSP:   0X380 ~ 0X5FF\n
*   -# DATA 0x0 ~ 0xFF
*   -# CODE 0x0 ~ 0x57FF for APP
*
* - interrupt
*   -# TIMER0         0
*   -# TIMER1         1
*   -# TIMER2         2
*   -# TIMER3         3
*   -# SPI            4
*   -# IIC            5
*   -# SOFT           6
*   -# UART           7
*   -# PORT           8
*   -# SPI_EMI DMA    11
*   -# LVD_RTC        15
*
* - clock system
* - CLKCON(reset:X0000001)\n
*   BIT0    :RC OSC enable = 1,RC OSC disable = 0\n
*   BIT1    :extern crystal osc enable = 1, extern crystal osc disable = 0\n
*   BIT2    :PLL enable = 1, PLL disable = 0\n
*   BIT3    :DSP clock en = 0, DSP clock disable = 1\n
*   BIT4    :PLL input is 32768 = 1, PLL input is not 32768 = 0\n
*   BIT5    :PLL input is 7.6M = 1, PLL input is 12M = 0\n
*   BIT6    :DAC clock enable = 0, DAC clock disable = 1\n
*   BIT7    :not used
*
* - PCON(reset:00000000)\n
*   BIT3~0  :DO NOT modify\n
*   BIT5~4  :\n
-system use RC clock:00,\n
-system use OSC clock(7.6M/12M):01,\n
-system use PLL output:10,\n
-system use OSC(32678):11.\n
-please use AND/OR/XOR to modify this register.\n
*   BIT7~6  :DO NOT modify\n
*/