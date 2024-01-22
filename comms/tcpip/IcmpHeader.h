/*
 * IcmpHeader.h
 *
 *  Created on: Oct 5, 2023
 *      Author: YvanRodriguez
 */

#ifndef LIB_STM32_TOOLBOX_COMMS_ETHERNET_ICMPHEADER_H_
#define LIB_STM32_TOOLBOX_COMMS_ETHERNET_ICMPHEADER_H_


struct IcmpHeader
{
	/*
    Header for an ICMP packet. Does not include the IP header.
	 */
	uint8_t type;
	uint8_t code;
	uint16_t checksum;
};




#endif /* LIB_STM32_TOOLBOX_COMMS_ETHERNET_ICMPHEADER_H_ */
