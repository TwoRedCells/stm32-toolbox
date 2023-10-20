#ifndef LIB_STM32_TOOLBOX_COMMS_ETHERNET_ICMPECHO_H_
#define LIB_STM32_TOOLBOX_COMMS_ETHERNET_ICMPECHO_H_

#include "IcmpHeader.h"
#include "IcmpStatus.h"

struct IcmpEcho
{
	/*
    Contents of an ICMP echo packet, including the ICMP header. Does not
    include the IP header.
	 */


	/*
    This constructor sets all fields and calculates the checksum. It is used
    to create ICMP packet data when we send a request.
    @param type: ICMP_ECHOREQ or ICMP_ECHOREP.
    @param _id: Some arbitrary id. Usually set once per process.
    @param _seq: The sequence number. Usually started at zero and incremented
    once per request.
    @param payload: An arbitrary chunk of data that we expect to get back in
    the response.
	 */
	IcmpEcho(uint8_t type, uint16_t _id, uint16_t _seq, uint8_t * _payload)
	: seq(_seq), id(_id), time(millis())
	{
		memcpy(payload, _payload, REQ_DATASIZE);
		icmpHeader.type = type;
		icmpHeader.code = 0;
		icmpHeader.checksum = _checksum(*this);
	}

	/*
    This constructor leaves everything zero. This is used when we receive a
    response, since we nuke whatever is here already when we copy the packet
    data out of the W5100.
	 */
	IcmpEcho()
	: seq(0), id(0), time(0)
	{
		memset(payload, 0, sizeof(payload));
		icmpHeader.code = 0;
		icmpHeader.type = 0;
		icmpHeader.checksum = 0;
	}

	/*
    Serialize the header as a byte array, in big endian format.
	 */

	IcmpHeader icmpHeader;
	uint16_t seq;
	uint16_t id;
	icmp_time_t time;
	uint8_t payload [REQ_DATASIZE];


	/*
    Serialize the header as a byte array, in big endian format.
	 */
	void serialize(uint8_t * binData) const
	{
		*(binData++) = icmpHeader.type;
		*(binData++) = icmpHeader.code;

		*(uint16_t *)binData = htons(icmpHeader.checksum); binData += 2;
		*(uint16_t *)binData = htons(id);                  binData += 2;
		*(uint16_t *)binData = htons(seq);                 binData += 2;
		*(icmp_time_t *)  binData = htonl(time);           binData += 4;

		memcpy(binData, payload, sizeof(payload));
	}

	/*
    Serialize the header as a byte array, in big endian format.
	 */
	void deserialize(uint8_t const * binData)
	{
		icmpHeader.type = *(binData++);
		icmpHeader.code = *(binData++);

		icmpHeader.checksum = ntohs(*(uint16_t *)binData); binData += 2;
		id                  = ntohs(*(uint16_t *)binData); binData += 2;
		seq                 = ntohs(*(uint16_t *)binData); binData += 2;

		if (icmpHeader.type != TIME_EXCEEDED)
		{
			time = ntohl(*(icmp_time_t *)binData);   binData += 4;
		}

		memcpy(payload, binData, sizeof(payload));
	}

private:
	uint16_t _checksum(const IcmpEcho& echo)
	{
		// calculate the checksum of an ICMPEcho with all fields but icmpHeader.checksum populated
		unsigned long sum = 0;

		// add the header, bytes reversed since we're using little-endian arithmetic.
		sum += _makeUint16(echo.icmpHeader.type, echo.icmpHeader.code);

		// add id and sequence
		sum += echo.id + echo.seq;

		// add time, one half at a time.
		uint16_t const * time = (uint16_t const *)&echo.time;
		sum += *time + *(time + 1);

		// add the payload
		for (uint8_t const * b = echo.payload; b < echo.payload + sizeof(echo.payload); b+=2)
		{
			sum += _makeUint16(*b, *(b + 1));
		}

		// ones complement of ones complement sum
		sum = (sum >> 16) + (sum & 0xffff);
		sum += (sum >> 16);
		return ~sum;
	}

	inline uint16_t _makeUint16(const uint8_t& highOrder, const uint8_t& lowOrder)
	{
		// make a 16-bit unsigned integer given the low order and high order bytes.
		// lowOrder first because the Arduino is little endian.
		uint8_t value [] = {lowOrder, highOrder};
		return *(uint16_t *)&value;
	}
};

#endif
