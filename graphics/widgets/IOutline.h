///	@file       graphics/widgets/IOutline.h
///	@class      IMonochrome
///	@brief      IOutline for a widget having an outline colour.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE

#ifndef LIB_STM32_TOOLBOX_GRAPHICS_WIDGETS_IOUTLINE_H_
#define LIB_STM32_TOOLBOX_GRAPHICS_WIDGETS_IOUTLINE_H_

template <class TColour>
class IOutline
{
protected:
	TColour outline_colour;
};

#endif /* LIB_STM32_TOOLBOX_GRAPHICS_WIDGETS_IOUTLINE_H_ */
