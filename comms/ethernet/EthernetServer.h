#ifndef ETHERNETSERVER_H
#define ETHERNETSERVER_H

#include "Server.h"
#include "Socket.h"
#include "Ethernet.h"
#include "EthernetClient.h"


class EthernetServer : public Server
{
public:
	EthernetServer(Ethernet* ethernet, uint16_t port)
	{
		this->ethernet = ethernet;
		_port = port;
	}


	void begin()
	{
		for (int sock = 0; sock < MAX_SOCK_NUM; sock++)
		{
			EthernetClient client(ethernet, sock);
			if (client.status() == SnSR::CLOSED) {
				ethernet->get_socket()->open(sock, SnMR::TCP, _port, 0);
				ethernet->get_socket()->listen(sock);
				ethernet->server_port[sock] = _port;
				break;
			}
		}
	}


	EthernetClient available()
	{
		accept();

		for (int sock = 0; sock < MAX_SOCK_NUM; sock++)
		{
			EthernetClient client(ethernet, sock);
			if (ethernet->server_port[sock] == _port)
			{
				uint8_t s = client.status();
				if (s == SnSR::ESTABLISHED || s == SnSR::CLOSE_WAIT)
				{
					if (client.available())
					{
						// XXX: don't always pick the lowest numbered socket->
						return client;
					}
				}
			}
		}

		return EthernetClient(ethernet, MAX_SOCK_NUM);
	}

	virtual size_t write(uint8_t c)
	{
		write(&c, 1);
	}


	size_t write(const uint8_t *buffer, size_t size)
	{
		size_t n = 0;
		accept();

		for (int sock = 0; sock < MAX_SOCK_NUM; sock++)
		{
			EthernetClient client(ethernet, sock);
			if (ethernet->server_port[sock] == _port && client.status() == SnSR::ESTABLISHED)
				n += client.write(buffer, size);
		}

		return n;
	}
	using Print::write;


private:
	void accept()
	{
		int listening = 0;

		for (int sock = 0; sock < MAX_SOCK_NUM; sock++)
		{
			EthernetClient client(ethernet, sock);

			if (ethernet->server_port[sock] == _port)
			{
				if (client.status() == SnSR::LISTEN)
					listening = 1;
				else if (client.status() == SnSR::CLOSE_WAIT && !client.available())
					client.stop();
			}
		}

		if (!listening)
			begin();
	}

	uint16_t _port;
	Ethernet* ethernet;
};

#endif
