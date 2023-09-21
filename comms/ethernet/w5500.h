/*
 * Copyright (c) 2010 by WIZnet <support@wiznet.co.kr>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef	W5500_H_INCLUDED
#define	W5500_H_INCLUDED

#include "util.h"
#include <stdint.h>
#include "toolbox.h"
#include "utility/Timer.h"


/** Total RAM buffer is 16 kBytes for Transmitter and 16 kBytes for receiver for 1 Socket.
 *  The Total W5500 RAM buffer is 32 kBytes (16 + 16).
 *  If you use more Sockets then the RAM buffer must be split.
 *  For example: if you use 2 Sockets then all socket must use upto 16 kBytes in total RAM.
 *  So, we have: 
 *
 *  #define MAX_SOCK_NUM 2   // Select two Sockets.
 *  #define RXBUF_SIZE   8   // The Receiver buffer size will be 8 kBytes
 *  #define TXBUF_SIZE   8   // The Transmitter buffer size will be 8 kBytes
 *
 *  In total we use (2 Sockets)*(8 kBytes) for transmitter + (2 Sockets)*(8 kBytes) for receiver = 32 kBytes.
 * 
 *  I would prefer to use only 1 Socket with 16 kBytes RAM buffer for transmitter and 16 kByte for receiver buffer.
 *
 *  #define MAX_SOCK_NUM 1   // Select only one Socket
 *  #define RXBUF_SIZE   16  // Select 16 kBytes Receiver RAM buffer
 *  #define TXBUF_SIZE   16  // Select 16 kBytes Transmitter RAM buffer
 * 
 *  (c) 02 September 2015 By Vassilis Serasidis 
 *   cleaned up and extended by stevestrong - 2017.07
 */

typedef uint8_t SOCKET;

#ifndef MAX_SOCK_NUM
#define MAX_SOCK_NUM 8   // Select the number of Sockets (1-8)
#endif

class SnMR {
public:
	static const uint8_t CLOSE  = 0x00;
	static const uint8_t TCP    = 0x01;
	static const uint8_t UDP    = 0x02;
	static const uint8_t IPRAW  = 0x03;
	static const uint8_t MACRAW = 0x04;
	static const uint8_t PPPOE  = 0x05;
	static const uint8_t ND     = 0x20;
	static const uint8_t MULTI  = 0x80;
};

enum SockCMD {
	Sock_OPEN      = 0x01,
	Sock_LISTEN    = 0x02,
	Sock_CONNECT   = 0x04,
	Sock_DISCON    = 0x08,
	Sock_CLOSE     = 0x10,
	Sock_SEND      = 0x20,
	Sock_SEND_MAC  = 0x21,
	Sock_SEND_KEEP = 0x22,
	Sock_RECV      = 0x40
};

class SnIR {
public:
	static const uint8_t SEND_OK = 0x10;
	static const uint8_t TIMEOUT = 0x08;
	static const uint8_t RECV    = 0x04;
	static const uint8_t DISCON  = 0x02;
	static const uint8_t CON     = 0x01;
};

class SnSR {
public:
	static const uint8_t CLOSED      = 0x00;
	static const uint8_t INIT        = 0x13;
	static const uint8_t LISTEN      = 0x14;
	static const uint8_t SYNSENT     = 0x15;
	static const uint8_t SYNRECV     = 0x16;
	static const uint8_t ESTABLISHED = 0x17;
	static const uint8_t FIN_WAIT    = 0x18;
	static const uint8_t CLOSING     = 0x1A;
	static const uint8_t TIME_WAIT   = 0x1B;
	static const uint8_t CLOSE_WAIT  = 0x1C;
	static const uint8_t LAST_ACK    = 0x1D;
	static const uint8_t UDP         = 0x22;
	static const uint8_t IPRAW       = 0x32;
	static const uint8_t MACRAW      = 0x42;
	static const uint8_t PPPOE       = 0x5F;
};

class IPPROTO {
public:
	static const uint8_t IP   = 0;
	static const uint8_t ICMP = 1;
	static const uint8_t IGMP = 2;
	static const uint8_t GGP  = 3;
	static const uint8_t TCP  = 6;
	static const uint8_t PUP  = 12;
	static const uint8_t UDP  = 17;
	static const uint8_t IDP  = 22;
	static const uint8_t ND   = 77;
	static const uint8_t RAW  = 255;
};


class W5500
{
public:

	W5500(SPI_HandleTypeDef & _spi, GPIO_TypeDef* cs_port, uint16_t cs_pin)
	{
		this->hspi = _spi;
		this->cs_port = cs_port;
		this->cs_pin = cs_pin;
		writeMR(0x80); // software reset the W5500 chip
		Timer::Block(milliseconds(100));
	}


