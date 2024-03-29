// DHCP Library v0.3 - April 25, 2009
// Author: Jordan Terrell - blog.jordanterrell.com

#ifndef Dhcp_h
#define Dhcp_h

#include "EthernetUdp.h"
#include "util.h"

/* DHCP state machine. */
#define STATE_DHCP_START 0
#define	STATE_DHCP_DISCOVER	1
#define	STATE_DHCP_REQUEST	2
#define	STATE_DHCP_LEASED	3
#define	STATE_DHCP_REREQUEST	4
#define	STATE_DHCP_RELEASE	5

#define DHCP_FLAGSBROADCAST	0x8000

/* UDP port numbers for DHCP */
#define	DHCP_SERVER_PORT	67	/* from server to client */
#define DHCP_CLIENT_PORT	68	/* from client to server */

/* DHCP message OP code */
#define DHCP_BOOTREQUEST	1
#define DHCP_BOOTREPLY		2

/* DHCP message type */
#define	DHCP_DISCOVER		1
#define DHCP_OFFER		  2
#define	DHCP_REQUEST		3
#define	DHCP_DECLINE		4
#define	DHCP_ACK		    5
#define DHCP_NAK		    6
#define	DHCP_RELEASE		7
#define DHCP_INFORM		  8

#define DHCP_HTYPE10MB		1
#define DHCP_HTYPE100MB		2

#define DHCP_HLENETHERNET	6
#define DHCP_HOPS		0
#define DHCP_SECS		0

#define MAGIC_COOKIE		0x63825363
#define MAX_DHCP_OPT	16

#define HOST_NAME "WIZnet"
#define DEFAULT_LEASE	(900) //default lease time in seconds

#define DHCP_CHECK_NONE         (0)
#define DHCP_CHECK_RENEW_FAIL   (1)
#define DHCP_CHECK_RENEW_OK     (2)
#define DHCP_CHECK_REBIND_FAIL  (3)
#define DHCP_CHECK_REBIND_OK    (4)

enum
{
	padOption		=	0,
	subnetMask		=	1,
	timerOffset		=	2,
	routersOnSubnet		=	3,
	/* timeServer		=	4,
	nameServer		=	5,*/
	dns			=	6,
	/*logServer		=	7,
	cookieServer		=	8,
	lprServer		=	9,
	impressServer		=	10,
	resourceLocationServer	=	11,*/
	hostName		=	12,
	/*bootFileSize		=	13,
	meritDumpFile		=	14,*/
	domainName		=	15,
	/*swapServer		=	16,
	rootPath		=	17,
	extentionsPath		=	18,
	IPforwarding		=	19,
	nonLocalSourceRouting	=	20,
	policyFilter		=	21,
	maxDgramReasmSize	=	22,
	defaultIPTTL		=	23,
	pathMTUagingTimeout	=	24,
	pathMTUplateauTable	=	25,
	ifMTU			=	26,
	allSubnetsLocal		=	27,
	broadcastAddr		=	28,
	performMaskDiscovery	=	29,
	maskSupplier		=	30,
	performRouterDiscovery	=	31,
	routerSolicitationAddr	=	32,
	staticRoute		=	33,
	trailerEncapsulation	=	34,
	arpCacheTimeout		=	35,
	ethernetEncapsulation	=	36,
	tcpDefaultTTL		=	37,
	tcpKeepaliveInterval	=	38,
	tcpKeepaliveGarbage	=	39,
	nisDomainName		=	40,
	nisServers		=	41,
	ntpServers		=	42,
	vendorSpecificInfo	=	43,
	netBIOSnameServer	=	44,
	netBIOSdgramDistServer	=	45,
	netBIOSnodeType		=	46,
	netBIOSscope		=	47,
	xFontServer		=	48,
	xDisplayManager		=	49,*/
	dhcpRequestedIPaddr	=	50,
	dhcpIPaddrLeaseTime	=	51,
	/*dhcpOptionOverload	=	52,*/
	dhcpMessageType		=	53,
	dhcpServerIdentifier	=	54,
	dhcpParamRequest	=	55,
	/*dhcpMsg			=	56,
	dhcpMaxMsgSize		=	57,*/
	dhcpT1value		=	58,
	dhcpT2value		=	59,
	/*dhcpClassIdentifier	=	60,*/
	dhcpClientIdentifier	=	61,
	endOption		=	255
};

