///	@file       comms/ethernet/NtpClient.h
///	@class      NtpClient
///	@brief      A mininalist NTP client that focuses on simplicity over accuracy.
/// @note       See the DS18B20 datasheet for details how this class is implemented.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef LIB_STM32_TOOLBOX_COMMS_ETHERNET_NTPCLIENT_H_
#define LIB_STM32_TOOLBOX_COMMS_ETHERNET_NTPCLIENT_H_

#include <stdint.h>
#include "comms/ethernet/w5500/Udp.h"
#include "comms/tcpip/IpAddress.h"

class NtpClient
{
public:
	static constexpr uint16_t ntp_port = 123;
	static constexpr uint16_t ntp_packet_size = 48;

	typedef struct {
		uint8_t li_vn_mode;				// Leap indicator, version number, mode.
		uint8_t stratum;				// Stratum level of the clock.
		uint8_t poll;					// Maximum interval between successive requests.
		uint8_t precision;				// Precision of the clock.
		uint32_t root_delay;			// Round trip delay.
		uint32_t root_dispersion;		// Maximum error allowed.
		uint32_t ref_id;				// Reference identifier.
		uint64_t ref_timestamp;			// Reference timestamp.
		uint64_t orig_timestamp;		// Originate timestamp.
		uint64_t recv_timestamp;		// Receive timestamp.
		uint64_t transmit_timestamp;	// Transit timestamp.
	} NtpPacket;


	NtpClient(Socket* socket, const uint8_t* ip)
	{
		this->ip = ip;
		this->socket = socket;
	}


	/**
	 * @brief 	Gets the number of seconds since January 1, 1900 from the NTP server.
	 * @param timestamp	A 64-bit integer in which the upper 32 bits are the number of seconds since the NTP epoch and the lower 32 bits are the fractional seconds.
	 */
	bool get_ntp_timestamp(uint64_t& timestamp)
	{
		NtpPacket packet = {0};
		packet.li_vn_mode |= 0b00100011; // SNTPv4, unicast client.
		Udp udp(socket);
		IPAddress ip(this->ip);

		udp.begin(ntp_port);
		if (!udp.beginPacket(ip, ntp_port))
			return false;

		udp.write((uint8_t*)&packet, sizeof(NtpPacket));
		udp.endPacket();
		int len = udp.parsePacket();
		if (len != sizeof(NtpPacket))
			return false;

		if (udp.read((uint8_t*)&packet, sizeof(NtpPacket)) != sizeof(NtpPacket))
			return false;

		// Reverse byte order.
		timestamp = 0;
		for (int i=0; i<8; i++)
			timestamp |= (packet.transmit_timestamp >> (8*i) & 0xff) << (8*(7-i));
		return true;
	}


	/**
	 * @brief 	Gets the number of seconds since January 1, 1970 from the NTP server.
	 * @param timestamp	A 32-bit integer representing the number of seconds since the UNIX epoch.
	 */
	bool get_unix_timestamp(uint32_t& timestamp)
	{
		uint64_t stamp;
		if (!get_ntp_timestamp(stamp))
			return false;

		timestamp = (uint32_t) ((stamp >> 32) - 2208988800ul);
		return true;
	}

private:
	const uint8_t* ip;
	Socket* socket;
};

#endif /* LIB_STM32_TOOLBOX_COMMS_ETHERNET_NTPCLIENT_H_ */
