#ifndef	_SOCKET_H_
#define	_SOCKET_H_

#include "Ethernet.h"
#include "TcpIp.h"
#include "comms/tcpip/IPAddress.h"

#ifndef word
#define word(a, b) ( (uint16_t)((a)<<8) | (b) )
#endif

class Socket : public TcpIp
{
public:
	Socket(Ethernet* w5500) : TcpIp(w5500)
	{
		this->w5500 = w5500;
	}

//	void set_server_port(uint16_t port)
//	{
//		server_port[socket_no] = port;
//	}

	//int EthernetClass::maintain(){
	//	int rc = DHCP_CHECK_NONE;
	//	//we have a pointer to dhcp, use it
	//	rc = _dhcp.checkLease();
	//	switch ( rc ){
	//	case DHCP_CHECK_NONE:
	//		//nothing done
	//		break;
	//	case DHCP_CHECK_RENEW_OK:
	//	case DHCP_CHECK_REBIND_OK:
	//		//we might have got a new IP.
	//		socket->setIPAddress(_dhcp.getLocalIp().raw_address());
	//		socket->setGatewayIp(_dhcp.getGatewayIp().raw_address());
	//		socket->setSubnetMask(_dhcp.getSubnetMask().raw_address());
	//		_dnsServerAddress = _dhcp.getDnsServerIp();
	//		break;
	//	default:
	//		//this is actually a error, it will retry though
	//		break;
	//	}
	//	return rc;
	//}


	/**
	 * @brief	This Socket function initialize the channel in perticular mode, and set the port and wait for w5500 done it.
	 * @return 	1 for success else 0.
	 */
	bool open(uint8_t protocol, uint16_t port, uint8_t flag)
	{
		// Select a socket to use.
		socket_no = MAX_SOCK_NUM;
		for (int i = 0; i < MAX_SOCK_NUM; i++)
		{
			uint8_t s = status();
			if (s == SnSR::CLOSED || s == SnSR::FIN_WAIT)
			{
				socket_no = i;
				break;
			}
		}

		if (socket_no == MAX_SOCK_NUM)
			return false;

		sock_is_sending &= ~(1<<socket_no);

		if ((protocol == SnMR::TCP) || (protocol == SnMR::UDP) || (protocol == SnMR::IPRAW) || (protocol == SnMR::MACRAW) || (protocol == SnMR::PPPOE))
		{
			close();
			w5500->writeSnMR(socket_no, protocol | flag);
			w5500->writeSnPORT(socket_no, port);
			w5500->execute_command(socket_no, Sock_OPEN);
			return true;
		}
		return false;
	}


	/**
	 * @brief	This function close the socket and parameter is "s" which represent the socket number
	 */
	void close(void)
	{
		sock_is_sending &= ~(1<<socket_no);
		w5500->execute_command(socket_no, Sock_CLOSE);
		w5500->writeSnIR(socket_no, 0xFF);
	}


	/**
	 * @brief	This function established  the connection for the channel in passive (server) mode. This function waits for the request from the peer.
	 * @return	1 for success else 0.
	 */
	bool listen(void)
	{
		if (w5500->readSnSR(socket_no) != SnSR::INIT)
			return false;
		w5500->execute_command(socket_no, Sock_LISTEN);
			return true;
	}


	/**
	 * @brief	This function established  the connection for the channel in Active (client) mode.
	 * 		This function waits for the untill the connection is established.
	 *
	 * @return	1 for success else 0.
	 */
	bool connect(uint8_t* addr, uint16_t port)
	{
		assert(IPAddress(addr) != IPAddress((uint32_t)0xFFFFFFFF));
		assert(IPAddress(addr) != IPAddress((uint32_t)0));
		assert(port != 0x00);

		// set destination IP
		w5500->writeSnDIPR(socket_no, addr);
		w5500->writeSnDPORT(socket_no, port);
		w5500->execute_command(socket_no, Sock_CONNECT);
		return true;
	}


//	void set(uint8_t* mac, uint8_t* addr, uint16_t port)
//	{
//		w5500->writeSnDHAR(socket_no, mac);
//		w5500->writeSnDIPR(socket_no, addr);
//		w5500->writeSnDPORT(socket_no, port);
//	}


