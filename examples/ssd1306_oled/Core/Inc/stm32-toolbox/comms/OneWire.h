///	@file       comms/OneWire.h
///	@class      OneWire
///	@brief      Encapsulates communications with devices that use the Dallas Semiconductor one-wire protocol.
/// @note       See the DS18B20 datasheet for details how this class is implemented.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_COMMS_ONEWIRE_H_
#define INC_COMMS_ONEWIRE_H_

#include <cmsis_os.h>
#include "stm32-toolbox/constants.h"
#include "stm32-toolbox/utility/Timer.h"


class OneWire
{
public:
    /**
     * Instantiates a one-wire interface using the specified GPIO.
     * @param port The GPIO port to use for communications.
     * @param pin The GPIO pin to use for communications.
     */
	OneWire(GPIO_TypeDef *port, uint16_t pin)
	{
		this->port = port;
		this->pin = pin;
		high();
	}


	/*
	 * During the initialization sequence the bus master transmits (Tx) the
	 * reset pulse by pulling the 1-Wire bus low for a minimum of 480µs. The
	 * bus master then releases the bus and goes into receive mode (Rx). When
	 * the bus is released, the 5kΩ pullup resistor pulls the 1-Wire bus
	 * high. When the DS18S20 detects this rising edge, it waits 15µs to
	 * 60µs and then transmits a presence pulse by pulling the 1-Wire bus
	 * low for 60µs to 240µs.
	 */
	bool reset(void)
	{
		low();
		wait(reset_time);
		high();
		wait(slot_time);  // Allow rise time.
		int32_t elapsed = wait_for(LOW, 240);
		if (elapsed < 0)
			return false;
		wait(reset_time - slot_time - elapsed);
		return true;
	}


	/**
	 * Waits for the specified duration by looping over a no-op.
	 * @param duration The time to wait in milliseconds.
	 */
	void wait(uint32_t duration)
	{
		t.start(duration);
		while (!t.is_elapsed())
		    asm("nop");
	}


	/**
	 * Sets the output high.
	 */
	void high(void)
	{
		HAL_GPIO_WritePin(port, pin, HIGH);
	}


	/**
	 * Sets the output low.
	 */
	void low(void)
	{
		HAL_GPIO_WritePin(port, pin, LOW);
	}


	/**
	 * Writes a bit to the interface.
	 * @param value The bit value.
	 */
	void write_bit(bool value)
	{
		if (value)
		{
			low();
			wait(guard_time);
			high();
			wait(slot_time - guard_time);
		}
		else
		{
			low();
			wait(slot_time - guard_time);
			high();
			wait(guard_time);
		}
	}


	/**
	 * Writes a byte to the interface.
	 * @param byte The byte value.
	 */
	void write_byte(uint8_t byte)
	{
		for(uint8_t i=0; i<8; i++)
		{
			bool bit = byte & (1<<i);
			write_bit(bit);
		}
	}


	/**
	 * Waits for the interface level to reach the specified state.
	 * @param state The state to wait for.
	 * @param timeout The maximum amount of time to wait.
	 * @return The actual time waited.
	 */
	int32_t wait_for(GPIO_PinState state, int32_t timeout)
	{
		int32_t elapsed = 0;

		for(; HAL_GPIO_ReadPin(port, pin) != state; elapsed++)
		{
			wait(microseconds(1));
			if (elapsed > timeout)
				return -1;
		}
		return elapsed;
	}


	/**
	 * Reads a bit from the interface.
	 * @return The bit value.
	 */
	bool read_bit(void)
	{
		low();
		wait(guard_time);
		high();
		wait(read_wait_time);
        //HAL_GPIO_WritePin(READY_LED_GPIO_Port, READY_LED_Pin, LOW);
        bool value = HAL_GPIO_ReadPin(port, pin);
        //HAL_GPIO_WritePin(READY_LED_GPIO_Port, READY_LED_Pin, HIGH);
		wait(slot_time - read_wait_time - guard_time);
		return value;
	}


	/**
	 * Reads a byte from the interface.
	 * @return The byte value.
	 */
	uint8_t read_byte(void)
	{
		uint8_t data = 0;
		// This prevents FreeRTOS from preempting this task, which will throw off the timing
		// and result in an incorrect bit.
        osKernelLock();
		for(uint8_t i=0; i<8; i++)
			data |= (read_bit() << i);
		osKernelUnlock();
		return data;

	}

protected:
	GPIO_TypeDef *port;
	uint16_t pin;

private:
	const uint32_t guard_time = microseconds(1);
	const uint32_t read_wait_time = microseconds(4);
	const uint32_t slot_time = microseconds(64);
	const uint32_t reset_time = microseconds(500);
	Timer t;
};

#endif /* INC_COMMS_ONEWIRE_H_ */
