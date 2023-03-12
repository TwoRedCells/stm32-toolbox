// Based on code by Blake Foster <blfoster@vassar.edu>

#ifndef INC_STM32_TOOLBOX_COMMS_ETHERNET_PING_H_
#define INC_STM32_TOOLBOX_COMMS_ETHERNET_PING_H_

#include <stdint.h>
#include "Ethernet.h"
#include "stm32-toolbox/utility/Timer.h"
#include "w5500.h"

#define REQ_DATASIZE 64
#define ICMP_ECHOREPLY 0
#define ICMP_ECHOREQ 8
#define ICMP_ECHOREP 0
#define TIME_EXCEEDED 11
#define PING_TIMEOUT 1000

#ifdef ICMPPING_INSERT_YIELDS
#define ICMPPING_DOYIELD()		delay(2)
#else
#define ICMPPING_DOYIELD()
#endif

namespace Ping
{

// ICMPPING_ASYNCH_ENABLE -- define this to enable asynch operations
//#define ICMPPING_ASYNCH_ENABLE

// ICMPPING_INSERT_YIELDS -- some platforms, such as ESP8266, like
// (read: need) to do background work so control must be yielded
// back to the main system periodically when you are doing something
// that takes a good while.
// Define (uncomment the following line) on these platforms, which
// will call a short delay() at critical junctures.
// #define ICMPPING_INSERT_YIELDS

typedef unsigned long icmp_time_t;


typedef enum
{
	/*
    Indicates whether a ping succeeded or failed due to one of various error
    conditions. These correspond to error conditions that occur in this
    library, not anything defined in the ICMP protocol.
	 */
	SUCCESS = 0,
	SEND_TIMEOUT = 1, // Timed out sending the request
	NO_RESPONSE = 2, // Died waiting for a response
	BAD_RESPONSE = 3, // we got back the wrong type
	ASYNC_SENT = 4
} Status;


struct ICMPHeader
{
	/*
    Header for an ICMP packet. Does not include the IP header.
	 */
	uint8_t type;
	uint8_t code;
	uint16_t checksum;
};


struct ICMPEcho
{
	/*
    Contents of an ICMP echo packet, including the ICMP header. Does not
    include the IP header.
	 */


	/*
    This constructor sets all fields and calculates the checksum. It is used
    to create ICMP packet data when we send a request.
    @param type: ICMP_ECHOREQ or ICMP_ECHOREP.
    @param _id: Some arbitrary id. Usually set once per process.
    @param _seq: The sequence number. Usually started at zero and incremented
    once per request.
    @param payload: An arbitrary chunk of data that we expect to get back in
    the response.
	 */
	ICMPEcho(uint8_t type, uint16_t _id, uint16_t _seq, uint8_t * _payload)
	: seq(_seq), id(_id), time(millis())
	{
		memcpy(payload, _payload, REQ_DATASIZE);
		icmpHeader.type = type;
		icmpHeader.code = 0;
		icmpHeader.checksum = _checksum(*this);
	}

	/*
    This constructor leaves everything zero. This is used when we receive a
    response, since we nuke whatever is here already when we copy the packet
    data out of the W5100.
	 */
	ICMPEcho()
	: seq(0), id(0), time(0)
	{
		memset(payload, 0, sizeof(payload));
		icmpHeader.code = 0;
		icmpHeader.type = 0;
		icmpHeader.checksum = 0;
	}

	/*
    Serialize the header as a byte array, in big endian format.
	 */

	ICMPHeader icmpHeader;
	uint16_t id;
	uint16_t seq;
	icmp_time_t time;
	uint8_t payload [REQ_DATASIZE];


	/*
    Serialize the header as a byte array, in big endian format.
	 */
	void serialize(uint8_t * binData) const
	{
		*(binData++) = icmpHeader.type;
		*(binData++) = icmpHeader.code;

		*(uint16_t *)binData = htons(icmpHeader.checksum); binData += 2;
		*(uint16_t *)binData = htons(id);                  binData += 2;
		*(uint16_t *)binData = htons(seq);                 binData += 2;
		*(icmp_time_t *)  binData = htonl(time);                binData += 4;

		memcpy(binData, payload, sizeof(payload));
	}

