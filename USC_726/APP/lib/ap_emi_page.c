/*! \page3 EMI 编程指南
 *
 * \section intro_sec_emi EMI介绍
 *
 * \section emi0 EMI用到的Pin：
 * - Port1为EMI的Data
 * - P33为EMI的写信号
 *
 * \section emi1 EMI 寄存器介绍
 * - EMICON：（可读可写，Reset值为0x00）\n
 *  Bit7：EMI Enable\n
 *  Bit6~4：EMI 写信号脉冲宽度，0代表1个时钟周期，最多是8个时钟周期。\n
 *  Bit3~2：EMI 写信号建立时间，0代表0个时钟周期，最多为3个时钟周期。\n
 *  Bit1~0：EMI写信号保持时间，0代表1个时钟周期，最多为4个时钟周期。\n
 *
 * - SPIDMACON:\n
 *  Bit6：EMI IDLE，高有效，这位为1时，表示EMI空闲，可启动发送。（只读，reset值为1）\n
 *
 * - EMIBUF：只能写 \n
 *  写一次，启动一次发送。
 *
 */