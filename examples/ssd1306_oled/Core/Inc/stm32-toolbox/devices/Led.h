///	@file       devices/Led.h
///	@class      Led
///	@brief      Abstracts control of an Led, or any output.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_DEVICES_LED_H_
#define INC_DEVICES_LED_H_

#include <stdint.h>
#include "stm32/stm32.h"
#include "stm32-toolbox/utility/Timer.h"

/// <summary>
/// Allows a string to be built with one or more commands similar to writing to a stream.
/// </summary>
class Led
{
public:

	/// <summary>
	/// Instantiates an Led instance.
	/// </summary>
	Led(void)
	{
	}


	/// <summary>
	/// Instantiates an Led instance and initializes the class to communicate with the hardware.
	/// </summary>
	/// <param name="port">The GPIO port.</param>
	/// <param name="pin">The GPIO pin.</param>
	/// <param name="inverted">Whether the meaning of the pin is inverted, typically when a pin is connected to the anode.</param>
	/// <param name="flash_on">The duration that the LED should stay on when flashing, in milliseconds.</param>
	/// <param name="flash_off">The duration that the LED should stay off when flashing, in milliseconds.</param>
	Led(GPIO_TypeDef* port, uint16_t pin, bool inverted=false, uint32_t flash_on=milliseconds(500), uint32_t flash_off=milliseconds(500))
	{
		setup(port, pin, inverted, flash_on, flash_off);
	}


	/// <summary>
	/// Initializes the class to communicate with the hardware.
	/// </summary>
	/// <param name="port">The GPIO port.</param>
	/// <param name="pin">The GPIO pin.</param>
	/// <param name="inverted">Whether the meaning of the pin is inverted, typically when a pin is connected to the anode.</param>
	/// <param name="flash_on">The duration that the LED should stay on when flashing, in milliseconds.</param>
	/// <param name="flash_off">The duration that the LED should stay off when flashing, in milliseconds.</param>
	void setup(GPIO_TypeDef* port, uint16_t pin, bool inverted=false, uint32_t flash_on=milliseconds(500), uint32_t flash_off=milliseconds(500))
	{
		this->port = port;
		this->pin = pin;
		this->inverted = inverted;
		this->flash_on = flash_on;
		this->flash_off = flash_off;
	}


	/// <summary>
	/// Sets the LED to the specified state.
	/// </summary>
	/// <param name="state">The desired state.</param>
	void set(bool state)
	{
		HAL_GPIO_WritePin(port, pin, (GPIO_PinState)(inverted ? !state : state));
		this->state = state;
	}


	/// <summary>
	/// Gets the current state.
	/// </summary>
	/// <returns>The current state.</returns>
	bool get(void)
	{
        return state;
	}


	/// <summary>
	/// Turns the LED on.
	/// </summary>
	void on(void)
	{
		set(true);
	}


	/// <summary>
	/// Turns the LED off.
	/// </summary>
	void off(void)
	{
		set(false);
	}


	/// <summary>
	/// Reverses the current state.
	/// </summary>
	void flip(void)
	{
		set(!state);
	}

	/// <summary>
	/// Starts (or stops) the LED flashing.
	/// </summary>
	/// <param name=start>True to start flashing; otherwise false.</param>
	void flash(bool start=true)
	{
		if (!start)
		{
			timer.reset();
			off();
			return;
		}

		// TODO: NOT IMPLEMENTED
	}


protected:
	GPIO_TypeDef *port;
	uint16_t pin;
	bool inverted;
	Timer timer;
	bool flash_state;
	uint32_t flash_on, flash_off;
	bool state;
};

#endif /* INC_DEVICES_RELAY_H_ */