typedef struct _RIP_MSG_FIXED
{
	uint8_t  op; 
	uint8_t  htype; 
	uint8_t  hlen;
	uint8_t  hops;
	uint32_t xid;
	uint16_t secs;
	uint16_t flags;
	uint8_t  ciaddr[4];
	uint8_t  yiaddr[4];
	uint8_t  siaddr[4];
	uint8_t  giaddr[4];
	uint8_t  chaddr[6];
} RIP_MSG_FIXED;


class DhcpClass
{
public:
	DhcpClass(EthernetUDP* udp)
	{
		this->udp = udp;
	}

	IPAddress getLocalIp() { return IPAddress(_dhcpLocalIp); }
	IPAddress getSubnetMask() { return IPAddress(_dhcpSubnetMask); }
	IPAddress getGatewayIp() { return IPAddress(_dhcpGatewayIp); }
	IPAddress getDhcpServerIp() { return IPAddress(_dhcpDhcpServerIp); }
	IPAddress getDnsServerIp() { return IPAddress(_dhcpDnsServerIp); }


	int beginWithDHCP(uint8_t *mac, unsigned long timeout, unsigned long responseTimeout)
	{
		_dhcpLeaseTime=0;
		_dhcpT1=0;
		_dhcpT2=0;
		_timeout = timeout;
		_responseTimeout = responseTimeout;

		// zero out _dhcpMacAddr
		memset(_dhcpMacAddr, 0, 6);
		reset_DHCP_lease();

		memcpy((uint8_t*)_dhcpMacAddr, (uint8_t*)mac, 6);
		_dhcp_state = STATE_DHCP_START;
		return request_DHCP_lease();
	}


	/*
	    returns:
	    0/DHCP_CHECK_NONE: nothing happened
	    1/DHCP_CHECK_RENEW_FAIL: renew failed
	    2/DHCP_CHECK_RENEW_OK: renew success
	    3/DHCP_CHECK_REBIND_FAIL: rebind fail
	    4/DHCP_CHECK_REBIND_OK: rebind success
	 */
	int checkLease()
	{
		int rc = DHCP_CHECK_NONE;

		unsigned long now = millis();
		unsigned long elapsed = now - _lastCheckLeaseMillis;

		// if more then one sec passed, reduce the counters accordingly
		if (elapsed >= 1000)
		{
			// set the new timestamps
			_lastCheckLeaseMillis = now - (elapsed % 1000);
			elapsed = elapsed / 1000;

			// decrease the counters by elapsed seconds
			// we assume that the cycle time (elapsed) is fairly constant
			// if the remainder is less than cycle time * 2
			// do it early instead of late
			if (_renewInSec < elapsed * 2)
				_renewInSec = 0;
			else
				_renewInSec -= elapsed;

			if (_rebindInSec < elapsed * 2)
				_rebindInSec = 0;
			else
				_rebindInSec -= elapsed;
		}

		// if we have a lease but should renew, do it
		if (_renewInSec == 0 &&_dhcp_state == STATE_DHCP_LEASED)
		{
			_dhcp_state = STATE_DHCP_REREQUEST;
			rc = 1 + request_DHCP_lease();
		}

		// if we have a lease or is renewing but should bind, do it
		if (_rebindInSec == 0 && (_dhcp_state == STATE_DHCP_LEASED || _dhcp_state == STATE_DHCP_START))
		{
			// this should basically restart completely
			_dhcp_state = STATE_DHCP_START;
			reset_DHCP_lease();
			rc = 3 + request_DHCP_lease();
		}
		return rc;
	}


private:
	uint32_t _dhcpInitialTransactionId;
	uint32_t _dhcpTransactionId;
	uint8_t  _dhcpMacAddr[6];
	uint8_t  _dhcpLocalIp[4];
	uint8_t  _dhcpSubnetMask[4];
	uint8_t  _dhcpGatewayIp[4];
	uint8_t  _dhcpDhcpServerIp[4];
	uint8_t  _dhcpDnsServerIp[4];
	uint32_t _dhcpLeaseTime;
	uint32_t _dhcpT1, _dhcpT2;
	uint32_t _renewInSec;
	uint32_t _rebindInSec;
	uint32_t _timeout;
	uint32_t _responseTimeout;
	uint32_t _lastCheckLeaseMillis;
	uint8_t _dhcp_state;
	EthernetUDP* udp;


