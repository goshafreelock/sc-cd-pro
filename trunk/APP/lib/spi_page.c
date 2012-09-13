/*! \page spi SPI 编程指南
 *
 * \section intro_sec_spi SPI 介绍
 *
 * \section spi0 SPI有两个出口：
 * - 出口1：CLK为P16、DO为P17、DI为P15
 * - 出口2：CLK为P30、DO为P31、DI为P32
 * - 当SPI被设置为两线模式时，DO同时为输入和输出。
 * \section spi1 SPI 寄存器：
 * - SPICON：（可写可读，Reset值为0x80）
 *   Bit0：SPI Enable，高有效，reset值为0\n
 *   Bit1：两线模式选择，0为三线模式，1为两线模式\n
 *   Bit2：SPI方向选择，0为输出，1为输入\n
 *   Bit3：SPI时钟边沿选择，0为下降沿，1为上升沿\n
 *   Bit4：SPI时钟空闲时状态，0为空闲时低电平，1为空闲时高电平\n
 *   Bit5：输入采样点选择，0为每一位在中间采样，1为每一位在结束采样。\n
 *   Bit6：SPI出口选择，0为出口1,  1为出口2\n
 *   Bit7：SPI Pending，高有效，软件可清零，但不能置1。\n
（注：在两线模式下，SPI是输入还是输出由方向选择寄存器控制。另外方向选择寄存器在DMA时，也作为DMA方向选择寄存器）\n
 *
 * - SPIBAUD：波特率寄存器，Reset值为x，用之前需要初始化
 *   波特率计算公式：波特率 = Freq_sys /(SPIBAUD+1)\n
 *
 * - SPIBUF：
 *   若在三线模式，写一次SPIBUF会启动一次收发操作，发出去的内容就是刚刚写入SPIBUF的内容，收到的内容则是DI输进来的内容。当Pending为1，收发完成，CPU可以读SPIBUF得到刚刚收到的数据。读完SPIBUF后，CPU要对Pending清零（也可以等下一次写SPIBUF清零）。\n
 *   若在两线模式，当方向寄存器设为输出，则写一次SPIBUF会启动一次发送操作，发出去的内容就是刚刚写入SPIBUF的内容，若方向寄存器设为输入，则写一次SPIBUF会启动一次接收操作，被写入SPIBUF的内容会被忽略。当Pending为1，收发完成，CPU可以读SPIBUF得到刚刚收到的数据。读完SPIBUF后，CPU要对Pending清零（也可以等下一次写SPIBUF清零）。\n
 *
 * \section spi2 SPI中断入口是4, 中断允许是IE0.4
 */