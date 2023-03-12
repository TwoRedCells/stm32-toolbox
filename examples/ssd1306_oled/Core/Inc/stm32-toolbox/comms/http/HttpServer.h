/**	@file       comms/HttpServer.h
 *	@class      HttpServer
 *	@brief      A minimalist web server suitable for simple pages or an API.
 *
 *  @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
 *              firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
 *  @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE
 */


#ifndef INC_STM32_TOOLBOX_COMMS_HTTPSERVER_H_
#define INC_STM32_TOOLBOX_COMMS_HTTPSERVER_H_

#include <stm32-toolbox/comms/ethernet/Ethernet.h>
#include "stm32-toolbox/comms/ethernet/Ping.h"
#include "HttpHandler.h"
#include "stm32-toolbox/generics/List.h"
#include "string.h"
#include "globals.h"
#include "strings.h"


/**
 * @brief A minimalist web server suitable for simple pages or an API.
 */
class HttpServer : public EthernetServer
{
public:
	/**
	 * @brief	Instantats the class.
	 * @param	ethernet Pointer to the Ethernet instance.
	 */
	HttpServer(EthernetClass* ethernet) : EthernetServer(80)
	{
		this->ethernet = ethernet;
		handlers.set_buffer(handler_buffer, HTTP_SERVER_MAX_HANDLERS);
		this->begin();
	}


	/**
	 * @brief	Listens for incoming connections.
	 */
	void listen(void)
	{
		uint8_t gw[4];
		uint32_t tick = 0;

		for(;;)
		{
			// Feed watchdog.
			gooddog.feed(HttpdTaskOk);

			// See if we are online.
			if (tick++ % 50 == 0)  // Every 5-10 seconds.
			{
				Ping::ICMPPing ping(4, 0xff);  // 4 is socket number.
				socket.getGatewayIp(gw);
				Ping::ICMPEchoReply echoReply = ping(gw, 1);  // 1 retry
				fault.update(NemoFault::NetworkGatewayOffline, echoReply.status != Ping::SUCCESS);
			}

			// Check for client connection.
			EthernetClient client = this->available();
			if (client)
			{
				handle_request(&client);
				client.print(S_LF);
				client.stop();
			}
			ethernet->maintain();  // Manage DHCP leases, if applicable.
			osDelay(100);
		}
	}


	/**
	 * @brief	Adds the specified handler instance to the list of handlers.
	 * @note	Every incoming request will be passed to the list of handlers. Each handler can opt
	 *          to handle or not handle the request.
	 * @param	handler	The `HttpHandler` instance to add.
	 */
	void register_handler(HttpHandler* handler)
	{
		handlers.add(handler);
	}


	/**
	 * @brief	Called to handle an incoming HTTP request.
	 * @note	Each registered handler will be given an opportunity to handle the request.
	 * @param	client A pointer to the client instance.
	 */
	void handle_request(EthernetClient* client)
	{
		// Read the request from the client e.g. `GET /index.html HTTP/1.1`
		String request = client->readStringUntil('\n');

		// Give each handler the chance to handler this request.
		for (uint32_t i=0; i<handlers.get_length(); i++)
		{
			HttpHandler* handler = handlers.get(i);
			if (handler->can_handle(request))
			{
				uint16_t ret = handler->handle(request, client);
				switch(ret)
				{
				case 400:
					respond_400();
					return;
				case 404:
					respond_404();
					return;
				case 409:
					respond_409();
					return;
				case 500:
					respond_500();
					return;
				case 501:
					respond_501();
					return;
				case 507:
					respond_507();
					return;
				default:
				case 200:
					return;
				}
				return;
			}
		}
	}


	/**
	 * @brief	Returns a generic 400 Bad Request error.
	 */
	void respond_400(void)
	{
		print("HTTP/1.1 400 Bad Request\n");
		print("Content-type: text/plain\n\n");
		print("That request didn't make sense. Please try again.");
	}


	/**
	 * @brief	Returns a generic 404 File Not Found error.
	 */
	void respond_404(void)
	{
		print("HTTP/1.1 404 Not Found\n");
		print("Content-type: text/plain\n\n");
		print("That file or endpoint doesn't exist here.");
	}


	/**
	 * @brief	Returns a generic 409 Conflict.
	 */
	void respond_409(void)
	{
		print("HTTP/1.1 409 Conflict\n");
		print("Content-type: text/plain\n\n");
		print("That file already exists.");
	}


	/**
	 * @brief	Returns a generic 500 Internal Server Error error.
	 */
	void respond_500(void)
	{
		print("HTTP/1.1 500 Internal Server Error\n");
		print("Content-type: text/plain\n\n");
		print("Something broke.");
	}


	/**
	 * @brief	Returns a generic 501 Not Implemented error.
	 */
	void respond_501(void)
	{
		print("HTTP/1.1 501 Not Implemented\n");
		print("Content-type: text/plain\n\n");
		print("This feature has not yet been implemented.");
	}


	/**
	 * @brief	Returns a generic 502 Bad Gateway error.
	 */
	void respond_502(void)
	{
		print("HTTP/1.1 502 Bad Gateway\n");
		print("Content-type: text/plain\n\n");
		print("The server was unable to contact a remote host.");
	}


	/**
	 * @brief	Returns a generic 507 Insufficient Storage error.
	 */
	void respond_507(void)
	{
		print("HTTP/1.1 507 Insufficient Storage\n");
		print("Content-type: text/plain\n\n");
		print("The uploaded file is too large to store.");
	}


private:
	EthernetClass* ethernet;
	HttpHandler* handler_buffer[0x200];
	List<HttpHandler*> handlers;
	static constexpr uint32_t RECV_BUFFER_LENGTH = 0x200;
};

#endif /* INC_STM32_TOOLBOX_COMMS_HTTPSERVER_H_ */
