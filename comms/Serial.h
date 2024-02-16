///	@file       devices/comms/Serial.h
///	@class      Serial
///	@brief      Wrapper for serial communications.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE

/**
 * \file       comms/Serial.h
 * \class      Serial
 * \brief      Encapsulates communications over the serial UART RS232C interface.
 */

#ifndef INC_COMMS_SERIAL_HPP_
#define INC_COMMS_SERIAL_HPP_

#include "utility/PrintLite.h"
#include "toolbox.h"
#include "generics/Queue.h"
#include "utility/IWrite.h"


/**
 * @brief	Wrapper for serial communications.
 */
class Serial : public PrintLite
{
public:
    /**
     * Initializes the serial interface.
     * @param handle Handle to the hardware interface.
     * @return True on success; otherwise failure.
     */
	Serial(UART_HandleTypeDef *handle)
	{
		this->handle = handle;
	}

	Serial(UART_HandleTypeDef *handle, uint8_t* buffer, uint32_t length)
	: queue(buffer, length)
	{
		this->handle = handle;
	}


	/**
	 * Writes a byte to the port.
	 * @param c The byte.
	 */
	size_t write(uint8_t c)
	{

#if SERIAL_USE_DMA_TX
		HAL_UART_Transmit_DMA(handle, &c, 1);

#else
		HAL_UART_Transmit(handle, &c, 1, HAL_MAX_DELAY);
#endif
		return 1;
	}

	/**
	 * Writes bytes from the buffer.
	 * @param buffer Pointer to the memory to write from.
	 * @param length Number of bytes to write.
	 */
	size_t write(void* buffer, uint32_t length)
	{
		HAL_UART_Transmit_IT(handle, (uint8_t*)buffer, length);
		return 1;
	}

	/**
	 * Writes 16-bits to the port in big-endian format.
	 * @param val The value to write.
	 */
	size_t write16(uint16_t val)
	{
		write(val >> 8);
		write(val & 0xff);
		return 2;
	}


	/**
	 * Writes 32-bits to the port in big-endian format.
	 * @param val The value to write.
	 */
	size_t write32(uint32_t val)
	{
		write(val >> 24);
		write(val >> 16);
		write(val >> 8);
		write(val & 0xff);
		return 4;
	}


	/**
	 * Reads bytes from the port.
	 * @param buffer Pointer to the location to store the data.
	 * @param length Number of bytes to read.
	 * @param timeout Number of milliseconds to wait for the buffer to fill.
	 */
	void read(uint8_t* buffer, uint32_t length)
	{
		for(uint32_t i=0; i<length; i++)
			buffer[i] = read();
	}


	uint8_t read(void)
	{
		return queue.dequeue();
	}

	uint32_t available(void)
	{
		return queue.get_length();
	}

	void start(void)
	{
		HAL_UART_Receive_IT(handle, &in, 1);
	}

	void on_rx_interrupt(void)
	{
		queue.enqueue(in);
		HAL_UART_Receive_IT(handle, &in, 1);
		if (in == '\r' && eol_callback != nullptr)
			eol_callback();
	}

	void flush_read(void)
	{
		queue.clear();
	}

	void set_eol_callback(void (*callback)(void))
	{
		eol_callback = callback;
	}

private:
	uint8_t in;
	UART_HandleTypeDef *handle;
	uint8_t* buffer;
	uint32_t length;
	Queue<uint8_t> queue;
	void (*eol_callback)(void) = nullptr;
};

#endif /* INC_COMMS_SERIAL_HPP_ */
