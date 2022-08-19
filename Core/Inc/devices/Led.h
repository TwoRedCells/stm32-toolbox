/**
 * \file       devices/Led.h
 * \class      Led
 * \brief      An LED interface.
 */

#ifndef INC_DEVICES_LED_H_
#define INC_DEVICES_LED_H_

#include <stdint.h>
#include "hal/hal.h"


class Led
{
public:
    /**
     * Initializes the class to communicate with the hardware.
     * @param port The GPIO port.
     * @param pin The GPIO pin.
     * @param inverted Whether the meaning of the pin is inverted.
     */
	void setup(GPIO_TypeDef* port, uint16_t pin, bool inverted=false)
	{
		this->port = port;
		this->pin = pin;
		this->inverted = inverted;
	}


	/**
	 * Sets the LED to the specified state.
	 * @param state The desired state.
	 */
	void set(bool state)
	{
		HAL_GPIO_WritePin(port, pin, (GPIO_PinState)(inverted ? !state : state));
	}


	/**
	 * Gets the current state.
	 * @return The current state.
	 */
	bool get(void)
	{
        bool ret = HAL_GPIO_ReadPin(port, pin);
        return inverted ? !ret : ret;
	}


	/**
	 * Turns the LED on.
	 */
	void on(void)
	{
		set(true);
	}


	/**
	 * Turns the LED off.
	 */
	void off(void)
	{
		set(false);
	}

protected:
	GPIO_TypeDef *port;
	uint16_t pin;
	bool inverted;
};

#endif /* INC_DEVICES_RELAY_H_ */