	uint8_t read_version(void)
	{
		return readVERSIONR();
	};


	/**
	 * @brief	This function is being used for copy the data form Receive buffer of the chip to application buffer.
	 *
	 * It calculate the actual physical address where one has to read
	 * the data from Receive buffer. Here also take care of the condition while it exceed
	 * the Rx memory uper-bound of socket.
	 */
	void read_data(SOCKET s, volatile uint16_t  src, volatile uint8_t * dst, uint16_t len)
	{
		read((uint16_t)src, (0x18+(s<<5)), (uint8_t *)dst, len);
	}


	/**
	 * @brief	 This function is being called by send() and sendto() function also.
	 *
	 * This function read the Tx write pointer register and after copy the data in buffer update the Tx write pointer
	 * register. User should read upper byte first and lower byte later to get proper value.
	 */
	void send_data_processing(SOCKET s, const uint8_t *data, uint16_t len)
	{
		send_data_processing_offset(s, 0, data, len);
	}


	/**
	 * @brief A copy of send_data_processing that uses the provided ptr for the
	 *        write offset.  Only needed for the "streaming" UDP API, where
	 *        a single UDP packet is built up over a number of calls to
	 *        send_data_processing_ptr, because TX_WR doesn't seem to get updated
	 *        correctly in those scenarios
	 * @param ptr value to use in place of TX_WR.  If 0, then the value is read
	 *        in from TX_WR
	 * @return New value for ptr, to be used in the next call
	 */
	void send_data_processing_offset(SOCKET s, uint16_t data_offset, const uint8_t *data, uint16_t len)
	{
		uint16_t ptr = readSnTX_WR(s);
		uint8_t cntl_byte = (0x14 + (s << 5));
		ptr += data_offset;
		write(ptr, cntl_byte, data, len);
		ptr += len;
		writeSnTX_WR(s, ptr);
	}


	/**
	 * @brief	This function is being called by recv() also.
	 *
	 * This function read the Rx read pointer register
	 * and after copy the data from receive buffer update the Rx write pointer register.
	 * User should read upper byte first and lower byte later to get proper value.
	 */
	void recv_data_processing(SOCKET s, uint8_t *data, uint16_t len, uint8_t peek=0)
	{
		uint16_t ptr = readSnRX_RD(s);
		read_data(s, ptr, data, len);
		if (!peek)
		{
			ptr += len;
			writeSnRX_RD(s, ptr);
		}
	}

	void set_gateway_ip(uint8_t* addr)  { writeGAR(addr); }
	void get_gateway_ip(uint8_t* addr)  { readGAR(addr); }

	void set_subnet_mask(uint8_t* addr) { writeSUBR(addr); }
	void get_subnet_mask(uint8_t* addr) { readSUBR(addr); }

	void set_mac_address(uint8_t* addr) { writeSHAR(addr); }
	void get_mac_address(uint8_t* addr) { readSHAR(addr); }

	void set_ip_address(uint8_t* addr)  { writeSIPR(addr); }
	void get_ip_address(uint8_t* addr)  { readSIPR(addr); }

	void set_retransmission_time(uint16_t timeout) { writeRTR(timeout); }
	void set_retransmission_count(uint8_t retry)  { writeRCR(retry); }

	void set_phy_config(uint8_t val) { writePHYCFGR(val); }
	uint8_t get_phy_config() { return read(0x002E, 0x00); }


	void execute_command(SOCKET s, SockCMD _cmd)
	{
		writeSnCR(s, _cmd);
		// Wait for command to complete
		while (readSnCR(s));
	}


	uint16_t get_tx_free_size(SOCKET s)
	{
		uint16_t val = 0, val1;
		do {
			val1 = readSnTX_FSR(s);
			if (val1 != 0)
				val = readSnTX_FSR(s);
		} while (val != val1);
		return val;
	}


	uint16_t get_rx_received_size(SOCKET s)
	{
		uint16_t val = 0, val1;
		do {
			val1 = readSnRX_RSR(s);
			if (val1 != 0)
				val = readSnRX_RSR(s);
		} while (val != val1);
		return val;
	}


	/**
	 * Sends 8-bit data over the SPI peripheral.
	 * @param spi SPI peripheral handle.
	 * @param data The data to send.
	 */
	void spi_transmit8(SPI_HandleTypeDef spi, uint8_t data)
	{
		spi_transmit_buf(spi, &data, 1);
	}


	/**
	 * Sends 16-bit data over the SPI peripheral.
	 * @param spi SPI peripheral handle.
	 * @param data The data to send.
	 */
	void spi_transmit16(SPI_HandleTypeDef spi, uint16_t data)
	{
		spi_transmit8(spi, (uint8_t)((data >> 8) & 0xFF));
		spi_transmit8(spi, (uint8_t)(data & 0xFF));
	}


