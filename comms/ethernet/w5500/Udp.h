/*
 *  Udp.cpp: Library to send/receive UDP packets with the Arduino ethernet shield.
 *  This version only offers minimal wrapping of socket.c/socket.h
 *  Drop Udp.h/.cpp into the Ethernet library directory at hardware/libraries/Ethernet/ 
 *
 * NOTE: UDP is fast, but has some important limitations (thanks to Warren Gray for mentioning these)
 * 1) UDP does not guarantee the order in which assembled UDP packets are received. This
 * might not happen often in practice, but in larger network topologies, a UDP
 * packet can be received out of sequence. 
 * 2) UDP does not guard against lost packets - so packets *can* disappear without the sender being
 * aware of it. Again, this may not be a concern in practice on small local networks.
 * For more information, see http://www.cafeaulait.org/course/week12/35.html
 *
 * MIT License:
 * Copyright (c) 2008 Bjoern Hartmann
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * bjoern@cs.stanford.edu 12/30/2008
 */

#ifndef ETHERNETUDP_H
#define ETHERNETUDP_H


#include <comms/tcpip/IPv4Address.h>
#include "Socket.h"
#include "comms/tcpip/IUdp.h"

#ifndef word
#define word(a, b) ( (uint16_t)((a)<<8) | (b) )
#endif


class Udp : public IUdp
{
public:
	Udp(Socket* socket)
	{
		this->socket = socket;
	}

	virtual bool begin(IPv4Address& ip, uint16_t port) override
	{
		_remaining = 0;
		this->ip = ip;
		this->port = port;
		if (!socket->open(SnMR::UDP, port, 0))
			return false;
		return true;
	}

	/* return number of bytes available in the current packet,
	   will return zero if parsePacket hasn't been called yet */
	virtual int available() override
	{
		return _remaining;
	}

	/* Release any resources being used by this EthernetUDP instance */
	virtual void stop() override
	{
		socket->close();
	}

	using PrintLite::write;

	virtual void beginPacket(void) override
	{
//		_offset = 0;
		socket->start_udp(ip, port);
	}

	virtual bool endPacket() override
	{
		return socket->send_udp();
	}


	virtual size_t write(uint8_t byte) override
	{
		return write(&byte, 1);
	}

	virtual size_t write(const uint8_t *buffer, size_t size) override
	{
		uint16_t bytes_written = socket->bufferData(buffer, size);
//		_offset += bytes_written;
		return bytes_written;
	}

	virtual int parsePacket() override
	{
		// discard any remaining bytes in the last packet
//		flush();

		if (socket->available() > 0)
		{
			//HACK - hand-parse the UDP packet using TCP recv method
			uint8_t tmpBuf[8];
			int ret =0;
			//read 8 header bytes and get IP and port from it
			ret = socket->recv(tmpBuf,8);
			if (ret > 0)
			{
				_remoteIP = tmpBuf;
				_remotePort = word(tmpBuf[4], tmpBuf[5]);
				_remaining = word(tmpBuf[6], tmpBuf[7]);

				// When we get here, any remaining bytes are the data
				ret = _remaining;
			}
			return ret;
		}
		// There aren't any packets available
		return 0;
	}

	virtual int read() override
	{
		uint8_t byte;

		if ((_remaining > 0) && (socket->recv(&byte, 1) > 0))
		{
			// We read things without any problems
			_remaining--;
			return byte;
		}

		// If we get here, there's no data available
		return -1;
	}

	virtual int read(uint8_t* buffer, size_t len) override
	{

		if (_remaining > 0)
		{
			int got;

			if (_remaining <= len)
			{
				// data should fit in the buffer
				got = socket->recv( buffer, _remaining);
			}
			else
			{
				// too much data for the buffer,
				// grab as much as will fit
				got = socket->recv( buffer, len);
			}

			if (got > 0)
			{
				_remaining -= got;
				return got;
			}
		}

		// If we get here, there's no data available or recv failed
		return -1;
	}

	virtual int peek() override
	{
		uint8_t b;
		// Unlike recv, peek doesn't check to see if there's any data available, so we must.
		// If the user hasn't called parsePacket yet then return nothing otherwise they
		// may get the Udp header
		if (!_remaining)
			return -1;
		socket->peek(&b);
		return b;
	}

	virtual void flush() override
	{
		// could this fail (loop endlessly) if _remaining > 0 and recv in read fails?
		// should only occur if recv fails after telling us the data is there, lets
		// hope the w5100 always behaves :)

		while (_remaining)
		{
			read();
		}
	}

//	/* Start EthernetUDP socket, listening at local port PORT */
//	virtual uint8_t beginMulticast(IPAddress ip, uint16_t port)
//	{
//		// Calculate MAC address from Multicast IP Address
//		uint8_t mac[] = {  0x01, 0x00, 0x5E, 0x00, 0x00, 0x00 };
//
//		mac[3] = ip[1] & 0x7F;
//		mac[4] = ip[2];
//		mac[5] = ip[3];
//
//		socket->set(mac, ip.raw_address(), port);
//		//W5100.writeSnDIPR(_sock, rawIPAddress(ip));
//		//W5100.writeSnDPORT(_sock, port);
//		//W5100.writeSnDHAR(_sock,mac);
//
//		_remaining = 0;
//		socket->open(SnMR::UDP, port, SnMR::MULTI);
//		return 1;
//	}


	// Return the IP address of the host who sent the current incoming packet
	virtual IPv4Address remoteIP() { return _remoteIP; };
	// Return the port of the host who sent the current incoming packet
	virtual uint16_t remotePort() { return _remotePort; };

private:
private:
	IPv4Address ip;
	uint16_t port; // local port to listen on
	IPv4Address _remoteIP; // remote IP address for the incoming packet whilst it's being processed
	uint16_t _remotePort; // remote port for the incoming packet whilst it's being processed
//	uint16_t _offset; // offset into the packet being sent
	uint16_t _remaining; // remaining bytes of incoming packet yet to be processed
	Socket* socket;
};

#endif
