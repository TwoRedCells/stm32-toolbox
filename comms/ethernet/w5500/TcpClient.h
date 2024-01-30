#ifndef TCPCLIENT_H
#define TCPCLIENT_H


#include "Socket.h"
#include <string.h>
#if USING_FREERTOS
#include "cmsis_os.h"
#endif

class TcpClient
{
public:

//	virtual operator bool() { return _sock != MAX_SOCK_NUM; }
//
//	virtual bool operator==(const bool value) { return bool() == value; }
//	virtual bool operator!=(const bool value) { return bool() != value; }
//	virtual bool operator==(const TcpClient& rhs)
//    		{ return _sock == rhs._sock && _sock != MAX_SOCK_NUM && rhs._sock != MAX_SOCK_NUM; }
//
//	virtual bool operator!=(const TcpClient& rhs) { return !this->operator==(rhs); }
//

//	inline uint8_t get_socket_number() { return _sock; }
//	inline void getRemoteIP(uint8_t * remoteIP) { socket->getRemoteIP(_sock, remoteIP); }

	TcpClient(Socket* socket, uint8_t sock=MAX_SOCK_NUM)
	{
		this->socket = socket;
	}

	virtual int read(void *buf, size_t size)
	{
		return socket->recv(buf, size);
	}

	void flush_write()
	{
		socket->flush();
	}

	void flush_read()
	{
		while(available())
			read();
	}

	virtual bool connect(IPAddress& ip, uint16_t port, uint32_t timeout=5000)
	{
		socket->open(SnMR::TCP, assign_local_port(), 0);
		if (!socket->connect(ip.raw_address(), port))
			return false;

		Timer t(milliseconds(timeout));
		t.start();

		while (status() != SnSR::ESTABLISHED)
		{
#if USING_FREERTOS
			osDelay(1);
#endif
			if (t.is_elapsed())
				return false;
			if (status() == SnSR::CLOSED)
				return false;
		}
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

	virtual int available()
	{
		return socket->available();
	}

	virtual int read()
	{
		uint8_t b;
		if (socket->recv(&b, 1) > 0)
			return b;
		else
			return -1;
	}

	virtual int peek()
	{
		uint8_t b;
		// Unlike recv, peek doesn't check to see if there's any data available, so we must
		if (!available())
			return -1;
		socket->peek(&b);
		return b;
	}

	virtual void stop()
	{
		flush_write();

		// attempt to close the connection gracefully (send a FIN to other side)
		socket->disconnect();
		unsigned long start = millis();

		// wait up to a second for the connection to close
		uint8_t s;
		do {
			s = status();
			if (s == SnSR::CLOSED)
				break; // exit the loop
			Timer::Block(milliseconds(1));
		} while (millis() - start < 1000);

		// if it hasn't closed, close it forcefully
		if (s != SnSR::CLOSED)
			socket->close();
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

	static uint16_t assign_local_port(void)
	{
		static uint16_t port = 49152; //Use IANA recommended ephemeral port range 49152-65535
		if (port == 0) port = 49152;
		return port++;
	}

private:
	Socket* socket;
};

#endif