	/*
    Serialize the header as a byte array, in big endian format.
	 */
	void deserialize(uint8_t const * binData)
	{
		icmpHeader.type = *(binData++);
		icmpHeader.code = *(binData++);

		icmpHeader.checksum = ntohs(*(uint16_t *)binData); binData += 2;
		id                  = ntohs(*(uint16_t *)binData); binData += 2;
		seq                 = ntohs(*(uint16_t *)binData); binData += 2;

		if (icmpHeader.type != TIME_EXCEEDED)
		{
			time = ntohl(*(icmp_time_t *)binData);   binData += 4;
		}

		memcpy(payload, binData, sizeof(payload));
	}

private:
	uint16_t _checksum(const ICMPEcho& echo)
	{
		// calculate the checksum of an ICMPEcho with all fields but icmpHeader.checksum populated
		unsigned long sum = 0;

		// add the header, bytes reversed since we're using little-endian arithmetic.
		sum += _makeUint16(echo.icmpHeader.type, echo.icmpHeader.code);

		// add id and sequence
		sum += echo.id + echo.seq;

		// add time, one half at a time.
		uint16_t const * time = (uint16_t const *)&echo.time;
		sum += *time + *(time + 1);

		// add the payload
		for (uint8_t const * b = echo.payload; b < echo.payload + sizeof(echo.payload); b+=2)
		{
			sum += _makeUint16(*b, *(b + 1));
		}

		// ones complement of ones complement sum
		sum = (sum >> 16) + (sum & 0xffff);
		sum += (sum >> 16);
		return ~sum;
	}

	inline uint16_t _makeUint16(const uint8_t& highOrder, const uint8_t& lowOrder)
	{
		// make a 16-bit unsigned integer given the low order and high order bytes.
		// lowOrder first because the Arduino is little endian.
		uint8_t value [] = {lowOrder, highOrder};
		return *(uint16_t *)&value;
	}
};


struct ICMPEchoReply
{
	/*
    Struct returned by ICMPPing().
    @param data: The packet data, including the ICMP header.
    @param ttl: Time to live
    @param status: SUCCESS if the ping succeeded. One of various error codes
    if it failed.
    @param addr: The ip address that we received the response from. Something
    is borked if this doesn't match the IP address we pinged.
	 */
	ICMPEcho data;
	uint8_t ttl;
	Status status;
	IPAddress addr;
};


class ICMPPing
{
	/*
    Function-object for sending ICMP ping requests.
	 */

public:
	/*
    Construct an ICMP ping object.
    @param socket: The socket number in the W5100.
    @param id: The id to put in the ping packets. Can be pretty much any
    arbitrary number.
	 */
	ICMPPing(SOCKET socket, uint8_t id) :
#ifdef ICMPPING_ASYNCH_ENABLE
		_curSeq(0), _numRetries(0), _asyncstart(0), _asyncstatus(BAD_RESPONSE),
#endif
		_id(id), _nextSeq(0), _socket(socket),  _attempt(0)
	{
		memset(_payload, 0x1A, REQ_DATASIZE);
	}


	/*
     Fetch the current setting for ping timeouts (in ms).
     @return: timeout for all ICMPPing requests, in milliseconds.
	 */
	static uint16_t timeout() { return ping_timeout;}

	/*
    This overloaded version of the () operator takes a (hopefully blank)
    ICMPEchoReply as parameter instead of constructing one internally and
    then copying it on return. This creates a very small improvement in
    efficiency at the cost of making your code uglier.
    @param addr: IP address to ping, as an array of four octets.
    @param nRetries: Number of times to rety before giving up.
    @param result: ICMPEchoReply that will hold the result.
	 */
	void operator()(const IPAddress& addr, int nRetries, ICMPEchoReply& result)
	{
		openSocket();

		ICMPEcho echoReq(ICMP_ECHOREQ, _id, _nextSeq++, _payload);

		for (_attempt=0; _attempt<nRetries; ++_attempt)
		{

			ICMPPING_DOYIELD();

			result.status = sendEchoRequest(addr, echoReq);
			if (result.status == SUCCESS)
			{
				uint8_t replyAddr [4];
				ICMPPING_DOYIELD();
				receiveEchoReply(echoReq, addr, result);
			}
			if (result.status == SUCCESS)
			{
				break;
			}
		}

		w5500.execCmdSn(_socket, Sock_CLOSE);
		w5500.writeSnIR(_socket, 0xFF);
	}

