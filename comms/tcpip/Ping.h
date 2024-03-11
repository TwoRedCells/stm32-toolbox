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
		uint16_t identifier;
		uint16_t sequence;
		uint8_t* payload;
	} IcmpPacket;

	Ping(Socket *socket)
	{
		this->id = id;
	}


	void SetPayload(uint8_t* payload, uint16_t length)
	{
		this->payload = payload;
		this->payload_length = length;
	}


	void SetID(uint8_t id)
	{
		this->id = id;
	}

private:
	int32_t Echo(IPv4Address ip)
	{
		IcmpPacket request {
			.identifier = id,
			.sequence = serial++,
			.payload = this->payload
		};

		// Calculate checksum.
		uint16_t packet_length = payload_length + 8;
		request.checksum = CalculateChecksum(&request, packet_length);

		socket->open(SnMR::IPRAW, 0, 0);
		socket->sendto(&request, packet_length, ip.raw_address(), 0);

		Timer t;
		t.start(milliseconds(timeout));
		IcmpPacket response;
		while (socket->recvfrom(&response, payload_length, ip.raw_address(), 0) != payload_length && !t.is_elapsed())
			osDelay(1);
		socket->close();
		return t.is_elapsed() ? -1 : t.elapsed();
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




	uint16_t id = 0xbeef;
	static inline uint16_t serial = 0;
	Socket* socket;
	uint8_t* payload = (uint8_t*) DefaultPayload;
	uint16_t payload_length = 72;
	static const constexpr char* DefaultPayload = "Everything under the sun is in tune; but the sun is eclipsed by the moon";
	uint32_t timeout = 2000;
};


#endif /* INC_STM32_TOOLBOX_COMMS_ETHERNET_PING_H_ */
