///	@file       comms/tcpip/TftpServer.h
///	@class      TftpServer
///	@brief      A mininalist TFTP server.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef LIB_STM32_TOOLBOX_COMMS_TCPIP_TFTPSERVER_H_
#define LIB_STM32_TOOLBOX_COMMS_TCPIP_TFTPSERVER_H_

#include <stdint.h>
#include "comms/ethernet/w5500/Udp.h"
#include "comms/tcpip/IPv4Address.h"
#include "utility/Timer.h"


class TftpServer
{
public:
	static constexpr uint16_t tftp_port = 69;
	static constexpr uint16_t OpcodeReadRequest = 0x01;
	static constexpr uint16_t OpcodeWriteRequest = 0x02;
	static constexpr uint16_t OpcodeData = 0x03;
	static constexpr uint16_t OpcodeAcknowledge = 0x04;
	static constexpr uint16_t OpcodeError = 0x05;

	static constexpr uint16_t ErrorUndefined = 0x00;
	static constexpr uint16_t ErrorFileNotFound = 0x01;
	static constexpr uint16_t ErrorAccessViolation = 0x02;
	static constexpr uint16_t ErrorDiskFull = 0x03;
	static constexpr uint16_t ErrorIllegalOperation = 0x04;
	static constexpr uint16_t ErrorUnknownTransferId = 0x05;
	static constexpr uint16_t ErrorFileExists = 0x06;
	static constexpr uint16_t ErrorNoSuchUser = 0x07;


	/**
	 * @brief	Constructs a TftpServer instance.
	 * @param socket	Pointer to a socket instance.
	 * @param tftp_port	The UDP port to use, 69 per the RFC.
	 */
	TftpServer(Socket* socket, uint16_t port=tftp_port)
	{
		this->socket = socket;
		this->port = port;
	}


	/**
	 * @brief	Configures the server to start receiving packets.
	 */
	bool begin(void)
	{
		return socket->open(SnMR::UDP, port, 0);
	}


	/**
	 * @brief	Sets the data callback function.
	 * @param	data_callback	Pointer to the callback function.
	 */
	void set_data_callback(void (*data_callback)(char*, uint16_t, uint8_t*, uint16_t))
	{
		this->data_callback = data_callback;
	}


	/**
	 * @brief Checks for incoming TFTP packets and handles them. The data callback may be called as required.
	 */
    void poll(void)
    {
    	uint8_t addr[4];
    	uint16_t port;
    	uint16_t length = socket->recvfrom(buffer, 512, addr, &port);

    	if (length == 0)
    	{
    		if (timeout.is_elapsed())
    		{
				timeout.reset();
				error(addr, port, ErrorUndefined, "Timeout exceeded");
				socket->disconnect();
				socket->close();
				state = Closed;
				begin();
    		}
    		return;
    	}

		uint16_t opcode = swap(*(uint16_t*)buffer);
		memcpy(client_ip, addr, 4);
		client_port = port;
		if (opcode == OpcodeWriteRequest) // WRQ / Write request
		{
			state = Open;
			// Copy filename.
			for (uint16_t i=0; i < 80; i++)
			{
				filename[i] = buffer[i+2];
				filename[i+1] = 0;
				if (filename[i] == 0x00)
					break;
			}

			// Respond with acknowledgement.
			ack(0);
			timeout.start(timeout_duration);
			data_callback(filename, 0, nullptr, 0);
		}

//			// If the client IP doesn't match, another host is connecting, which isn't allowed.
//    		if (memcmp(client_ip, addr, 4))
//    		{
//    			error(addr, port, ErrorIllegalOperation, "Another host is connected to this server.");
//    			return;
//    		}

		else if (opcode == OpcodeError)
		{
			state = Closed;
			socket->disconnect();
			socket->close();
			timeout.reset();
			return;
		}

		else if (opcode == OpcodeData)
		{
			uint16_t block_id = swap(*(uint16_t*)(buffer+2));
			data_callback(filename, block_id, buffer+4, length-4);
			timeout.restart();

			// If less than 512B of data, it is the last block.
			if (length-4 < 512)
			{
				state = Closed;
				timeout.reset();
			}
			ack(block_id);
		}

		else
		{
			error(addr, port, ErrorUndefined, "Intention not understood.");
		}
    }


private:
    /**
     * @brief Swaps the bytes of a 16-bit word.
     * @param word	The word to manipulate.
     * @returns	Teh manipulated word.
     */
    uint16_t swap(uint16_t word)
    {
    	return word << 8 | word >> 8;
    }


    /**
     * @brief Acknowledges a block to the client.
     * @param block_id	The block to acknowledge.
     */
    void ack(uint16_t block_id)
    {
    	uint16_t out[2];
    	out[0] = swap(OpcodeAcknowledge);
    	out[1] = swap(block_id);
    	socket->start_udp(client_ip, client_port);
    	socket->bufferData(out, 4);
 //   	socket->send(out, 4);
    	socket->send_udp();
    }


    /**
     * @brief Sends an error to the client.
     * @param ip	The client's IP address.
     * @param port	The client's UDP port.
     * @param error_code	The error code per the RFC.
     * @param error_message	A message further explaining the error.     *
     */
    void error(uint8_t* ip, uint16_t port, uint16_t error_code, const char* error_message)
    {
    	uint16_t length = strlen(error_message)+5;
    	uint8_t out[length] = {0};
    	uint16_t* opcode = (uint16_t*) out+0;
    	*opcode = swap(OpcodeError);
    	uint16_t* block_id = (uint16_t*) out+2;
    	*block_id = swap(error_code);
    	strcpy((char*)out+4, error_message);
    	socket->start_udp(ip, port);
//    	socket->send(&OpcodeError, sizeof(uint16_t));
//    	socket->send(&error_code, sizeof(uint16_t));
//    	socket->send(error_message, strlen(error_message));
    	socket->bufferData(out, length);
//    	socket->send(out, length);
    	socket->send_udp();
    }


private:
	uint16_t port;
	uint8_t client_ip[4];
	uint16_t client_port;
	uint16_t last_block;
	Socket* socket;
	uint8_t buffer[516];
	char filename[81];
	enum { Closed, Open } state = Closed;
	void (*data_callback)(char* filename, uint16_t block_id, uint8_t* data, uint16_t length);
	Timer timeout;
	const uint32_t timeout_duration = milliseconds(Constants::TftpTimeout);
};

#endif /* LIB_STM32_TOOLBOX_COMMS_ETHERNET_NTPCLIENT_H_ */
