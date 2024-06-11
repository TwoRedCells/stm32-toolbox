#ifndef TCPCLIENT_H
#define TCPCLIENT_H


#include "Socket.h"
#include "utility/IWrite.h"
#include <string.h>
#include "toolbox.h"


class TcpClient : public IWrite
{
public:
//	inline void getRemoteIP(uint8_t * remoteIP) { socket->getRemoteIP(_sock, remoteIP); }

	TcpClient(Socket* socket)
	{
		this->socket = socket;
	}

	virtual size_t read(void *buf, size_t size)
	{
		return socket->recv(buf, size);
	}

	void flush()
	{
		socket->flush();
	}

	void purge()
	{
		while(available())
			read();
	}

	virtual bool connect(IPv4Address& ip, uint16_t port, uint32_t timeout=5000)
	{
		socket->open(SnMR::TCP, assign_local_port(), 0);
		socket->connect(ip.raw_address(), port);

		Timer t(milliseconds(timeout));
		t.start();

		while (status() != SnSR::ESTABLISHED)
			if (t.is_elapsed() || status() == SnSR::CLOSED)
				return false;

		return true;
	}

	size_t write(const void *buf, size_t size)
	{
		if (!socket->send(buf, size))
			return 0;
		return size;
	}

	size_t write(const char *buf)
	{
		return write((const uint8_t *)buf, strlen(buf));
	}

	size_t write(uint8_t b)
	{
		return write(&b, 1);
	}

	virtual size_t available()
	{
		return socket->available();
	}

	virtual uint8_t read()
	{
		uint8_t b;
		socket->recv(&b, 1);
		return b;
	}

	virtual int peek()
	{
		uint8_t b;
		socket->peek(&b);
		return b;
	}

	virtual void stop(uint32_t timeout=1000)
	{
		Timer t(milliseconds(1000));
		t.start();

		// attempt to close the connection gracefully (send a FIN to other side)
		socket->disconnect();

		// wait up to a second for the connection to close
		for (uint8_t s = status(); s != SnSR::CLOSED; )
		{
			s = status();
			if (t.is_elapsed())
			{
				socket->close();
				break;
			}
		}
	}

	virtual uint8_t connected()
	{
		uint8_t s = status();
		return !(s == SnSR::LISTEN || s == SnSR::CLOSED || s == SnSR::FIN_WAIT ||
				(s == SnSR::CLOSE_WAIT && !available()));
	}

	virtual uint8_t status()
	{
		if (!socket->is_connected())
			return SnSR::CLOSED;
		return socket->status();
	}

protected:
	static uint16_t assign_local_port(void)
	{
		static uint16_t port = 49152; //Use IANA recommended ephemeral port range 49152-65535
		if (port == 0)
			port = 49152;
		return port++;
	}

private:
	Socket* socket;
};

#endif
