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


	/**
	 * Writes a byte to the port.
	 * @param c The byte.
	 */
	size_t write(uint8_t c)
	{
#ifdef FREERTOS_CONFIG_H
//		osMutexAcquire(mutex, osWaitForever);
#endif
#if SERIAL_USE_DMA_TX
		HAL_UART_Transmit_DMA(handle, &c, 1);

#else
		HAL_UART_Transmit(handle, &c, 1, HAL_MAX_DELAY);
#endif
#ifdef FREERTOS_CONFIG_H
//		osMutexRelease(mutex);
#endif
		return 1;
	}

private:
	UART_HandleTypeDef *handle;
#ifdef FREERTOS_CONFIG_H
    osMutexId_t mutex;
#endif
} ;



#endif /* INC_COMMS_SERIAL_HPP_ */