	/**
	 * @brief	This function used to send the data in TCP mode
	 * @return	1 for success else 0.
	 */
	uint16_t send(const void* buf, uint16_t len)
	{
		uint16_t ret = 0;
		while (ret < len)
		{
			if (sock_is_sending & (1 << socket_no))
			{
				sock_is_sending &= ~(1 << socket_no);
				while (!(w5500->readSnIR(socket_no) & SnIR::SEND_OK))
				{
					uint8_t snSR = w5500->readSnSR(socket_no);
					if((snSR != SnSR::ESTABLISHED) && (snSR != SnSR::CLOSE_WAIT))
					{
						close();
						return 0;
					}
					if (w5500->readSnIR(socket_no) & SnIR::TIMEOUT)
					{
						w5500->writeSnIR(socket_no, (SnIR::SEND_OK | SnIR::TIMEOUT)); /* clear SEND_OK & TIMEOUT */
						close();
						return 0;
					}
				}
				/* +2008.01 bj */
				w5500->writeSnIR(socket_no, SnIR::SEND_OK);
			}

			uint16_t trx = len - ret;
			if (trx > w5500->SSIZE)
				trx = w5500->SSIZE; // check size not to exceed MAX size.

			// if freebuf is available, start.
			while (w5500->get_tx_free_size(socket_no) < trx)
			{
				uint8_t status = w5500->readSnSR(socket_no);
				if ((status != SnSR::ESTABLISHED) && (status != SnSR::CLOSE_WAIT))
				{
					close();
					return 0;
				}
			}

			// copy data
			w5500->send_data_processing(socket_no, (uint8_t *)buf+ret, trx);
			w5500->execute_command(socket_no, Sock_SEND);
			sock_is_sending |= (1 << socket_no);
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
	int16_t recv(void *buf, int16_t len)
	{
		// Check how much data is available
		int16_t ret = w5500->get_rx_received_size(socket_no);
		if (ret == 0)
		{
			// No data available.
			uint8_t status = w5500->readSnSR(socket_no);
			if (status == SnSR::LISTEN || status == SnSR::CLOSED || status == SnSR::CLOSE_WAIT)
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

		if (ret > 0)
		{
			w5500->recv_data_processing(socket_no, buf, ret);
			w5500->execute_command(socket_no, Sock_RECV);
		}
		return ret;
	}


	/**
	 * @brief	This function is an application I/F function which is used to send the data for other then TCP mode.
	 * 		Unlike TCP transmission, The peer's destination address and the port is needed.
	 *
	 * @return	This function return send data size for success else -1.
	 */
	uint16_t sendto(const void *buf, uint16_t len, uint8_t *addr, uint16_t port)
	{
		if (len > w5500->SSIZE) len = w5500->SSIZE; // check size not to exceed MAX size.

		if (( IPAddress(addr) == IPAddress((uint32_t)0) ) || (port == 0x00) || (len == 0)) {
			/* +2008.01 [bj] : added return value */
			return 0;
		}
		w5500->writeSnDIPR(socket_no, addr);
		w5500->writeSnDPORT(socket_no, port);
		return send(buf, len);
	}


	/**
	 * @brief	This function is an application I/F function which is used to receive the data in other then
	 * 	TCP mode. This function is used to receive UDP, IP_RAW and MAC_RAW mode, and handle the header as well.
	 *
	 * @return	This function return received data size for success else -1.
	 */
	uint16_t recvfrom(void *buf, uint16_t len, uint8_t *addr, uint16_t *port)
	{
		uint8_t head[8];
		uint16_t data_len=0;

		if (len > 0)
		{
			uint16_t ptr = w5500->readSnRX_RD(socket_no);
			switch (w5500->readSnMR(socket_no) & 0x07)
			{
			case SnMR::UDP :
				w5500->read_data(socket_no, ptr, head, 0x08);
				ptr += 8;
				// read peer's IP address, port number.
				memcpy(addr,head,4);
				*port = (uint16_t)(head[4]<<8) | head[5];
				data_len = (uint16_t)(head[6]<<8) | head[7];

				w5500->read_data(socket_no, ptr, buf, data_len); // data copy.
				ptr += data_len;

				w5500->writeSnRX_RD(socket_no, ptr);
				break;

			case SnMR::IPRAW :
				w5500->read_data(socket_no, ptr, head, 0x06);
				ptr += 6;

				memcpy(addr,head,4);
				data_len = word(head[4], head[5]);

				w5500->read_data(socket_no, ptr, buf, data_len); // data copy.
				ptr += data_len;

				w5500->writeSnRX_RD(socket_no, ptr);
				break;

			case SnMR::MACRAW:
				w5500->read_data(socket_no, ptr, head, 2);
				ptr+=2;
				data_len = word(head[0], head[1]) - 2;

				w5500->read_data(socket_no, ptr, buf, data_len);
				ptr += data_len;
				w5500->writeSnRX_RD(socket_no, ptr);
				break;

			default :
				break;
			}
			w5500->execute_command(socket_no, Sock_RECV);
		}
		return data_len;
	}

	/**
	 * @brief	Wait for buffered transmission to complete.
	 */
	void flush(void)
	{
	}

	uint16_t igmpsend(const void * buf, uint16_t len)
	{
		uint16_t ret=0;

		if (len > w5500->SSIZE)
			ret = w5500->SSIZE; // check size not to exceed MAX size.
		else
			ret = len;

		if (ret == 0)
			return 0;

		w5500->send_data_processing(socket_no, (uint8_t *)buf, ret);
		w5500->execute_command(socket_no, Sock_SEND);

		while ( !(w5500->readSnIR(socket_no) & SnIR::SEND_OK) )
		{
			if (w5500->readSnIR(socket_no) & SnIR::TIMEOUT)
			{
				/* in case of igmp, if send fails, then socket closed */
				/* if you want change, remove this code. */
				close();
				return 0;
			}
		}

		w5500->writeSnIR(socket_no, SnIR::SEND_OK);
		return ret;
	}


	/*
	  @brief This function copies up to len bytes of data from buf into a UDP datagram to be
	  sent later by sendUDP.  Allows datagrams to be built up from a series of bufferData calls.
	  @return Number of bytes successfully buffered
	 */
	uint16_t bufferData(uint16_t offset, const void* buf, uint16_t len)
	{
		uint16_t ret =0;
		if (len > w5500->get_tx_free_size(socket_no))
			ret = w5500->get_tx_free_size(socket_no); // check size not to exceed MAX size.
		else
			ret = len;
		w5500->send_data_processing_offset(socket_no, offset, buf, ret);
		return ret;
	}


	/*
	  @brief This function sets up a UDP datagram, the data for which will be provided by one
	  or more calls to bufferData and then finally sent with sendUDP.
	  @return 1 if the datagram was successfully set up, or 0 if there was an error
	 */
	bool start_udp(IPAddress addr, uint16_t port)
	{
		if ( (IPAddress(addr) == IPAddress((uint32_t)0)) || (port == 0) )
		{
			return false;
		}
		else
		{
			w5500->writeSnDIPR(socket_no, addr.raw_address());
			w5500->writeSnDPORT(socket_no, port);
			return true;
		}
	}

	/*
	  @brief Send a UDP datagram built up from a sequence of startUDP followed by one or more
	  calls to bufferData.
	  @return 1 if the datagram was successfully sent, or 0 if there was an error
	 */
	bool send_udp(void)
	{
		w5500->execute_command(socket_no, Sock_SEND);

		/* +2008.01 bj */
		while (!(w5500->readSnIR(socket_no) & SnIR::SEND_OK))
		{
			if (w5500->readSnIR(socket_no) & SnIR::TIMEOUT)
			{
				/* +2008.01 [bj]: clear interrupt */
				w5500->writeSnIR(socket_no, (SnIR::SEND_OK|SnIR::TIMEOUT));
				return false;
			}
		}

		/* +2008.01 bj */
		w5500->writeSnIR(socket_no, SnIR::SEND_OK);
		return true;
	}


//	Ethernet* get_hardware(void)
//	{
//		return this->w5500;
//	}

	uint8_t status(void)
	{
		return w5500->readSnSR(socket_no);
	}

	int16_t available(void)
	{
		return w5500->get_rx_received_size(socket_no);
	}

	void disconnect(void)
	{
		w5500->execute_command(socket_no, Sock_DISCON);
	}

	void peek(uint8_t *buf)
	{
		w5500->recv_data_processing(socket_no, buf, 1, 1);
	}

	virtual uint8_t is_connected()
	{
		uint8_t s = status();
		return !(s == SnSR::LISTEN || s == SnSR::CLOSED || s == SnSR::FIN_WAIT ||
				(s == SnSR::CLOSE_WAIT && !available()));
	}


private:
	Ethernet* w5500;
	uint8_t socket_no;
	inline static uint16_t local_port = 0;
	inline static uint8_t sock_is_sending = 0;
	inline static uint16_t server_port[MAX_SOCK_NUM] = { 0, };
};

#endif /* _SOCKET_H_ */
