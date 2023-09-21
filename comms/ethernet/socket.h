#ifndef	_SOCKET_H_
#define	_SOCKET_H_

#include "w5500.h"
#include "IPAddress.h"

class Socket
{
public:
	Socket(W5500* w5500)
	{
		this->w5500 = w5500;
	}

	inline uint8_t readChipVersion(void) { return w5500->read_version(); }
	inline uint8_t status(SOCKET s) { return w5500->readSnSR(s); }
	inline int16_t recvAvailable(SOCKET s) { return w5500->get_rx_received_size(s); }
	inline void setMACAddress(uint8_t * mac) { w5500->set_mac_address(mac); }
	inline void setIPAddress(uint8_t * ip) { w5500->set_ip_address(ip); }
	inline void getIPAddress(uint8_t * ip) { w5500->get_ip_address(ip); }
	inline void setGatewayIp(uint8_t * ip) { w5500->set_gateway_ip(ip); }
	inline void getGatewayIp(uint8_t * ip) { w5500->get_gateway_ip(ip); }
	inline void setSubnetMask(uint8_t * sm) { w5500->set_subnet_mask(sm); }
	inline void getSubnetMask(uint8_t * sm) { w5500->get_subnet_mask(sm); }
	inline void getRemoteIP(SOCKET s, uint8_t * ip) { w5500->readSnDIPR(s, ip); }
	inline void disconnect(SOCKET s) { w5500->execute_command(s, Sock_DISCON); } // disconnect the connection
	inline void peek(SOCKET s, uint8_t *buf) { w5500->recv_data_processing(s, buf, 1, 1); }


	inline static uint16_t local_port = 0;
	inline static uint8_t sock_is_sending = 0;

	/**
	 * @brief	This Socket function initialize the channel in perticular mode, and set the port and wait for w5500 done it.
	 * @return 	1 for success else 0.
	 */
	uint8_t open(SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag)
	{
		sock_is_sending &= ~(1<<s);

		if ((protocol == SnMR::TCP) || (protocol == SnMR::UDP) || (protocol == SnMR::IPRAW) || (protocol == SnMR::MACRAW) || (protocol == SnMR::PPPOE))
		{
			close(s);
			w5500->writeSnMR(s, protocol | flag);
			if (port != 0) {
				w5500->writeSnPORT(s, port);
			}
			else {
				local_port++; // if don't set the source port, set local_port number.
				w5500->writeSnPORT(s, local_port);
			}

			w5500->execute_command(s, Sock_OPEN);

			return 1;
		}

		return 0;
	}


	/**
	 * @brief	This function close the socket and parameter is "s" which represent the socket number
	 */
	void close(SOCKET s)
	{
		sock_is_sending &= ~(1<<s);
		w5500->execute_command(s, Sock_CLOSE);
		w5500->writeSnIR(s, 0xFF);
	}


	/**
	 * @brief	This function established  the connection for the channel in passive (server) mode. This function waits for the request from the peer.
	 * @return	1 for success else 0.
	 */
	uint8_t listen(SOCKET s)
	{
		if (w5500->readSnSR(s) != SnSR::INIT)
			return 0;
		w5500->execute_command(s, Sock_LISTEN);
		return 1;
	}


	/**
	 * @brief	This function established  the connection for the channel in Active (client) mode.
	 * 		This function waits for the untill the connection is established.
	 *
	 * @return	1 for success else 0.
	 */
	uint8_t connect(SOCKET s, uint8_t * addr, uint16_t port)
	{
		if ( ( IPAddress(addr) == IPAddress((uint32_t)0xFFFFFFFF ) ) ||
				( IPAddress(addr) == IPAddress((uint32_t)0 ) ) ||
				(port == 0x00) )
			return 0;

		// set destination IP
		w5500->writeSnDIPR(s, addr);
		w5500->writeSnDPORT(s, port);
		w5500->execute_command(s, Sock_CONNECT);

		return 1;
	}

	void set(SOCKET s, uint8_t * mac, uint8_t * addr, uint16_t port)
	{
		w5500->writeSnDHAR(s, mac);
		w5500->writeSnDIPR(s, addr);
		w5500->writeSnDPORT(s, port);
	}


