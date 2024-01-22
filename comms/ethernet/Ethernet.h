#ifndef ETHERNET_STM32_H
#define ETHERNET_STM32_H


class Ethernet
{
public:
	inline static uint8_t _state[MAX_SOCK_NUM] = { 0, };

	// Initialize the Ethernet shield to use the provided MAC address and gain the rest of the
	// configuration through DHCP.
	// Returns 0 if the DHCP configuration failed, and 1 if it succeeded
	int begin(uint8_t *mac_address, unsigned long timeout = 60000, unsigned long responseTimeout = 4000);

	socket->setMACAddress(mac);

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


};

#endif
