#ifndef ETHERNETCLIENT_H
#define ETHERNETCLIENT_H


#include "Socket.h"
#include "Dns.h"
#include "Client.h"


class TcpIpClient : public Client
{
public:
	TcpIpClient();
	TcpIpClient(uint8_t sock);

	using Print::print;

	uint8_t status();
	virtual int connect(IP4Address ip, uint16_t port);
	virtual int connect(const char *host, uint16_t port);
	size_t write(const uint8_t *buf, size_t size);
	size_t inline write(const char *buf, size_t size) { return write((const uint8_t *)buf, size); }
	size_t inline write(const char *buf) { return write((const uint8_t *)buf, strlen(buf)); }
	size_t write(uint8_t b) { return write(&b, 1); }
	virtual int available();
	virtual int read();
	virtual int read(uint8_t *buf, size_t size) { return socket.recv(_sock, buf, size); }
	virtual int peek();
	inline void flush() { socket.flush(_sock); }
	virtual void stop();
	virtual uint8_t connected();
	virtual operator bool() { return _sock != MAX_SOCK_NUM; }
	virtual bool operator==(const bool value) { return bool() == value; }
	virtual bool operator!=(const bool value) { return bool() != value; }
	virtual bool operator==(const TcpIpClient& rhs)
    				{ return _sock == rhs._sock && _sock != MAX_SOCK_NUM && rhs._sock != MAX_SOCK_NUM; }
	virtual bool operator!=(const TcpIpClient& rhs) { return !this->operator==(rhs); }
	inline uint8_t getSocketNumber() { return _sock; }
	inline void getRemoteIP(uint8_t * remoteIP) { socket.getRemoteIP(_sock, remoteIP); }


	uint16_t TcpIpClient::_srcport = 49152;      //Use IANA recommended ephemeral port range 49152-65535

	TcpIpClient::TcpIpClient() : _sock(MAX_SOCK_NUM) { }

	TcpIpClient::TcpIpClient(uint8_t sock) : _sock(sock) { }


	int connect(const char* host, uint16_t port)
	{
		// Look up the host first
		int ret = 0;
		DNSClient dns;
		IP4Address remote_addr;

		dns.begin(Ethernet.dnsServerIP());
		ret = dns.getHostByName(host, remote_addr);
		if (ret == 1) {
			return connect(remote_addr, port);
		} else {
			return ret;
		}
	}

	int connect(IP4Address ip, uint16_t port)
	{
		if (_sock != MAX_SOCK_NUM)
			return 0;

		for (int i = 0; i < MAX_SOCK_NUM; i++) {
			uint8_t s = socket.status(i);
			if (s == SnSR::CLOSED || s == SnSR::FIN_WAIT || s == SnSR::CLOSE_WAIT) {
				_sock = i;
				break;
			}
		}

		if (_sock == MAX_SOCK_NUM)
			return 0;

		_srcport++;
		if (_srcport == 0) _srcport = 49152;          //Use IANA recommended ephemeral port range 49152-65535
		socket.open(_sock, SnMR::TCP, _srcport, 0);


		if (!socket.connect(_sock, rawIPAddress(ip), port)) {
			_sock = MAX_SOCK_NUM;
			return 0;
		}

		Timer t;
		t.block(milliseconds(1000));

		while (status() != SnSR::ESTABLISHED) {
			t.block(milliseconds(1));
			if (status() == SnSR::CLOSED) {
				_sock = MAX_SOCK_NUM;
				return 0;
			}
		}

		return 1;
	}

	size_t write(const uint8_t *buf, size_t size)
	{
		if (_sock == MAX_SOCK_NUM) {
			setWriteError();
			return 0;
		}
		if (!socket.send(_sock, buf, size)) {
			setWriteError();
			return 0;
		}
		return size;
	}

	int available()
	{
		if (_sock != MAX_SOCK_NUM)
			return socket.recvAvailable(_sock);
		return 0;
	}

	int read()
	{
		uint8_t b;
		if ( socket.recv(_sock, &b, 1) > 0 )
		{
			// recv worked
			return b;
		}
		else
		{
			// No data available
			return -1;
		}
	}

	int peek()
	{
		uint8_t b;
		// Unlike recv, peek doesn't check to see if there's any data available, so we must
		if (!available())
			return -1;
		socket.peek(_sock, &b);
		return b;
	}

	void stop()
	{
		if (_sock == MAX_SOCK_NUM)
			return;

		flush();

		// attempt to close the connection gracefully (send a FIN to other side)
		socket.disconnect(_sock);
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
			socket.close(_sock);

		Ethernet.server_port[_sock] = 0;
		_sock = MAX_SOCK_NUM;
	}

	uint8_t connected()
	{
		if (_sock == MAX_SOCK_NUM) return 0;

		uint8_t s = status();
		return !(s == SnSR::LISTEN || s == SnSR::CLOSED || s == SnSR::FIN_WAIT ||
				(s == SnSR::CLOSE_WAIT && !available()));
	}

	uint8_t status()
	{
		if (_sock == MAX_SOCK_NUM) return SnSR::CLOSED;
		return socket.status(_sock);
	}


	friend class EthernetServer;

private:
	static uint16_t _srcport;
	uint8_t _sock;
};

#endif
