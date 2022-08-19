/**
 * \file       comms/Serial.h
 * \class      Serial
 * \brief      Encapsulates communications over the serial UART RS232C interface.
 */

#ifndef INC_COMMS_SERIAL_HPP_
#define INC_COMMS_SERIAL_HPP_

#include <cmsis_os.h>
#include <utility/Print.hpp>
#include "diagnostics/Fault.h"
#include "hal/hal.h"

class Serial : public Print
{
public:
    /**
     * Initializes the serial interface.
     * @param handle Handle to the hardware interface.
     * @return True on success; otherwise failure.
     */
	bool setup(UART_HandleTypeDef *handle)
	{
		this->handle = handle;
		handle->Instance = USART1;
		handle->Init.BaudRate = 57600;
		handle->Init.WordLength = UART_WORDLENGTH_8B;
		handle->Init.StopBits = UART_STOPBITS_1;
		handle->Init.Parity = UART_PARITY_NONE;
		handle->Init.Mode = UART_MODE_TX_RX;
		handle->Init.HwFlowCtl = UART_HWCONTROL_NONE;
		handle->Init.OverSampling = UART_OVERSAMPLING_16;

		osMutexAttr_t log_mutex_attr;
		mutex = osMutexNew(&log_mutex_attr);
		osMutexAcquire(mutex, osWaitForever);

		if (HAL_UART_Init(handle) != HAL_OK)
			return false;

		osMutexRelease(mutex);
		return true;
	}


	/**
	 * Writes a byte to the port.
	 * @param c The byte.
	 */
	void write(uint8_t c)
	{
		osMutexAcquire(mutex, osWaitForever);
		HAL_UART_Transmit(handle, &c, 1, HAL_MAX_DELAY);
		osMutexRelease(mutex);
	}

	void end(void)
	{
		// For compatibility.
	}

	void flush(void)
	{
		// The FIFO automatically empties on STM32.
	}

private:
	UART_HandleTypeDef *handle;
    osMutexId_t mutex;
} ;



#endif /* INC_COMMS_SERIAL_HPP_ */
