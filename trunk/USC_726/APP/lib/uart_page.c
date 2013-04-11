/*! \page0 UART ���ָ��
 *
 * \section intro0_sec UART �������
 * - UART���ĸ�����\n
 *   ����1��TXΪP06��RXΪP07\n
 *   ����2��TXΪP24��RXΪP25\n
 *   ����3��TXΪP32��RXΪP33\n
 *   ����4��TXΪUSBDP��RXΪUSBDM
 * \section uart0  UART �Ĵ�������
 * - UTCON��UART���ƼĴ���\n
 *   Bit0��UART Enable������Ч��ResetֵΪ0\n
 *   Bit1��9bitģʽ������Ч��ResetֵΪ0\n
 *   Bit2��TX�ж���������Ч��ResetֵΪ0����TX PendingΪ1������TX�ж�����Ϊ1���������ж�\n
 *   Bit3��RX�ж���������Ч��ResetֵΪ0����RX PendingΪ1������RX�ж�����Ϊ1���������ж�\n
 *   Bit5~4�������ʿ��ƼĴ�����2λ��ResetֵΪ00 \n
 *   Bit7~6��UART����ѡ��Ĵ�����00ѡ����1��01ѡ����2��ResetֵΪ00\n
 *   ע�������жϵ�ַΪ��0x0033;
 * - UTSTA�� UART״̬�Ĵ���\n
 *   Bit0��9Bitģʽʱ��RX���յ��ĵ�9λ��ResetֵΪX\n
 *   Bit1��9Bitģʽʱ��TX���͵ĵ�9λ��ResetֵΪX\n
 *   Bit6��RX Pending��ResetֵΪ0����������㣬��������1��\n
 *   Bit7��TX Pending��ResetֵΪ1����������㣬��������1��
 * - UTBAUD�������ʼĴ�����8λ��ResetֵΪX����֮ǰҪ��ʼ��\n
 *   �����ʼ��㹫ʽ�������� = Freq_sys / ((UTBAUD+1) *8)\n
 *   ���Ϲ�ʽUTBAUDΪ10λ�Ĵ���
 * - UTBUF:\n
 *   ��UART���Ϳ���ʱ��дUTBUF������һ�η��͡����͵����ݾ���дUTBUF�����ݡ�\n
 *   ��RX PendingΪ1ʱ����UTBUF�ɶ������յ��ֽڡ�ע�⣬�����Ҫ�����RX Pending����������յ����ֽڲ�����UTBUF���ᶪʧ��
 */