	void reset_DHCP_lease()
	{
		// zero out _dhcpSubnetMask, _dhcpGatewayIp, _dhcpLocalIp, _dhcpDhcpServerIp, _dhcpDnsServerIp
		memset(_dhcpLocalIp, 0, 20);
	}

	//return:0 on error, 1 if request is sent and response is received
	int request_DHCP_lease()
	{
		uint8_t messageType = 0;

		// Pick an initial transaction ID
		_dhcpTransactionId = (rand() % (2000UL - 1UL)) + 1UL;
		_dhcpInitialTransactionId = _dhcpTransactionId;

		udp->stop();
		if (udp->begin(DHCP_CLIENT_PORT) == 0)
			return 0;  // Couldn't get a socket

		presend_DHCP();
		int result = 0;
		unsigned long startTime = millis();

		while(_dhcp_state != STATE_DHCP_LEASED)
		{
			if(_dhcp_state == STATE_DHCP_START)
			{
				_dhcpTransactionId++;

				send_DHCP_MESSAGE(DHCP_DISCOVER, ((millis() - startTime) / 1000));
				_dhcp_state = STATE_DHCP_DISCOVER;
			}
			else if(_dhcp_state == STATE_DHCP_REREQUEST)
			{
				_dhcpTransactionId++;
				send_DHCP_MESSAGE(DHCP_REQUEST, ((millis() - startTime)/1000));
				_dhcp_state = STATE_DHCP_REQUEST;
			}
			else if(_dhcp_state == STATE_DHCP_DISCOVER)
			{
				uint32_t respId;
				messageType = parseDHCPResponse(_responseTimeout, respId);
				if(messageType == DHCP_OFFER)
				{
					// We'll use the transaction ID that the offer came with,
					// rather than the one we were up to
					_dhcpTransactionId = respId;
					send_DHCP_MESSAGE(DHCP_REQUEST, ((millis() - startTime) / 1000));
					_dhcp_state = STATE_DHCP_REQUEST;
				}
			}
			else if(_dhcp_state == STATE_DHCP_REQUEST)
			{
				uint32_t respId;
				messageType = parseDHCPResponse(_responseTimeout, respId);
				if(messageType == DHCP_ACK)
				{
					_dhcp_state = STATE_DHCP_LEASED;
					result = 1;
					//use default lease time if we didn't get it
					if(_dhcpLeaseTime == 0){
						_dhcpLeaseTime = DEFAULT_LEASE;
					}
					// Calculate T1 & T2 if we didn't get it
					if(_dhcpT1 == 0){
						// T1 should be 50% of _dhcpLeaseTime
						_dhcpT1 = _dhcpLeaseTime >> 1;
					}
					if(_dhcpT2 == 0){
						// T2 should be 87.5% (7/8ths) of _dhcpLeaseTime
						_dhcpT2 = _dhcpLeaseTime - (_dhcpLeaseTime >> 3);
					}
					_renewInSec = _dhcpT1;
					_rebindInSec = _dhcpT2;
				}
				else if(messageType == DHCP_NAK)
					_dhcp_state = STATE_DHCP_START;
			}

			if(messageType == 255)
			{
				messageType = 0;
				_dhcp_state = STATE_DHCP_START;
			}

			if(result != 1 && ((millis() - startTime) > _timeout))
				break;
		}

		// We're done with the socket now
		udp->stop();
		_dhcpTransactionId++;
		_lastCheckLeaseMillis = millis();
		return result;
	}

	void presend_DHCP()
	{
	}

