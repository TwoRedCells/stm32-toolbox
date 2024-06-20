/*
 * TcpIp.h
 *
 *  Created on: Jan 19, 2024
 *      Author: YvanRodriguez
 */

#ifndef LIB_STM32_TOOLBOX_COMMS_ETHERNET_W5500_TCPIP_H_
#define LIB_STM32_TOOLBOX_COMMS_ETHERNET_W5500_TCPIP_H_

#include <comms/tcpip/IPv4Address.h>
#include "Ethernet.h"


class TcpIp
{
public:
	TcpIp(Ethernet* ethernet)
	{
		this->ethernet = ethernet;
	}

	//void get_remote_ip(SOCKET s, uint8_t *ip) { ethernet->readSnDIPR(s, ip); }

	void set_gateway_ip(uint8_t* addr)  { ethernet->tcpip_set_gateway_ip(addr); }
	void get_gateway_ip(uint8_t* addr)  { ethernet->tcpip_get_gateway_ip(addr); }

	void set_subnet_mask(uint8_t* addr) { ethernet->tcpip_set_subnet_mask(addr); }
	void get_subnet_mask(uint8_t* addr) { ethernet->tcpip_get_subnet_mask(addr); }

	void set_mac_address(uint8_t* addr) { ethernet->set_mac_address(addr); }
	void get_mac_address(uint8_t* addr) { ethernet->get_mac_address(addr); }

	void set_ip_address(uint8_t* addr)  { ethernet->tcpip_set_ip_address(addr); }
	void get_ip_address(uint8_t* addr)  { ethernet->tcpip_get_ip_address(addr); }



	void begin(uint8_t *mac_address, IPv4Address local_ip)
	{
		// Assume the DNS server will be the machine on the same network as the local IP
		// but with last octet being '1'
		IPv4Address dns_server = local_ip;
		dns_server[3] = 1;
		begin(mac_address, local_ip, dns_server);
	}

	void begin(uint8_t *mac_address, IPv4Address local_ip, IPv4Address dns_server)
	{
		// Assume the gateway will be the machine on the same network as the local IP
		// but with last octet being '1'
		IPv4Address gateway = local_ip;
		gateway[3] = 1;
		begin(mac_address, local_ip, dns_server, gateway);
	}

	void begin(uint8_t *mac_address, IPv4Address local_ip, IPv4Address dns_server, IPv4Address gateway)
	{
		IPv4Address subnet(255, 255, 255, 0);
		begin(mac_address, local_ip, dns_server, gateway, subnet);
	}

	void begin(uint8_t *mac, IPv4Address local_ip, IPv4Address dns_server, IPv4Address gateway, IPv4Address subnet)
	{
		ethernet->set_mac_address(mac);
		set_ip_address(local_ip.raw_address());
		set_gateway_ip(gateway.raw_address());
		set_subnet_mask(subnet.raw_address());
	}

private:
	Ethernet* ethernet;
};
#endif /* LIB_STM32_TOOLBOX_COMMS_ETHERNET_W5500_TCPIP_H_ */
