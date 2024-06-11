///	@file       comms/tcpip/TelnetServer.h
///	@class      TelnetServer
///	@brief      A mininalist Telnet server.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef LIB_STM32_TOOLBOX_COMMS_TCPIP_TELNETSERVER_H_
#define LIB_STM32_TOOLBOX_COMMS_TCPIP_TELNETSERVER_H_

#include <stdint.h>
#include "comms/ethernet/w5500/TcpServer.h"
#include "comms/tcpip/IPv4Address.h"
#include "generics/Queue.h"


class TelnetServer : public TcpServer
{
public:
	static constexpr uint16_t telnet_port = 23;


	TelnetServer(Socket* socket, uint8_t* buffer, uint32_t length, uint16_t port=telnet_port) : TcpServer(socket, port),
	queue(buffer, length)
	{
	}


	/**
	 * Sets the function to callback when an end-of-line is encountered.
	 * @param	callback	Pointer to the function.
	 */
	void set_eol_callback(void (*callback)(void))
	{
		eol_callback = callback;
	}


	/**
	 * Sets the function to callback when a character is input.
	 * @param	callback	Pointer to the function.
	 */
	void set_input_callback(void (*callback)(uint8_t))
	{
		input_callback = callback;
	}


	/**
	 * Called by an external function when data is received.
	 * @param	value	The byte value to pass into the input buffer.
	 */
	void on_data_received(uint8_t value)
	{
		if (input_callback != nullptr)
			input_callback(value);

		switch (value)
		{
		case '\r':
			queue.enqueue(0);
			if (eol_callback != nullptr)
				eol_callback();
			break;
		case 0x08:  // backspace
		case 0x7f:  // delete
			queue.trim();
			break;
		default:
			queue.enqueue(value);
			break;
		}
	}


	/**
	 * Gets the number of bytes available in the input buffer.
	 * @returns	The number of bytes in the input buffer.
	 */
	size_t available(void) override
	{
		return queue.get_length();
	}


	/**
	 * Determines if data is available in the input buffer.
	 * @returns True if data is buffered; otherwise false.
	 */
	bool is_available(void)
	{
		return TcpServer::available();
	}


	/**
	 * Reads bytes from the port.
	 * @param buffer Pointer to the location to store the data.
	 * @param length Number of bytes to read.
	 * @returns The number of bytes read.
	 */
	size_t read(void* buffer, size_t length) override
	{
		for(uint32_t i=0; i<length; i++)
			((uint8_t*)buffer)[i] = queue.dequeue();
		return length;
	}

	using TcpServer::read;


	/**
	 * Discards all bytes from the input buffer.
	 */
	void purge()
	{
		while (is_available())
			read();
		queue.clear();
	}


private:
	uint16_t port;
	Socket* socket;
	Queue<uint8_t> queue;
	void (*eol_callback)(void) = nullptr;
	void (*input_callback)(uint8_t) = nullptr;
};

#endif /* LIB_STM32_TOOLBOX_COMMS_ETHERNET_NTPCLIENT_H_ */