	void send_DHCP_MESSAGE(uint8_t messageType, uint16_t secondsElapsed)
	{
		uint8_t buffer[32];
		memset(buffer, 0, 32);
		IPAddress dest_addr(255, 255, 255, 255); // Broadcast address

		if (-1 == udp->beginPacket(dest_addr, DHCP_SERVER_PORT))
		{
			// FIXME Need to return errors
			return;
		}

		buffer[0] = DHCP_BOOTREQUEST;   // op
		buffer[1] = DHCP_HTYPE10MB;     // htype
		buffer[2] = DHCP_HLENETHERNET;  // hlen
		buffer[3] = DHCP_HOPS;          // hops

		// xid
		unsigned long xid = htonl(_dhcpTransactionId);
		memcpy(buffer + 4, &(xid), 4);

		// 8, 9 - seconds elapsed
		buffer[8] = ((secondsElapsed & 0xff00) >> 8);
		buffer[9] = (secondsElapsed & 0x00ff);

		// flags
		unsigned short flags = htons(DHCP_FLAGSBROADCAST);
		memcpy(buffer + 10, &(flags), 2);

		// ciaddr: already zeroed
		// yiaddr: already zeroed
		// siaddr: already zeroed
		// giaddr: already zeroed

		//put data in W5100 transmit buffer
		udp->write(buffer, 28);

		memset(buffer, 0, 32); // clear local buffer
		memcpy(buffer, _dhcpMacAddr, 6); // chaddr

		//put data in W5100 transmit buffer
		udp->write(buffer, 16);
		memset(buffer, 0, 32); // clear local buffer

		// leave zeroed out for sname && file
		// put in W5100 transmit buffer x 6 (192 bytes)

		for(int i = 0; i < 6; i++)
			udp->write(buffer, 32);

		// OPT - Magic Cookie
		buffer[0] = (uint8_t)((MAGIC_COOKIE >> 24)& 0xFF);
		buffer[1] = (uint8_t)((MAGIC_COOKIE >> 16)& 0xFF);
		buffer[2] = (uint8_t)((MAGIC_COOKIE >> 8)& 0xFF);
		buffer[3] = (uint8_t)(MAGIC_COOKIE& 0xFF);

		// OPT - message type
		buffer[4] = dhcpMessageType;
		buffer[5] = 0x01;
		buffer[6] = messageType; //DHCP_REQUEST;

		// OPT - client identifier
		buffer[7] = dhcpClientIdentifier;
		buffer[8] = 0x07;
		buffer[9] = 0x01;
		memcpy(buffer + 10, _dhcpMacAddr, 6);

		// OPT - host name
		buffer[16] = hostName;
		buffer[17] = strlen(HOST_NAME) + 6; // length of hostname + last 3 bytes of mac address
		strcpy((char*)&(buffer[18]), HOST_NAME);

		printByte((char*)&(buffer[24]), _dhcpMacAddr[3]);
		printByte((char*)&(buffer[26]), _dhcpMacAddr[4]);
		printByte((char*)&(buffer[28]), _dhcpMacAddr[5]);

		//put data in W5100 transmit buffer
		udp->write(buffer, 30);

		if(messageType == DHCP_REQUEST)
		{
			buffer[0] = dhcpRequestedIPaddr;
			buffer[1] = 0x04;
			buffer[2] = _dhcpLocalIp[0];
			buffer[3] = _dhcpLocalIp[1];
			buffer[4] = _dhcpLocalIp[2];
			buffer[5] = _dhcpLocalIp[3];

			buffer[6] = dhcpServerIdentifier;
			buffer[7] = 0x04;
			buffer[8] = _dhcpDhcpServerIp[0];
			buffer[9] = _dhcpDhcpServerIp[1];
			buffer[10] = _dhcpDhcpServerIp[2];
			buffer[11] = _dhcpDhcpServerIp[3];

			//put data in W5100 transmit buffer
			udp->write(buffer, 12);
		}

		buffer[0] = dhcpParamRequest;
		buffer[1] = 0x06;
		buffer[2] = subnetMask;
		buffer[3] = routersOnSubnet;
		buffer[4] = dns;
		buffer[5] = domainName;
		buffer[6] = dhcpT1value;
		buffer[7] = dhcpT2value;
		buffer[8] = endOption;

		//put data in W5100 transmit buffer
		udp->write(buffer, 9);
		udp->endPacket();
	}


