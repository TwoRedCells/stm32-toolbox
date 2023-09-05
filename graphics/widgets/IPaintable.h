///	@file       graphics/widgets/IPaintable.h
///	@class      IWidget
///	@brief      Interface for drawing surface.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef LIB_STM32_TOOLBOX_GRAPHICS_IPAINTABLE_H_
#define LIB_STM32_TOOLBOX_GRAPHICS_IPAINTABLE_H_

template <class TColour>
class IPaintable
{
public:
	/**
	 * Paints a pixel on the canvas.
	 * @param x The x-coordinate.
	 * @param y They-coordinate.
	 * @param c The colour.
	 */
	virtual void pixel(uint32_t x, uint32_t y, TColour c)
	{
		// Abstract class. Do nothing.
	}

	/**
	 * Sets a region for fast painting.
	 * @param x The upper-left x-coordinate.
	 * @param y The upper-left y-coordinate.
	 * @param w The width.
	 * @param h The height.
	 */
	virtual void start_region(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
	{
	}


	/**
	 * Ends a fast painting transaction.
	 */
	virtual void end_region(void)
	{
	}

};

#endif
