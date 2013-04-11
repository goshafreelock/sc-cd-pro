/*! \page dma SPI/EMI DMA ���ָ��
 *
 * \section dma0 SPI&EMI DMA ����
 * - SPI & EMI DMA��һ��˫��DMAͨ����������SPI��EMI��DMA���������������費��ͬʱDMA��
 *
 * \section dma1 SPI&EMI DMA �Ĵ��� ����
 *
 * - SPIDMACON������д�ɶ���\n
 *   Bit1��DMAģ��ѡ��0ΪSPI��1ΪEMI��ResetֵΪ0\n
 *   Bit6��EMI IDLE������Ч����λΪ1ʱ����ʾEMI���У����������͡���ֻ����resetֵΪ1��\n
 *   Bit7��DMA Pending������Ч��ResetֵΪ0��pending���������������0�����������1����\n
 *
 * - SPIDMAADRL��SPIDMAADRH��SPI DMAָ�룬����ADRLֻ�и�7λ��Ч��ADRHֻ�е�5λ��Ч������λ�����ԡ�\n
 *   �������Ĵ���ֻ��д�����ܶ���\n
 *   SPI DMAָ��ָ��һ��Word��16λ����������Byte��ַ��\n
 *
 * - SPIDMACNT����ֻ��д���ܶ���\n
 *   DMA Counter����д��üĴ���������һ��DMA��DMA����ĿΪ2��SPIDMACNT��ֵ����д��10����ʵ��DMA 20��Byte����һ���������������SPIDMACNTд��0�����DMA 512 Byte��\n
 *
 * \section dma2 SPI DMA�ķ�����SPI����Ĵ������ƣ�EMI DMAֻ���������
 * \section dma3 �ж�������11


 */