	/**
	 * Sends a buffer of bytes data over the SPI peripheral.
	 * @param spi SPI peripheral handle.
	 * @param data The data to send.
	 */
	void spi_transmit_buf(SPI_HandleTypeDef spi, uint8_t *data, uint16_t len)
	{
		for (int i = 0; i < len; i++)
			HAL_SPI_Transmit(&spi, &data[i], 1, 100);
	}


	/**
	 * Receives 8-bit data over the SPI peripheral.
	 * @param spi SPI peripheral handle.
	 * @returns The received byte.
	 */
	uint8_t spi_receive8(SPI_HandleTypeDef spi)
	{
		uint8_t data;
		spi_receive_buf(spi, &data, 1);
		return data;
	}


	/**
	 * Receives 16-bit data over the SPI peripheral.
	 * @param spi SPI peripheral handle.
	 * @returns The received word.
	 */
	uint16_t spi_receive16(SPI_HandleTypeDef spi)
	{
		uint16_t dato_recibido;
		dato_recibido = spi_receive8(spi) << 8;
		dato_recibido |= spi_receive8(spi);
		return dato_recibido;
	}


	/**
	 * Receives a buffer of bytes data over the SPI peripheral.
	 * @param spi SPI peripheral handle.
	 * @param data Pointer to store the data.
	 */
	void spi_receive_buf(SPI_HandleTypeDef spi, uint8_t *data, uint16_t _len)
	{
		for(int i = 0; i < _len; i++)
			HAL_SPI_Receive(&spi, &data[i], 1, 100);
	}


private:
	void write(uint16_t _addr, uint8_t _cb, uint8_t _data)
	{
		select_ss();
		spi_transmit16(hspi, _addr);
		spi_transmit16(hspi,(uint16_t) (_cb << 8) | _data);
		deselect_ss();
	}


	void write16(uint16_t _addr, uint8_t _cb, uint16_t _data)
	{
		select_ss();
		spi_transmit16(hspi,_addr);
		spi_transmit8(hspi, _cb);
		spi_transmit16(hspi,_data);
		deselect_ss();
	}


	void write(uint16_t _addr, uint8_t _cb, const uint8_t *_buf, uint16_t _len)
	{
		select_ss();
		spi_transmit16(hspi,_addr);
		spi_transmit8(hspi, _cb);
		spi_transmit_buf(hspi, (uint8_t *)_buf, _len);
		deselect_ss();
	}


	uint8_t read(uint16_t _addr, uint8_t _cb)
	{
		select_ss();
		spi_transmit16(hspi,_addr);
		spi_transmit8(hspi, _cb);
		uint8_t _data = spi_receive8(hspi);
		deselect_ss();
		return _data;
	}


	uint16_t read16(uint16_t _addr, uint8_t _cb)
	{
		select_ss();
		spi_transmit16(hspi,_addr);
		spi_transmit8(hspi, _cb);
		uint16_t _data = spi_receive16(hspi);
		deselect_ss();
		return _data;
	}


	uint16_t read(uint16_t _addr, uint8_t _cb, uint8_t *_buf, uint16_t _len)
	{
		select_ss();
		spi_transmit16(hspi,_addr);
		spi_transmit8(hspi, _cb);
		spi_receive_buf(hspi, &*_buf , _len);
		deselect_ss();
		return _len;
	}

	// W5500 Registers
	// ---------------

#define __GP_REGISTER8(name, address)          \
	void write##name(uint8_t _data) { write(address, 0x04, _data); } \
	uint8_t read##name() { return read(address, 0x00); }

#define __GP_REGISTER16(name, address)         \
	void write##name(uint16_t _data) { write16(address, 0x04, _data); }  \
	uint16_t read##name() { return read16(address, 0x00); }

#define __GP_REGISTER_N(name, address, size)   \
	void write##name(uint8_t *_buff) { write(address, 0x04, _buff, size); }  \
	uint16_t read##name(uint8_t *_buff) { return read(address, 0x00, _buff, size); }

public:
	__GP_REGISTER8 (MR,      0x0000);    // Mode
	__GP_REGISTER_N(GAR,     0x0001, 4); // Gateway IP address
	__GP_REGISTER_N(SUBR,    0x0005, 4); // Subnet mask address
	__GP_REGISTER_N(SHAR,    0x0009, 6); // Source MAC address
	__GP_REGISTER_N(SIPR,    0x000F, 4); // Source IP address
	__GP_REGISTER8 (IR,      0x0015);    // Interrupt
	__GP_REGISTER8 (IMR,     0x0016);    // Interrupt Mask
	__GP_REGISTER16(RTR,     0x0019);    // Timeout address
	__GP_REGISTER8 (RCR,     0x001B);    // Retry count
	__GP_REGISTER8 (PTIMER,  0x001C);    // PPP LCP Request Timer
	__GP_REGISTER8 (PMAGIC,  0x001D);    // PPP LCP Magic Number
	__GP_REGISTER_N(UIPR,    0x0028, 4); // Unreachable IP address in UDP mode
	__GP_REGISTER16(UPORT,   0x002C);    // Unreachable Port address in UDP mode
	__GP_REGISTER8 (PHYCFGR, 0x002E);   // PHY Configuration
	__GP_REGISTER8 (VERSIONR,0x0039);   // Chip version


#undef __GP_REGISTER8
#undef __GP_REGISTER16
#undef __GP_REGISTER_N

