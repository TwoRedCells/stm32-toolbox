///	@file       devices/basic/Gpio.h
///	@class      Gpio
///	@brief      Abstracts interaction with a GPIO.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_DEVICES_GPIO_H_
#define INC_DEVICES_GPIO_H_


/// <summary>
/// Abstracts interaction with a GPIO.
/// </summary>
class Gpio
{
public:
	/// <summary>
	/// Instantiates an Led instance and initializes the class to communicate with the hardware.
	/// </summary>
	/// <param name="port">The GPIO port.</param>
	/// <param name="pin">The GPIO pin.</param>
	/// <param name="inverted">Whether the meaning of the pin is inverted, typically when a pin is connected to the anode.</param>
	Gpio(GPIO_TypeDef* port, uint16_t pin, bool inverted=false)
	{
		this->port = port;
		this->pin = pin;
		this->inverted = inverted;
	}


	/// <summary>
	/// Sets the GPIO to the specified state.
	/// </summary>
	/// <param name="state">The desired state.</param>
	void set(bool state)
	{
		HAL_GPIO_WritePin(port, pin, (GPIO_PinState)(inverted ? !state : state));
	}


	/// <summary>
	/// Gets the current state.
	/// </summary>
	/// <returns>The current state.</returns>
	bool get(void)
	{
        bool state = HAL_GPIO_ReadPin(port, pin);
        return inverted ? !state : state;
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
		bool state = get();
		set (!state);
	}


protected:
	GPIO_TypeDef *port;
	uint16_t pin;
	bool inverted;
};

#endif /* INC_DEVICES_RELAY_H_ */