	/*
    Pings the given IP address.
    @param addr: IP address to ping, as an array of four octets.
    @param nRetries: Number of times to rety before giving up.
    @return: An ICMPEchoReply containing the response. The status field in
    the return value indicates whether the echo request succeeded or
    failed. If the request failed, the status indicates the reason for
    failure on the last retry.
	 */
	ICMPEchoReply operator()(const IPAddress& addr, int nRetries)
	{
		ICMPEchoReply reply;
		operator()(addr, nRetries, reply);
		return reply;
	}

	/*
     Use setPayload to set custom data for all ICMP packets
     by passing it an array of [REQ_DATASIZE].  E.g.
       uint8_t myPayload[REQ_DATASIZE] = { ... whatever ...};
       ICMPPing ping(pingSocket, (uint16_t)random(0, 255));
       ping.setPayload(myPayload);
       // ... as usual ...
     @param payload: pointer to start of REQ_DATASIZE array of bytes to use as payload
	 */
	void setPayload(uint8_t * payload)
	{
		memcpy(_payload, payload, REQ_DATASIZE);
	}

#ifdef ICMPPING_ASYNCH_ENABLE
	/*
     Asynchronous ping methods -- only enabled if ICMPPING_ASYNCH_ENABLE is defined, above.
     These methods are used to start a ping request, go do something else, and
     come back later to check if the results are in. A complete example is in the
     examples directory but the gist of it is E.g.
       // say we're in some function, to simplify things...
       IPAddress pingAddr(74,125,26,147); // ip address to ping
       ICMPPing ping(0, (uint16_t)random(0, 255));
       ICMPEchoReply theResult;
       if (! asyncStart(pingAddr, 3, theResult))
       {
       	   // well, this didn't start off on the right foot
       	   Serial.print("Echo request send failed; ");
       	   Serial.println((int)theResult.status);
       	   //
       	   return; // forget about this
       }
       // ok, ping has started...
       while (! ping.asyncComplete(theResult)) {
       	   // whatever needs handling while we wait on results
       	   doSomeStuff();
       	   doSomeOtherStuff();
       	   delay(30);
       }
       // we get here means we either got a response, or timed out...
       if (theResult.status == SUCCESS)
       {
       	   // yay... do something.
       } else {
       	   // boooo... do something else.
       }
       return;
	 */
#endif

private:


	/*
     Control the ping timeout (ms).  Defaults to PING_TIMEOUT (1000ms) but can
     be set using setTimeout(MS).
     @param timeout_ms: Timeout for ping replies, in milliseconds.
     @note: this value is static -- i.e. system-wide for all ICMPPing objects.
	 */
	static void setTimeout(uint16_t setTo)
	{
		ping_timeout = setTo;
	}


	void openSocket()
	{
		w5500.execCmdSn(_socket, Sock_CLOSE);
		w5500.writeSnIR(_socket, 0xFF);
		w5500.writeSnMR(_socket, SnMR::IPRAW);
		w5500.writeSnPROTO(_socket, IPPROTO::ICMP);
		w5500.writeSnPORT(_socket, 0);
		w5500.execCmdSn(_socket, Sock_OPEN);
	}