	// W5500 Socket registers
	// ----------------------
private:
	uint8_t readSn(SOCKET s, uint16_t addr)    { return read(addr, (s<<5)+0x08); }
	uint16_t readSn16(SOCKET s, uint16_t addr) { return read16(addr, (s<<5)+0x08); }
	uint16_t readSn(SOCKET s, uint16_t addr, uint8_t *buf, uint16_t len) { return read(addr, (s<<5)+0x08, buf, len ); }
	void writeSn(SOCKET s, uint16_t addr, uint8_t data) { write(addr, (s<<5)+0x0C, data); }
	void writeSn16(SOCKET s, uint16_t addr, uint16_t data) { write16(addr, (s<<5)+0x0C, data); }
	void writeSn(SOCKET s, uint16_t addr, uint8_t *buf, uint16_t len) { write(addr, (s<<5)+0x0C, buf, len); }

#define __SOCKET_REGISTER8(name, address)                 \
	void write##name(SOCKET _s, uint8_t _data) { writeSn(_s, address, _data); }  \
	uint8_t read##name(SOCKET _s) { return readSn(_s, address); }

#define __SOCKET_REGISTER16(name, address)                \
	void write##name(SOCKET _s, uint16_t _data) { writeSn16(_s, address, _data); } \
	uint16_t read##name(SOCKET _s) { return readSn16(_s, address);}

#define __SOCKET_REGISTER_N(name, address, size)          \
	void write##name(SOCKET _s, uint8_t *_buff) { writeSn(_s, address, _buff, size); } \
	uint16_t read##name(SOCKET _s, uint8_t *_buff) { return readSn(_s, address, _buff, size);}

public:
	__SOCKET_REGISTER8(SnMR,        0x0000)        // Mode
	__SOCKET_REGISTER8(SnCR,        0x0001)        // Command
	__SOCKET_REGISTER8(SnIR,        0x0002)        // Interrupt
	__SOCKET_REGISTER8(SnSR,        0x0003)        // Status
	__SOCKET_REGISTER16(SnPORT,     0x0004)        // Source Port
	__SOCKET_REGISTER_N(SnDHAR,     0x0006, 6)     // Destination Hardw Addr
	__SOCKET_REGISTER_N(SnDIPR,     0x000C, 4)     // Destination IP Addr
	__SOCKET_REGISTER16(SnDPORT,    0x0010)        // Destination Port
	__SOCKET_REGISTER16(SnMSSR,     0x0012)        // Max Segment Size
	__SOCKET_REGISTER8(SnPROTO,     0x0014)        // Protocol in IP RAW Mode
	__SOCKET_REGISTER8(SnTOS,       0x0015)        // IP TOS
	__SOCKET_REGISTER8(SnTTL,       0x0016)        // IP TTL
	__SOCKET_REGISTER16(SnTX_FSR,   0x0020)        // TX Free Size
	__SOCKET_REGISTER16(SnTX_RD,    0x0022)        // TX Read Pointer
	__SOCKET_REGISTER16(SnTX_WR,    0x0024)        // TX Write Pointer
	__SOCKET_REGISTER16(SnRX_RSR,   0x0026)        // RX Free Size
	__SOCKET_REGISTER16(SnRX_RD,    0x0028)        // RX Read Pointer
	__SOCKET_REGISTER16(SnRX_WR,    0x002A)        // RX Write Pointer (supported?)

#undef __SOCKET_REGISTER8
#undef __SOCKET_REGISTER16
#undef __SOCKET_REGISTER_N

public:
	static const uint16_t SSIZE = 2048; // Max Tx buffer size

private:
	SPI_HandleTypeDef hspi;
	GPIO_TypeDef* cs_port;
	uint16_t cs_pin;

	void select_ss()
	{
		HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
	}

	void deselect_ss()
	{
		HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
	}
};

extern W5500 w5500;

#endif
