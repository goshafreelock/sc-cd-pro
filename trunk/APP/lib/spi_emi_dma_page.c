/*! \page dma SPI/EMI DMA 编程指南
 *
 * \section dma0 SPI&EMI DMA 介绍
 * - SPI & EMI DMA是一个双向DMA通道，可用于SPI或EMI的DMA操作，但两个外设不能同时DMA。
 *
 * \section dma1 SPI&EMI DMA 寄存器 介绍
 *
 * - SPIDMACON：（可写可读）\n
 *   Bit1：DMA模块选择，0为SPI，1为EMI，Reset值为0\n
 *   Bit6：EMI IDLE，高有效，这位为1时，表示EMI空闲，可启动发送。（只读，reset值为1）\n
 *   Bit7：DMA Pending，高有效，Reset值为0，pending产生后软件可以清0（软件不能置1）。\n
 *
 * - SPIDMAADRL、SPIDMAADRH：SPI DMA指针，其中ADRL只有高7位有效，ADRH只有低5位有效。其他位被忽略。\n
 *   这两个寄存器只能写，不能读。\n
 *   SPI DMA指针指向一个Word（16位），而不是Byte地址。\n
 *
 * - SPIDMACNT：（只能写不能读）\n
 *   DMA Counter，当写入该寄存器会启动一次DMA，DMA的数目为2×SPIDMACNT的值，如写入10，则实际DMA 20个Byte。有一种特殊情况，当对SPIDMACNT写入0，则会DMA 512 Byte。\n
 *
 * \section dma2 SPI DMA的方向由SPI方向寄存器控制，EMI DMA只能是输出。
 * \section dma3 中断向量：11


 */