	Status sendEchoRequest(const IPAddress& addr, const ICMPEcho& echoReq)
	{
		// I wish there were a better way of doing this, but if we use the uint32_t
		// cast operator, we're forced to (1) cast away the constness, and (2) deal
		// with an endianness nightmare.
		uint8_t addri [] = {addr[0], addr[1], addr[2], addr[3]};
		w5500.writeSnDIPR(_socket, addri);
		w5500.writeSnTTL(_socket, 128);
		// The port isn't used, becuause ICMP is a network-layer protocol. So we
		// write zero. This probably isn't actually necessary.
		w5500.writeSnDPORT(_socket, 0);

		uint8_t serialized [sizeof(ICMPEcho)];
		echoReq.serialize(serialized);

		w5500.send_data_processing(_socket, serialized, sizeof(ICMPEcho));
		w5500.execCmdSn(_socket, Sock_SEND);

		while ((w5500.readSnIR(_socket) & SnIR::SEND_OK) != SnIR::SEND_OK)
		{
			if (w5500.readSnIR(_socket) & SnIR::TIMEOUT)
			{
				w5500.writeSnIR(_socket, (SnIR::SEND_OK | SnIR::TIMEOUT));
				return SEND_TIMEOUT;
			}

			ICMPPING_DOYIELD();
		}
		w5500.writeSnIR(_socket, SnIR::SEND_OK);
		return Status::SUCCESS;
	}


	void receiveEchoReply(const ICMPEcho& echoReq, const IPAddress& addr, ICMPEchoReply& echoReply)
	{
		icmp_time_t start = millis();
		while (millis() - start < ping_timeout)
		{

			if (w5500.getRXReceivedSize(_socket) < 1)
			{
				// take a break, maybe let platform do
				// some background work (like on ESP8266)
				ICMPPING_DOYIELD();
				continue;
			}

			// ah! we did receive something... check it out.

			uint8_t ipHeader[6];
			uint8_t buffer = w5500.readSnRX_RD(_socket);
			w5500.read_data(_socket, (uint16_t) buffer, ipHeader, sizeof(ipHeader));
			buffer += sizeof(ipHeader);
			for (int i = 0; i < 4; ++i)
				echoReply.addr[i] = ipHeader[i];
			uint8_t dataLen = ipHeader[4];
			dataLen = (dataLen << 8) + ipHeader[5];

			uint8_t serialized[sizeof(ICMPEcho)];
			if (dataLen > sizeof(ICMPEcho))
				dataLen = sizeof(ICMPEcho);
			w5500.read_data(_socket, (uint16_t) buffer, serialized, dataLen);
			echoReply.data.deserialize(serialized);

			buffer += dataLen;
			w5500.writeSnRX_RD(_socket, buffer);
			w5500.execCmdSn(_socket, Sock_RECV);

			echoReply.ttl = w5500.readSnTTL(_socket);

			// Since there aren't any ports in ICMP, we need to manually inspect the response
			// to see if it originated from the request we sent out.
			switch (echoReply.data.icmpHeader.type) {
			case ICMP_ECHOREP: {
				if (echoReply.data.id == echoReq.id
						&& echoReply.data.seq == echoReq.seq) {
					echoReply.status = Status::SUCCESS;
					return;
				}
				break;
			}
			case TIME_EXCEEDED: {
				uint8_t * sourceIpHeader = echoReply.data.payload;
				unsigned int ipHeaderSize = (sourceIpHeader[0] & 0x0F) * 4u;
				uint8_t * sourceIcmpHeader = echoReply.data.payload + ipHeaderSize;

				// The destination ip address in the originating packet's IP header.
				IPAddress sourceDestAddress(sourceIpHeader + ipHeaderSize - 4);

				if (!(sourceDestAddress == addr))
					continue;

				uint16_t sourceId = ntohs(*(uint16_t * )(sourceIcmpHeader + 4));
				uint16_t sourceSeq = ntohs(*(uint16_t * )(sourceIcmpHeader + 6));

				if (sourceId == echoReq.id && sourceSeq == echoReq.seq) {
					echoReply.status = BAD_RESPONSE;
					return;
				}
				break;
			}
			}


		}
		echoReply.status = NO_RESPONSE;
	}



#ifdef ICMPPING_ASYNCH_ENABLE
	/*
	 * When ICMPPING_ASYNCH_ENABLE is defined, we have access to the
	 * asyncStart()/asyncComplete() methods from the API.
	 */
	bool asyncSend(ICMPEchoReply& result)
	{
		ICMPEcho echoReq(ICMP_ECHOREQ, _id, _curSeq, _payload);

		Status sendOpResult(NO_RESPONSE);
		bool sendSuccess = false;
		for (uint8_t i=_attempt; i<_numRetries; ++i)
		{
			_attempt++;

			ICMPPING_DOYIELD();
			sendOpResult = sendEchoRequest(_addr, echoReq);
			if (sendOpResult == SUCCESS)
			{
				sendSuccess = true; // it worked
				sendOpResult = ASYNC_SENT; // we're doing this async-style, force the status
				_asyncstart = millis(); // not the start time, for timeouts
				break; // break out of this loop, 'cause we're done.

			}
		}
		_asyncstatus = sendOpResult; // keep track of this, in case the ICMPEchoReply isn't re-used
		result.status = _asyncstatus; // set the result, in case the ICMPEchoReply is checked
		return sendSuccess; // return success of send op
	}


