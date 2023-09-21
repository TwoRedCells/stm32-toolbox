///	@file       graphics/widgets/Pixel.h
///	@class      Pixel
///	@brief      A point consisting of a single pixel.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef LIB_STM32_TOOLBOX_GRAPHICS_PIXEL_H_
#define LIB_STM32_TOOLBOX_GRAPHICS_PIXEL_H_

#include <graphics/widgets/Container.h>
#include <stdint.h>
#include "IWidget.h"
#include "IMonochrome.h"

template <class TColour>
class Pixel : public IWidget<TColour>, public IMonochrome<TColour>
{
public:
	/**
	 * Constructs a pixel.
	 * @param x The x-coordinate.
	 * @param y The y-coordinate.
	 * @param c The colour.
	 */
	Pixel(uint32_t x, uint32_t y, TColour c)
	{
		this->width = 1;
		this->height = 1;
		this->origin_x = x;
		this->origin_y = y;
		this->colour = c;
	}

	/**
	 * Renders this widget on a canvas.
	 * @param canvas The canvas to paint on.
	 */
	void render(IPaintable<TColour>* surface) override
	{
		if (this->enabled)
			surface->pixel(this->origin_x, this->origin_y, this->colour);
	}

protected:
};

#endif /* LIB_STM32_TOOLBOX_GRAPHICS_PIXEL_H_ */