	/**
	 * @brief	This function used to send the data in TCP mode
	 * @return	1 for success else 0.
	 */
	uint16_t send(SOCKET s, const uint8_t * buf, uint16_t len)
	{
		uint16_t ret = 0;
		while ( ret<len )
		{
			if( sock_is_sending & (1<<s) )
			{
				sock_is_sending &= ~(1 << s);
				while ( !(w5500->readSnIR(s) & SnIR::SEND_OK) )
				{
					uint8_t snSR = w5500->readSnSR(s);
					if((snSR != SnSR::ESTABLISHED) && (snSR != SnSR::CLOSE_WAIT))
					{
						close(s); return 0;
					}
					if (w5500->readSnIR(s) & SnIR::TIMEOUT)
					{
						w5500->writeSnIR(s, (SnIR::SEND_OK | SnIR::TIMEOUT)); /* clear SEND_OK & TIMEOUT */
						close(s); return 0;
					}
				}
				/* +2008.01 bj */
				w5500->writeSnIR(s, SnIR::SEND_OK);
			}

			uint16_t trx = len - ret;
			if (trx > w5500->SSIZE) trx = w5500->SSIZE; // check size not to exceed MAX size.

			// if freebuf is available, start.
			while ( w5500->get_tx_free_size(s) < trx )
			{
				uint8_t status = w5500->readSnSR(s);
				if ((status != SnSR::ESTABLISHED) && (status != SnSR::CLOSE_WAIT))
				{
					close(s); return 0;
				}
			}

			// copy data
			w5500->send_data_processing(s, (uint8_t *)buf+ret, trx);
			w5500->execute_command(s, Sock_SEND);
			sock_is_sending |= (1 << s);

			ret += trx;
		}
		return ret;
	}


	/**
	 * @brief	This function is an application I/F function which is used to receive the data in TCP mode.
	 * 		It continues to wait for data as much as the application wants to receive.
	 *
	 * @return	received data size for success else -1.
	 */
	int16_t recv(SOCKET s, uint8_t *buf, int16_t len)
	{
		// Check how much data is available
		int16_t ret = w5500->get_rx_received_size(s);
		if ( ret == 0 )
		{
			// No data available.
			uint8_t status = w5500->readSnSR(s);
			if ( status == SnSR::LISTEN || status == SnSR::CLOSED || status == SnSR::CLOSE_WAIT )
			{
				// The remote end has closed its side of the connection, so this is the eof state
				ret = 0;
			}
			else
			{
				// The connection is still up, but there's no data waiting to be read
				ret = -1;
			}
		}
		else if (ret > len)
		{
			ret = len;
		}

		if ( ret > 0 )
		{
			w5500->recv_data_processing(s, buf, ret);
			w5500->execute_command(s, Sock_RECV);
		}
		return ret;
	}


	/**
	 * @brief	This function is an application I/F function which is used to send the data for other then TCP mode.
	 * 		Unlike TCP transmission, The peer's destination address and the port is needed.
	 *
	 * @return	This function return send data size for success else -1.
	 */
	uint16_t sendto(SOCKET s, const uint8_t *buf, uint16_t len, uint8_t *addr, uint16_t port)
	{
		if (len > w5500->SSIZE) len = w5500->SSIZE; // check size not to exceed MAX size.

		if ( ( IPAddress(addr) == IPAddress((uint32_t)0) ) || (port == 0x00) || (len == 0) ) {
			/* +2008.01 [bj] : added return value */
			return 0;
		}
		w5500->writeSnDIPR(s, addr);
		w5500->writeSnDPORT(s, port);

		return send(s, buf, len);
	}


