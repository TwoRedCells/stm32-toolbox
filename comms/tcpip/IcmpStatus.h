/*
 * IcmpStatus.h
 *
 *  Created on: Oct 5, 2023
 *      Author: YvanRodriguez
 */

#ifndef LIB_STM32_TOOLBOX_COMMS_ETHERNET_ICMPSTATUS_H_
#define LIB_STM32_TOOLBOX_COMMS_ETHERNET_ICMPSTATUS_H_


#define REQ_DATASIZE 64
#define ICMP_ECHOREPLY 0
#define ICMP_ECHOREQ 8
#define ICMP_ECHOREP 0
#define TIME_EXCEEDED 11
#define PING_TIMEOUT 1000


	/*
	Indicates whether a ping succeeded or failed due to one of various error
	conditions. These correspond to error conditions that occur in this
	library, not anything defined in the ICMP protocol.
	 */
typedef enum
{
	Success = 0,
	SendTimeout = 1, // Timed out sending the request
	NoResponse = 2, // Died waiting for a response
	BadResponse = 3, // we got back the wrong type
	AsyncSent = 4
} IcmpStatus;

typedef unsigned long icmp_time_t;


#endif /* LIB_STM32_TOOLBOX_COMMS_ETHERNET_ICMPSTATUS_H_ */
