/*
 modified 12 Aug 2013
 by Soohwan Kim (suhwan@wiznet.co.kr)
 */
#ifndef ETHERNET_STM32_H
#define ETHERNET_STM32_H


#include "IPAddress.h"
#include "Socket.h"

class Ethernet
{
private:
	IPAddress _dnsServerAddress;
	Socket* socket;

public:
	Ethernet(Socket* socket)
	{
		this->socket = socket;
	}

	inline static uint8_t _state[MAX_SOCK_NUM] = { 0, };
	inline static uint16_t server_port[MAX_SOCK_NUM] = { 0, };

	//void init(SPI_HandleTypeDef &spi, const uint8_t cs_pin) { socket.init(spi, cs_pin); }
	//void init(const uint8_t cs_pin) { init(SPI, cs_pin); }
	// Initialize the Ethernet shield to use the provided MAC address and gain the rest of the
	// configuration through DHCP.
	// Returns 0 if the DHCP configuration failed, and 1 if it succeeded
	int begin(uint8_t *mac_address, unsigned long timeout = 60000, unsigned long responseTimeout = 4000);


	//DhcpClass _dhcp;
	//
	//int EthernetClass::begin(uint8_t *mac_address, unsigned long timeout, unsigned long responseTimeout)
	//{
	//	// Initialise the basic info
	//	socket->setMACAddress(mac_address);
	//	socket->setIPAddress(IPAddress(0,0,0,0).raw_address());
	//
	//	// Now try to get our config info from a DHCP server
	//	int ret = _dhcp.beginWithDHCP(mac_address, timeout, responseTimeout);
	//	if(ret == 1)
	//	{
	//		// We've successfully found a DHCP server and got our configuration info, so set things
	//		// accordingly
	//		socket->setIPAddress(_dhcp.getLocalIp().raw_address());
	//		socket->setGatewayIp(_dhcp.getGatewayIp().raw_address());
	//		socket->setSubnetMask(_dhcp.getSubnetMask().raw_address());
	//		_dnsServerAddress = _dhcp.getDnsServerIp();
	//	}
	//
	//	return ret;
	//}

	void begin(uint8_t *mac_address, IPAddress local_ip)
	{
		// Assume the DNS server will be the machine on the same network as the local IP
		// but with last octet being '1'
		IPAddress dns_server = local_ip;
		dns_server[3] = 1;
		begin(mac_address, local_ip, dns_server);
	}

	void begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server)
	{
		// Assume the gateway will be the machine on the same network as the local IP
		// but with last octet being '1'
		IPAddress gateway = local_ip;
		gateway[3] = 1;
		begin(mac_address, local_ip, dns_server, gateway);
	}

	void begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway)
	{
		IPAddress subnet(255, 255, 255, 0);
		begin(mac_address, local_ip, dns_server, gateway, subnet);
	}

	void begin(uint8_t *mac, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet)
	{
		socket->setMACAddress(mac);
		socket->setIPAddress(local_ip.raw_address());
		socket->setGatewayIp(gateway.raw_address());
		socket->setSubnetMask(subnet.raw_address());
		_dnsServerAddress = dns_server;
	}

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

	IPAddress localIP()
	{
		IPAddress ret;
		socket->getIPAddress(ret.raw_address());
		return ret;
	}

	IPAddress subnetMask()
	{
		IPAddress ret;
		socket->getSubnetMask(ret.raw_address());
		return ret;
	}

	IPAddress gatewayIP()
	{
		IPAddress ret;
		socket->getGatewayIp(ret.raw_address());
		return ret;
	}

	IPAddress dnsServerIP()
	{
		return _dnsServerAddress;
	}

	Socket* get_socket(void)
	{
		return socket;
	}
};

#endif