	/**
	 * @brief	This function is an application I/F function which is used to receive the data in other then
	 * 	TCP mode. This function is used to receive UDP, IP_RAW and MAC_RAW mode, and handle the header as well.
	 *
	 * @return	This function return received data size for success else -1.
	 */
	uint16_t recvfrom(SOCKET s, uint8_t *buf, uint16_t len, uint8_t *addr, uint16_t *port)
	{
		uint8_t head[8];
		uint16_t data_len=0;

		if ( len > 0 )
		{
			uint16_t ptr = w5500->readSnRX_RD(s);
			switch (w5500->readSnMR(s) & 0x07)
			{
			case SnMR::UDP :
				w5500->read_data(s, ptr, head, 0x08);
				ptr += 8;
				// read peer's IP address, port number.
				memcpy(addr,head,4);
				*port = word(head[4], head[5]);
				data_len = word(head[6], head[7]);

				w5500->read_data(s, ptr, buf, data_len); // data copy.
				ptr += data_len;

				w5500->writeSnRX_RD(s, ptr);
				break;

			case SnMR::IPRAW :
				w5500->read_data(s, ptr, head, 0x06);
				ptr += 6;

				memcpy(addr,head,4);
				data_len = word(head[4], head[5]);

				w5500->read_data(s, ptr, buf, data_len); // data copy.
				ptr += data_len;

				w5500->writeSnRX_RD(s, ptr);
				break;

			case SnMR::MACRAW:
				w5500->read_data(s, ptr, head, 2);
				ptr+=2;
				data_len = word(head[0], head[1]) - 2;

				w5500->read_data(s, ptr, buf, data_len);
				ptr += data_len;
				w5500->writeSnRX_RD(s, ptr);
				break;

			default :
				break;
			}
			w5500->execute_command(s, Sock_RECV);
		}
		return data_len;
	}

	/**
	 * @brief	Wait for buffered transmission to complete.
	 */
	void flush(SOCKET s) {
		// TODO
		(void)s; // unsused
	}

	uint16_t igmpsend(SOCKET s, const uint8_t * buf, uint16_t len)
	{
		uint16_t ret=0;

		if (len > w5500->SSIZE)
			ret = w5500->SSIZE; // check size not to exceed MAX size.
		else
			ret = len;

		if (ret == 0)
			return 0;

		w5500->send_data_processing(s, (uint8_t *)buf, ret);
		w5500->execute_command(s, Sock_SEND);

		while ( !(w5500->readSnIR(s) & SnIR::SEND_OK) )
		{
			if (w5500->readSnIR(s) & SnIR::TIMEOUT)
			{
				/* in case of igmp, if send fails, then socket closed */
				/* if you want change, remove this code. */
				close(s);
				return 0;
			}
		}

		w5500->writeSnIR(s, SnIR::SEND_OK);
		return ret;
	}


	/*
	  @brief This function copies up to len bytes of data from buf into a UDP datagram to be
	  sent later by sendUDP.  Allows datagrams to be built up from a series of bufferData calls.
	  @return Number of bytes successfully buffered
	 */
	uint16_t bufferData(SOCKET s, uint16_t offset, const uint8_t* buf, uint16_t len)
	{
		uint16_t ret =0;
		if (len > w5500->get_tx_free_size(s))
		{
			ret = w5500->get_tx_free_size(s); // check size not to exceed MAX size.
		}
		else
		{
			ret = len;
		}
		w5500->send_data_processing_offset(s, offset, buf, ret);
		return ret;
	}

	// Functions to allow buffered UDP send (i.e. where the UDP datagram is built up over a
	// number of calls before being sent
	/*
	  @brief This function sets up a UDP datagram, the data for which will be provided by one
	  or more calls to bufferData and then finally sent with sendUDP.
	  @return 1 if the datagram was successfully set up, or 0 if there was an error
	 */
	int startUDP(SOCKET s, uint8_t* addr, uint16_t port)
	{
		if ( (IPAddress(addr) == IPAddress((uint32_t)0)) || (port == 0) ) {
			return 0;
		}
		else
		{
			w5500->writeSnDIPR(s, addr);
			w5500->writeSnDPORT(s, port);
			return 1;
		}
	}

	/*
	  @brief Send a UDP datagram built up from a sequence of startUDP followed by one or more
	  calls to bufferData.
	  @return 1 if the datagram was successfully sent, or 0 if there was an error
	 */
	int sendUDP(SOCKET s)
	{
		w5500->execute_command(s, Sock_SEND);

		/* +2008.01 bj */
		while ( !(w5500->readSnIR(s) & SnIR::SEND_OK) )
		{
			if (w5500->readSnIR(s) & SnIR::TIMEOUT)
			{
				/* +2008.01 [bj]: clear interrupt */
				w5500->writeSnIR(s, (SnIR::SEND_OK|SnIR::TIMEOUT));
				return 0;
			}
		}

		/* +2008.01 bj */
		w5500->writeSnIR(s, SnIR::SEND_OK);

		/* Sent ok */
		return 1;
	}


	W5500* get_hardware(void)
	{
		return this->w5500;
	}

private:
	W5500* w5500;

};

#endif /* _SOCKET_H_ */
