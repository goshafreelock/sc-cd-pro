/*! \page  PORT PORT 指南
 *
 * \section intro_sec_port PORT 总体介绍
 *
 * - AC209N 有P0、P1、P2、P3、P4
 *
 * \subsection  PORT 寄存器-介绍
 * - Port口输入、输出寄存器：P0、P1、P2、P3、P4\n
 *   Reset值为0
 *
 * - Port口方向寄存器：P0DIR、P1DIR、P2DIR、P3DIR、P4DIR\n
 *   Reset值为1
 *
 * - Port口上拉控制寄存器：P0PU、P1PU、P2PU、P3PU 、P4PU\n
 *   高有效，除P00和P01，以及整个Port4，其他reset值都为0；
 *
 * - Port口下拉控制寄存器：P0PD、P1PDA、P1PDB、P3PD\n
 *   高有效，Reset值为0；\n
 *   （注：只有Port0、Port1和Port3可以下拉，其中P0和P3下拉都是10K，P1则可下拉1K或500欧，P1PDA为1时下拉1K，P1PDB为1时下拉500欧，其中P1PDB优先）
 *
 * - Port口数字模拟选择寄存器：\n
 *   有些IO口可上有数字功能和模拟功能，如：ADC在P0口，当不用为ADC时，也可以当作数字IO使用\n
 *   P0IE、P2IE、DACCON0.6、DACCON0.7用于选择相应的IO口为何种功能，等于1时，选择为数字IO，等于0时，选择为模拟IO，复位后值为1\n
 *   以上2组寄存器都是可读可写\n
 *   其中P0IE控制整个P0口的工作模式\n
 *   P3IE控制P3的低6位，当P30～P35输出1/2时，最好将相应位设成模拟，否则会漏电。P3IE的Bit6，控制P23(reset = 1)，Bit7控制Headphone LDO Enable(高有效,reset=1)。\n
 *   DACCON0: dac control register 0\n
 *   DACCON0.7: 	P27/P26工作模式选择\n
 *   DACCON0.6: 	P25/P24工作模式选择\n
 *
 * - Port口高驱动控制寄存器：P3HD\n
 *   高有效，Reset值为0， 0对应8mA输出，1对应24mA输出。\n
 *   （注：只有Port3可以通过寄存器控制强输出（24mA），其余IO中，P4为强输出，其他为8mA输出）
 *
 * - 当方向寄存器为0，相应的IO为输出（输出打开），输出的值为输出寄存器被写入的值。这时如果读Port口，只能读到输出寄存器的值。
 *
 * - 当方向寄存器为1，相应的IO不输出，这时该IO口是否作为输入，由输入允许寄存器控制，当输入允许寄存器为1，则该IO输入允许，读Port口可读到相应管脚的值。否则，读Port口只能读到0。
 *
 * - 上拉控制寄存器为0时，或方向寄存器为0时，上拉禁止。否则相应IO会有10K上拉。
 *
 * - 对于前4个Port，当方向寄存器为1、输入允许寄存器为0，上拉寄存器为0，IO就不会漏电。
 * - 对于Port4，当方向寄存器为1、上拉寄存器为1，IO没有接外部信号，就不会漏电。
 *
 * - P36和P37实际上就是USBDP和USBDM，因此P3的各个寄存器的Bit6和Bit7和其他寄存器作用有所不同。\n
 *   P3HD（Bit7/6）：中断banksel，选择中断入口地址\n
 *   P3PU（Bit6）：P36上拉1.5K控制位，高有效 \n
 *   P3PU（Bit7）：P37上拉150K控制位，高有效 \n
 *   P3PD（Bit6）：P36下拉15K控制位，高有效  \n
 *   P3PD（Bit7）：P37下拉15K控制位，高有效  \n
 *   P3（Bit6）、P3（Bit7）、P3DIR（Bit6）、P3DIR（Bit7）功能和其他pin的相应寄存器一样（当USBIO_EN = 1的时候，USBIO_EN在USBCON0的Bit0）。\n
 *
 * - 注：为使遥控接收头和接收管都达到最佳性能，AC209N_48PIN封装片原PIN4（P23）将只支持红外接收管，不支持红外接收头，红外接收头需接PIN13（VPP）。后续AC209N_48PIN 封装芯片PIN4 也将只支持红外接收管，红外接收头需接VPP 脚，另外PIN4（P23）也不能当做普通IO 使用。请大家及时修改原理图和PCB，给大家带来的不便之处，敬请谅解！\n
 *
 * \subsection port1 PORT WakeUp介绍
 * - 支持WakeUp的脚共有以下四条：P02、P05、P07，这三条pin对WakeUp电路来说分别为WakeUp Pin0~2
 * - WKUPCON:\n
 *   WKUPCON的低4位为WKUPEN（高有效），分别控制四条pin是否能产生唤醒（或中断）。\n
 *   Reset值为0
 * - WKUPCON的高4位为WKUPEDG，分别选择四条pin唤醒的边沿，0为上升沿，1为下降沿    \n
 *   Reset值为0
 * - WKUPPND\n
 *   WKUPPND的低4位为四条Pin的Pending信号，0为没有Pending，1为有Pending。\n
 *   WKUPPND可以被CPU写0或1\n
 *   Reset值为X，使用前需初始化\n
 * - 以上的寄存器都是可读可写
 * - 当芯片处于正常工作状态，有效边沿出现，则芯片会产生中断（中断允许要打开，否则只会有Pending，而没有中断），Port中断的中断向量为11。
 * - 当芯片处于Stop Clock时，有效边沿出现，则芯片会被唤醒，若stop clock前中断允许是打开的，则被唤醒的CPU会马上进入中断，否则会执行stop clock后一条指令。
 * - 当芯片处于Sleep时，有效边沿出现，则芯片会被Reset。芯片重新起来后可以读PD（power down）寄存器，PD寄存器为1，说明是Sleep后唤醒。
 * - VPP\n
 *   VPP在烧写时，提供烧写电压给OTP\n
 *   VPP也可以当作普通的输入口来使用\n
 *   输入寄存器：WKUPPND.7\n
 *   10K上拉电阻使能：WKUPPND.6\n
 *
 * - USB口可以当作普通IO使用：
 *   芯片复位后调用disable_usb()\n
 *   任何时候不可以调用以下函数：udisk_disconnect_check(),usb_diskin_detect();usb_pcin_detect();device_check();init_usb_device_hardware();UFI_massstorage();等等会操作USB的函数，也不能使用USB功能\n
 *   USB当普通IO口使用时，DP为P36，有1.5K上拉，DM为P37，有120K上拉\n
 *   \image html port.bmp
 */





