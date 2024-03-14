///	@file       comms/tcpip/Ping.h
///	@class      Ping
///	@brief      A mininalist ping (ICMP) implementation.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE

#ifndef INC_STM32_TOOLBOX_COMMS_ETHERNET_PING_H_
#define INC_STM32_TOOLBOX_COMMS_ETHERNET_PING_H_

#include <stdint.h>
#include "utility/Timer.h"
#include "comms/ethernet/w5500/Socket.h"


class Ping
{
public:

	typedef struct {
		uint8_t type = 8;
		uint8_t code = 0;
		uint16_t checksum = 0;
		uint16_t identifier = 0xbeef;
		uint16_t sequence;
	} IcmpPacket;

	Ping(Socket *socket)
	{
		this->socket = socket;
	}


//	void SetPayload(uint8_t* payload, uint16_t length)
//	{
//		this->payload = payload;
//		this->payload_length = length;
//	}


//	void SetID(uint8_t id)
//	{
//		this->id = id;
//	}

	int32_t Echo(IPv4Address ip)
	{
		// Avoid false result from integer wrap.
		if (serial == 0)
			serial = 1;

		IcmpPacket request;
		request.sequence = serial++;

		// Calculate checksum.
		request.checksum = CalculateChecksum(&request, sizeof(IcmpPacket));

		socket->open(SnMR::IPRAW, 1);  // 1 == ICMP
		socket->sendto(&request, sizeof(IcmpPacket), ip.raw_address(), 0);

		Timer t;
		t.start(milliseconds(timeout));
		IcmpPacket response = {0};
		uint8_t sender[4];
		while (request.sequence != response.sequence && !t.is_elapsed())
		{
			socket->recvfrom(&response, sizeof(IcmpPacket), sender, 0);
			osDelay(1);
		}
		socket->close();
		return t.is_elapsed() ? -1 : t.elapsed();
	}

private:
	void SwapBytes(void* target, uint16_t length)
	{
		uint8_t* p = (uint8_t*) target;
		assert (length % 2 == 0);
		for (int i=0; i< length; i+=2)
		{
			uint8_t buf[2];
			memcpy(buf, p+i, 2);
			*(p+i) = buf[1];
			*(p+i+1) = buf[0];
		}
	}

	uint16_t CalculateChecksum(IcmpPacket* request, uint16_t length)
	{
		uint16_t sum = 0;
		uint16_t* p = (uint16_t*) request;
		for (uint16_t i=0; i < length/2; i++, p++)
			sum += *p;

		// One's compliment.
		sum = (sum >> 16) + (sum & 0xffff);
		sum += (sum >> 16);
		return ~sum;
	}

	static inline uint16_t serial = 1;
	Socket* socket;
	uint32_t timeout = 2000;
};


#endif /* INC_STM32_TOOLBOX_COMMS_ETHERNET_PING_H_ */