	/*
     asyncStart -- begins a new ping request, asynchronously.  Parameters are the
     same as for regular ping, but the method returns false on error.
     @param addr: IP address to ping, as an array of four octets.
     @param nRetries: Number of times to rety before giving up.
     @param result: ICMPEchoReply that will hold a status == ASYNC_SENT on success.
     @return: true on async request sent, false otherwise.
     @author: Pat Deegan, http://psychogenic.com
	 */
	bool asyncStart(const IPAddress& addr, int nRetries, ICMPEchoReply& result)
	{
		openSocket();

		// stash our state, so we can access
		// in asynchSend()/asyncComplete()
		_numRetries = nRetries;
		_attempt = 0;
		_curSeq = _nextSeq++;
		_addr = addr;

		return asyncSend(result);

	}

	/*
     asyncComplete --  check if the asynchronous ping is done.
     This can be either because of a successful outcome (reply received)
     or because of an error/timeout.
     @param result: ICMPEchoReply that will hold the result.
     @return: true if the result ICMPEchoReply contains the status/other data,
              false if we're still waiting for it to complete.
     @author: Pat Deegan, http://psychogenic.com
	 */
	bool asyncComplete(ICMPEchoReply& result)
	{
		if (_asyncstatus != ASYNC_SENT)
		{
			// we either:
			//  - didn't start an async request;
			//	- failed to send; or
			//	- are no longer waiting on this packet.
			// either way, we're done
			return true;
		}

		if (w5500.getRXReceivedSize(_socket))
		{
			// ooooh, we've got a pending reply
			ICMPEcho echoReq(ICMP_ECHOREQ, _id, _curSeq, _payload);
			receiveEchoReply(echoReq, _addr, result);
			_asyncstatus = result.status; // make note of this status, whatever it is.
			return true; // whatever the result of the receiveEchoReply(), the async op is done.
		}

		// nothing yet... check if we've timed out
		if ( (millis() - _asyncstart) > ping_timeout)
		{

			// yep, we've timed out...
			if (_attempt < _numRetries)
			{
				// still, this wasn't our last attempt, let's try again
				if (asyncSend(result))
				{
					// another send has succeeded
					// we'll wait for that now...
					return false;
				}

				// this send has failed. too bad,
				// we are done.
				return true;
			}

			// we timed out and have no more attempts left...
			// hello?  is anybody out there?
			// guess not:
			result.status = NO_RESPONSE;
			return true;
		}

		// have yet to time out, will wait some more:
		return false; // results still not in

	}

#endif	/* ICMPPING_ASYNCH_ENABLE */

	// holds the timeout, in ms, for all objects of this class.
	static inline uint16_t ping_timeout = PING_TIMEOUT;
#ifdef ICMPPING_ASYNCH_ENABLE
	uint8_t _curSeq;
	uint8_t _numRetries;
	icmp_time_t _asyncstart;
	Status _asyncstatus;
	IPAddress	_addr;
#endif
	uint8_t _id;
	uint8_t _nextSeq;
	SOCKET _socket;
	uint8_t _attempt;
	uint8_t _payload[REQ_DATASIZE];
};
}


#endif /* INC_STM32_TOOLBOX_COMMS_ETHERNET_PING_H_ */
