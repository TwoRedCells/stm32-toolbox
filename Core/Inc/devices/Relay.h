/**
 * \file       devices/Relay.h
 * \class      Relay
 * \brief      A relay or contactor with feedback.
 */

#ifndef INC_DEVICES_RELAY_H_
#define INC_DEVICES_RELAY_H_

#include <stdint.h>
#include "hal/hal.h"


class Relay
{
public:
    /**
     * Prepares the class to interface with the relay hardware.
     * @param coil_port The GPIO port that the coil is connected to.
     * @param coil_pin The GPIO pin that the coil is connected to.
     * @param feedback_port The GPIO port that the feedback circuit is connected to.
     * @param feedback_pin The GPIOpin that the feedback circuit is connected to.
     * @param coil_inverted Whether the meaning of the coil pin state is inverted (default is false).
     * @param feedback_inverted Whether the meaning of the feedback pin state is inverted (defaults is false).
     */
	void setup(GPIO_TypeDef* coil_port, uint16_t coil_pin, GPIO_TypeDef* feedback_port, uint16_t feedback_pin, bool coil_inverted=false, bool feedback_inverted=false)
	{
		this->coil_port = coil_port;
		this->coil_pin = coil_pin;
		this->coil_inverted = coil_inverted;
		this->feedback_port = feedback_port;
		this->feedback_pin = feedback_pin;
		this->feedback_inverted = feedback_inverted;
	}


	/**
	 * Sets the relay coil to the specified state.
	 * @param state The desired state.
	 * @return True if the feedback circuit confirms the reacted.
	 */
	bool set(bool state)
	{
		HAL_GPIO_WritePin(coil_port, coil_pin, (GPIO_PinState)(coil_inverted ? !state : state));
		osDelay(25); // Wait for contacts to change state.
		return get();
	}


	/**
	 * Gets the state of the feedback circuit.
	 * @return The state.
	 */
	bool get(void)
	{
        bool ret = HAL_GPIO_ReadPin(feedback_port, feedback_pin);
        return feedback_inverted ? !ret : ret;
	}


	/**
	 * Energizes the relay.
	 * @return True if the feedback circuit reflected the new state.
	 */
	bool on(void)
	{
		return set(true);
	}


    /**
     * Denergizes the relay.
     * @return True if the feedback circuit reflected the new state.
     */
	bool off(void)
	{
		return set(false);
	}

protected:
	GPIO_TypeDef *coil_port;
	uint16_t coil_pin;
	bool coil_inverted;
	GPIO_TypeDef *feedback_port;
	uint16_t feedback_pin;
	bool feedback_inverted;
};

#endif /* INC_DEVICES_RELAY_H_ */
