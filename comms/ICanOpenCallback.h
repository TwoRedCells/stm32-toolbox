///	@file       comms/ITimer.h
///	@class      Timer
///	@brief      A general purpose timer that can be used as a stopwatch or a means to coordinate asynchonous events.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE

#ifndef INC_STM32_TOOLBOX_COMMS_ICANOPENCALLBACK_H_
#define INC_STM32_TOOLBOX_COMMS_ICANOPENCALLBACK_H_


class ICanOpenCallback
{
	virtual void on_sdo(uint16_t address, uint16_t index, uint8_t subindex, uint8_t data)
	{

	}
};



#endif /* INC_STM32_TOOLBOX_COMMS_ICANOPENCALLBACK_H_ */
