#ifndef ETHERNETSERVER_H
#define ETHERNETSERVER_H

#include "Socket.h"
#include "Ethernet.h"
#include "TcpClient.h"

class TcpServer : public TcpClient
{
public:
	TcpServer(Socket* socket, uint16_t port) : TcpClient(socket)
	{
		this->socket = socket;
		this->port = port;
	}

	bool open(void)
	{
		return socket->open(SnMR::TCP, port, 0);
	}


	bool listen(void)
	{
		return socket->listen();
	}

	size_t available() override
	{
		return (status() == SnSR::ESTABLISHED || status() == SnSR::CLOSE_WAIT) && TcpClient::available();
	}

	using TcpClient::read;


private:
//	TcpClient accept(void)
//	{
//		TcpClient client (socket);
//		uint8_t status = client.status();
//		if (status == SnSR::LISTEN)
//		{
//			return client;
//		}
//
//		if (status == SnSR::CLOSE_WAIT && !client.available())
//		{
//			socket->close();
//			open();
//		}
//
//		return client;
//	}

	uint16_t port;
	Socket *socket;
};

#endif
