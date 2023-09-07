///	@file       graphics/widgets/FilledRectangle.h
///	@class      FilledRectangle
///	@brief      A filled 4-sided polygon having horizontal and vertical sides.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef LIB_STM32_TOOLBOX_GRAPHICS_FILLEDRECTANGLE_H_
#define LIB_STM32_TOOLBOX_GRAPHICS_FILLEDRECTANGLE_H_

#include <stdint.h>
#include "IWidget.h"
#include "IOutline.h"
#include "IFill.h"
#include "Line.h"
#include "math.h"
#include "Rectangle.h"


template <class TColour>
class FilledRectangle : public Rectangle<TColour>, public IFill<TColour>
{


public:
	FilledRectangle(void)
	{}
	/**
	 * Constructs a rectangle.
	 * @param a The first coordinate parameter.
	 * @param b The second coordinate parameter.
	 * @param c The third coordinate parameter.
	 * @param d The fourth coordinate parameter.
	 * @param c The colour.
	 */
	FilledRectangle(RectangleType type, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t thickness, TColour outline_colour, TColour fill_colour)
	: Rectangle<TColour>(type, a, b, c, d, thickness, outline_colour)
	{
		this->fill_colour = fill_colour;
	}


	/**
	 * Renders this widget on a canvas.
	 * @param canvas The canvas to paint on.
	 */
	void render(IPaintable<TColour>* surface) override
	{
		if (this->enabled)
			render(surface, this->x1, this->y1, this->x2, this->y2, this->thickness, this->outline_colour, this->fill_colour);
	}

	static void render(IPaintable<TColour>* surface, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t thickness, TColour outline_colour, TColour fill_colour)
	{
		Rectangle<TColour>::render(surface, x1, y1, x2, y2, thickness, outline_colour);
		for (uint32_t y=y1+thickness; y<y2-thickness+1; y++)
			Line<TColour>::render(surface, x1+thickness, y, x2-thickness, y, fill_colour);
	}


	/**
	 * Sets the outline colour.
	 * @param colour The outline colour.
	 */
	void set_outline_colour(TColour colour)
	{
		this->outline_colour = colour;
	}


	/**
	 * Sets the outline colour.
	 * @param colour The outline colour.
	 */
	void set_fill_colour(TColour colour)
	{
		this->fill_colour = colour;
	}
};

#endif /* LIB_STM32_TOOLBOX_GRAPHICS_RECTANGLE_H_ */