	uint8_t parseDHCPResponse(unsigned long responseTimeout, uint32_t& transactionId)
	{
		uint8_t type = 0;
		uint8_t opt_len = 0;

		unsigned long startTime = millis();

		while(udp->parsePacket() <= 0)
		{
			if((millis() - startTime) > responseTimeout)
				return 255;
			Timer t;
			t.block(milliseconds(50));
		}

		// start reading in the packet
		RIP_MSG_FIXED fixedMsg;
		udp->read((uint8_t*)&fixedMsg, sizeof(RIP_MSG_FIXED));

		if(fixedMsg.op == DHCP_BOOTREPLY && udp->remotePort() == DHCP_SERVER_PORT)
		{
			transactionId = ntohl(fixedMsg.xid);
			if(memcmp(fixedMsg.chaddr, _dhcpMacAddr, 6) != 0 || (transactionId < _dhcpInitialTransactionId) || (transactionId > _dhcpTransactionId))
			{
				// Need to read the rest of the packet here regardless
				udp->flush();
				return 0;
			}

			memcpy(_dhcpLocalIp, fixedMsg.yiaddr, 4);

			// Skip to the option part
			// Doing this a byte at a time so we don't have to put a big buffer
			// on the stack (as we don't have lots of memory lying around)
			for (int i =0; i < (240 - (int)sizeof(RIP_MSG_FIXED)); i++)
				udp->read(); // we don't care about the returned byte

			while (udp->available() > 0)
			{
				switch (udp->read())
				{
				case endOption :
					break;

				case padOption :
					break;

				case dhcpMessageType :
					opt_len = udp->read();
					type = udp->read();
					break;

				case subnetMask :
					opt_len = udp->read();
					udp->read(_dhcpSubnetMask, 4);
					break;

				case routersOnSubnet :
					opt_len = udp->read();
					udp->read(_dhcpGatewayIp, 4);
					for (int i = 0; i < opt_len-4; i++)
					{
						udp->read();
					}
					break;

				case dns :
					opt_len = udp->read();
					udp->read(_dhcpDnsServerIp, 4);
					for (int i = 0; i < opt_len-4; i++)
						udp->read();
					break;

				case dhcpServerIdentifier :
					opt_len = udp->read();
					if( IPAddress(_dhcpDhcpServerIp) == IPAddress((uint32_t)0) || IPAddress(_dhcpDhcpServerIp) == udp->remoteIP())
					{
						udp->read(_dhcpDhcpServerIp, sizeof(_dhcpDhcpServerIp));
					}
					else
					{
						// Skip over the rest of this option
						while (opt_len--)
							udp->read();
					}
					break;

				case dhcpT1value :
					opt_len = udp->read();
					udp->read((uint8_t*)&_dhcpT1, sizeof(_dhcpT1));
					_dhcpT1 = ntohl(_dhcpT1);
					break;

				case dhcpT2value :
					opt_len = udp->read();
					udp->read((uint8_t*)&_dhcpT2, sizeof(_dhcpT2));
					_dhcpT2 = ntohl(_dhcpT2);
					break;

				case dhcpIPaddrLeaseTime :
					opt_len = udp->read();
					udp->read((uint8_t*)&_dhcpLeaseTime, sizeof(_dhcpLeaseTime));
					_dhcpLeaseTime = ntohl(_dhcpLeaseTime);
					_renewInSec = _dhcpLeaseTime;
					break;

				default :
					opt_len = udp->read();
					// Skip over the rest of this option
					while (opt_len--)
						udp->read();
					break;
				}
			}
		}

		// Need to skip to end of the packet regardless here
		udp->flush();
		return type;
	}

	void printByte(char * buf, uint8_t n )
	{
		char *str = &buf[1];
		buf[0]='0';
		do {
			unsigned long m = n;
			n /= 16;
			char c = m - 16 * n;
			*str-- = c < 10 ? c + '0' : c + 'A' - 10;
		} while(n);
	}
};

#endif
