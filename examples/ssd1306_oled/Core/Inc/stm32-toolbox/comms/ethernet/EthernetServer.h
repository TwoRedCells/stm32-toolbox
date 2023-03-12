#ifndef ETHERNETSERVER_H
#define ETHERNETSERVER_H


#include "Server.h"
#include "Ethernet.h"


extern EthernetClass Ethernet;
class EthernetClient;

class EthernetServer : public Server {
private:
	uint16_t _port;
	void accept();
public:
	inline size_t write(uint8_t b) { return write(&b, 1); }
	using Print::write;


	EthernetServer(uint16_t port)
	{
		_port = port;
	}

	void begin()
	{
		for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
			EthernetClient client(sock);
			if (client.status() == SnSR::CLOSED) {
				socket.open(sock, SnMR::TCP, _port, 0);
				socket.listen(sock);
				Ethernet.server_port[sock] = _port;
				break;
			}
		}
	}


	EthernetClient available()
	{
		accept();

		for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
			EthernetClient client(sock);
			if (Ethernet.server_port[sock] == _port) {
				uint8_t s = client.status();
				if (s == SnSR::ESTABLISHED || s == SnSR::CLOSE_WAIT) {
					if (client.available()) {
						// XXX: don't always pick the lowest numbered socket.
						return client;
					}
				}
			}
		}

		return EthernetClient(MAX_SOCK_NUM);
	}


	size_t write(const uint8_t *buffer, size_t size)
	{
		size_t n = 0;

		accept();

		for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
			EthernetClient client(sock);

			if (Ethernet.server_port[sock] == _port &&
					client.status() == SnSR::ESTABLISHED) {
				n += client.write(buffer, size);
			}
		}
		return n;
	}

private:
	void accept()
	{
		int listening = 0;

		for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
			EthernetClient client(sock);

			if (Ethernet.server_port[sock] == _port) {
				if (client.status() == SnSR::LISTEN) {
					listening = 1;
				}
				else if (client.status() == SnSR::CLOSE_WAIT && !client.available()) {
					client.stop();
				}
			}
		}

		if (!listening) {
			begin();
		}
	}
};

#endif
