#ifndef TCPCLIENT_H
#define TCPCLIENT_H


#include "Socket.h"
#include "comms/tcpip/ITcp.h"
#include "utility/IWrite.h"
#include "toolbox.h"
#include "comms/ethernet/IEthernet.h"


class Tcp: public Socket, public IWrite, public ITcp
{
public:
	Tcp(IEthernet* ethernet) : Socket(ethernet)
	{
	}

	bool Open(void) override
	{
		return Socket::Open(SnMR::TCP, assign_local_port());
	}

	uint8_t Read(void) override
	{
		uint8_t b;
		Receive(&b, 1);
		return b;
	}

	virtual bool Connect(IPv4Address& ip, uint16_t port, uint32_t timeout=5000) override
	{
		Connect(ip, port);

		Timer t(milliseconds(timeout));
		t.start();

		while (Status() != SnSR::ESTABLISHED)
			if (t.is_elapsed() || Status() == SnSR::CLOSED)
				return false;

		return true;
	}


	virtual void Disconnect(uint32_t timeout=1000) override
	{
		Timer t(milliseconds(1000));
		t.start();

		// Purge buffers.
		while (Socket::Available())
			Read();
		Flush();

		// attempt to close the connection gracefully (send a FIN to other side)
		Disconnect();

		// wait up to a second for the connection to close
		for (uint8_t s = Status(); s != SnSR::CLOSED; )
		{
			s = Status();
			if (t.is_elapsed())
			{
				Close();
				break;
			}
		}
	}

	virtual size_t Write(uint8_t byte) override
	{
		// If the TX buffer is full, flush it.
		if (GetTxFree() == 0)
			Flush();

		Write(&byte, 1);
		return 1;
	}


	virtual uint8_t Status()
	{
		if (!IsConnected())
			return SnSR::CLOSED;
		return Socket::Status();
	}


	static uint16_t assign_local_port(void)
	{
		static uint16_t port = 49152; //Use IANA recommended ephemeral port range 49152-65535
		if (port == 0)
			port = 49152;
		return port++;
	}

	virtual void Flush(void)
	{
		Socket::Flush();
	}

	virtual size_t Read(void *buf, size_t size)
	{
		return Socket::Receive(buf, size);
	}

	virtual size_t Write(const void *buf, size_t size)
	{
		return Socket::Send(buf,  size);
	}

	virtual void Peek(uint8_t *buf, uint16_t len)
	{
		return Socket::Peek(buf, len);
	}

	virtual bool IsConnected()
	{
		return Socket::IsConnected();
	}

	virtual void Listen(void)
	{
		Socket::Listen();
	}

	virtual size_t Available(void)
	{
		return Socket::Available();
	}
};

#endif
