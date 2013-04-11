/*! \mainpage AC209N 开发包编程指南
 *
 * \section intro_sec_main 总体介绍
 *
 * \version 0.04
 *
 * \date 2011-3-7
 *
 * \section sec_descriptor 主芯片介绍
 * - 8051兼容指令集
 * - 最大工作频率：48M
 * - 256 byte DRAM\n
 *   9K byte XRAM\n
 *   22K byte OTP for APP
 * - 两级可嵌套中断优先级
 * - TIMER,UART,IIC,SPI,EMI,ADC(10bit),DAC(24bit)
 * - SDMMC 控制器
 * - USB OTG full speed 控制器
 * - aDSP模块用于音频处理\n
 *   支持MP3(layer1,2,3)格式\n
 *   支持CBR/VBR,最大编码率320kbps\n
 *   支持ID3V2\n
 *   支持ima/microsoft ADPCM格式\n
 *   支持PCM格式\n
 *   支持采样率(K):8/11.025/12/16/22.05/24/32/44.1/48\n
 *   支持音效模式(EQ)
 *   支持快进快退
 * - 支持USB Speaker/ mass storage
 * - 支持片内RTC
 * \section sec_descriptr 开发包介绍
 * - 目录划分
 *   -# lib: 项目库，主要包含解码控制接口
 *   -# app: 常用的控制模块，包含按键检测，设备管理，文件管理等
 *   -# music_play：音频解码模式下的流程控制，消息处理
 *   -# fm_receiver：FM收音模式下的流程控制，消息处理
 * \section new_change 新的修改：
 * - 将 USB 主从实时检测改为开机检测；在非USB从机模式下工作不做主从机检测，只做USB mass storage检测,提高USB兼容性\n
 * - 采样LDO电压源(1.2V)和LDOIN的1/2的值，运算出LDOIN的电量(实际电量*10)\n
 * - 将AUX和FM分开\n
 * - 修复了一些BUG\n
 * - 增加USB从机模式下，通讯状态提示功能\n
 * - 更改红外遥控的IO位置后，相应修改软件\n
 * - 增加淡入淡出的选择功能\n
 * - 增加了电源管理功能\n
 * - 增加了RTC功能\n
 */