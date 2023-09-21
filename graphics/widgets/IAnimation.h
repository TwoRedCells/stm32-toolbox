///	@file       graphics/widgets/IAnimation.h
///	@class      IAnimation
///	@brief      An animated widget.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE

#ifndef LIB_STM32_TOOLBOX_GRAPHICS_WIDGETS_IANIMATED_H_
#define LIB_STM32_TOOLBOX_GRAPHICS_WIDGETS_IANIMATED_H_

class IAnimation
{
protected:
	virtual void first(void) = 0;
	virtual void next(void) = 0;
	uint32_t frames;
};

#endif /* LIB_STM32_TOOLBOX_GRAPHICS_WIDGETS_IANIMATED_H_ */
