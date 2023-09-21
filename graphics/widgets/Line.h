///	@file       graphics/widgets/Line.h
///	@class      Line
///	@brief      A line between two points.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef LIB_STM32_TOOLBOX_GRAPHICS_LINE_H_
#define LIB_STM32_TOOLBOX_GRAPHICS_LINE_H_

#include <stdint.h>
#include <math.h>
#include "IWidget.h"


template <class TColour>
class Line : public IWidget<TColour>
{
public:
	/**
	 * Constructs a line.
	 * @param x1 The x-coordinate of the first point
	 * @param y1 The y-coordinate of the first point.
	 * @param x2 The x-coordinate of the second point
	 * @param y2 The y-coordinate of the second point.
	 * @param c The colour.
	 */
	Line(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, TColour c)
	{
		this->x1 = x1 < x2 ? x1 : x2;
		this->y1 = y1 < y2 ? y1 : y2;
		this->x2 = x2;
		this->y2 = y2;
		this->x_origin = this->x1;
		this->y_origin = this->y2;
		this->width = x2 - x1;
		this->height = y2 - y1;
		this->colour = c;
	}

	/**
	 * Renders this widget on a canvas.
	 * @param canvas The canvas to paint on.
	 */
	void render(IPaintable<TColour>* surface) override
	{
		if (this->enabled)
			render(surface, x1, y1, x2, y2, colour);
	}


	/**
	 * Draws a line.
	 * @param surface Pointer to the drawing surface.
	 * @param x1 The x-coordinate of the first point
	 * @param y1 The y-coordinate of the first point.
	 * @param x2 The x-coordinate of the second point
	 * @param y2 The y-coordinate of the second point.
	 * @param colour The colour.
	 */
	static void render(IPaintable<TColour>* surface, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, TColour colour)
	{
		if (y1 == y2)  // Horizontal.
		{
			for(uint32_t x=x1; x<x2+1; x++)
				surface->pixel(x, y1, colour);
		}
		else if (x1 == x2)  // Vertical
		{
			for(uint32_t y=y1; y<y2+1; y++)
				surface->pixel(x1, y, colour);
		}
		else
		{
			float slope=(float)(y2-y1)/(float)(x2-x1);
			for(uint32_t x=x1; x<x2+1; x++)
				surface->pixel(x, x*slope, colour);
		}
	}



	/**
	 * Draws a line by using a region for acceleration.
	 * @param surface Pointer to the drawing surface.
	 * @param x1 The x-coordinate of the first point
	 * @param y1 The y-coordinate of the first point.
	 * @param x2 The x-coordinate of the second point
	 * @param y2 The y-coordinate of the second point.
	 * @param colour The colour.
	 */
	static void render_fast(IPaintable<TColour>* surface, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, TColour colour)
	{
		surface->start_region(x1, y1, x2-x1+1, y2-y1+1);
		if (y1 == y2)  // Horizontal.
		{
			for(uint32_t x=x1; x<x2+1; x++)
				surface->pixel(x, y1, colour);
		}
		else if (x1 == x2)  // Vertical
		{
			for(uint32_t y=y1; y<y2+1; y++)
				surface->pixel(x1, y, colour);
		}
		else
		{
			// Diagonals not supported.
		}
		surface->end_region();
	}

protected:
	uint32_t x1;
	uint32_t y1;
	uint32_t x2;
	uint32_t y2;
	TColour colour;
};

#endif /* LIB_STM32_TOOLBOX_GRAPHICS_LINE_H_ */
