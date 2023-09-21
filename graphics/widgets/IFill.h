///	@file       graphics/widgets/IFill.h
///	@class      IFill
///	@brief      IOutline for a widget having a fill colour.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE

#ifndef LIB_STM32_TOOLBOX_GRAPHICS_WIDGETS_IFILL_H_
#define LIB_STM32_TOOLBOX_GRAPHICS_WIDGETS_IFILL_H_

template <class TColour>
class IFill
{
public:
	/**
	 * Sets the fill colour.
	 * @param colour The fill colour.
	 */
	void set_fill_colour(TColour colour)
	{
		fill_colour = colour;
	}


protected:
	TColour fill_colour;
};

#endif /* LIB_STM32_TOOLBOX_GRAPHICS_WIDGETS_IOUTLINE_H_ */
