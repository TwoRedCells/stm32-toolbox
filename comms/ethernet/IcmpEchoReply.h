/*
 * IcmpEchoReply.h
 *
 *  Created on: Oct 5, 2023
 *      Author: YvanRodriguez
 */

#ifndef LIB_STM32_TOOLBOX_COMMS_ETHERNET_ICMPECHOREPLY_H_
#define LIB_STM32_TOOLBOX_COMMS_ETHERNET_ICMPECHOREPLY_H_

#include "IcmpStatus.h"


struct IcmpEchoReply
{
	/*
    Struct returned by ICMPPing().
    @param data: The packet data, including the ICMP header.
    @param ttl: Time to live
    @param status: SUCCESS if the ping succeeded. One of various error codes
    if it failed.
    @param addr: The ip address that we received the response from. Something
    is borked if this doesn't match the IP address we pinged.
	 */
	IcmpEcho data;
	uint8_t ttl;
	IcmpStatus status;
	IPAddress addr;
};




#endif /* LIB_STM32_TOOLBOX_COMMS_ETHERNET_ICMPECHOREPLY_H_ */
