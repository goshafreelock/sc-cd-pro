/*! \page3 EMI ���ָ��
 *
 * \section intro_sec_emi EMI����
 *
 * \section emi0 EMI�õ���Pin��
 * - Port1ΪEMI��Data
 * - P33ΪEMI��д�ź�
 *
 * \section emi1 EMI �Ĵ�������
 * - EMICON�����ɶ���д��ResetֵΪ0x00��\n
 *  Bit7��EMI Enable\n
 *  Bit6~4��EMI д�ź������ȣ�0����1��ʱ�����ڣ������8��ʱ�����ڡ�\n
 *  Bit3~2��EMI д�źŽ���ʱ�䣬0����0��ʱ�����ڣ����Ϊ3��ʱ�����ڡ�\n
 *  Bit1~0��EMIд�źű���ʱ�䣬0����1��ʱ�����ڣ����Ϊ4��ʱ�����ڡ�\n
 *
 * - SPIDMACON:\n
 *  Bit6��EMI IDLE������Ч����λΪ1ʱ����ʾEMI���У����������͡���ֻ����resetֵΪ1��\n
 *
 * - EMIBUF��ֻ��д \n
 *  дһ�Σ�����һ�η��͡�
 *
 */