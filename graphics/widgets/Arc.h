///	@file       graphics/widgets/Arc.h
///	@class      Arc
///	@brief      An arc of arbitrary start angle and length.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef LIB_STM32_TOOLBOX_GRAPHICS_ARC_H_
#define LIB_STM32_TOOLBOX_GRAPHICS_ARC_H_

#include <stdint.h>
#include "IWidget.h"
#include "IOutline.h"
#include "IFill.h"
#include "math.h"



template <class TColour>
class Arc : public IWidget<TColour>, public IOutline<TColour>, public IFill<TColour>
{
public:
	Arc()
	{}

	/**
	 * Constructs a rectangle.
	 * @param x The centre x-coordinate.
	 * @param y The centre y-coordinate.
	 * @param r The radius
	 * @param s The start angle
	 * @param e The end angle.
	 * @param t The outline thickness.
	 * @param o The outline colour.
	 * @param f The fill colour.
	 */
	Arc(uint32_t x, uint32_t y, uint32_t o_r, uint32_t i_r, uint32_t s, uint32_t e, uint32_t t, TColour o, TColour f)
	{
		this->x_origin = x;
		this->y_origin = y;
		this->outer_radius = o_r;
		this->inner_radius = i_r;
		this->start = s;
		this->end = e;
		this->thickness = t;
		this->outline_colour = o;
		this->fill_colour = f;
		this->width = o_r * 2;
		this->height = o_r * 2;
	}


	/**
	 * Renders this widget on a canvas.
	 * @param canvas The canvas to paint on.
	 */
	void render(IPaintable<TColour>* surface) override
	{
		if (this->enabled)
			render(surface, this->x_origin, this->y_origin, outer_radius, inner_radius, start, end, this->thickness, this->outline_colour, this->fill_colour);
	}


	/**
	 * Renders the arc.
	 * @param surface The drawing surface.
	 * @param x The centre x-coordinate.
	 * @param y The centre y-coordinate.
	 * @param r The radius
	 * @param s The start angle
	 * @param e The end angle.
	 * @param t The outline thickness.
	 * @param o The outline colour.
	 * @param f The fill colour.
	 */
	static void render(IPaintable<TColour>* surface, uint32_t x, uint32_t y, uint32_t o_r, uint32_t i_r, uint32_t s, uint32_t e, uint32_t t, TColour o, TColour f)
	{
		float _r = o_r;  // Radius as float.
		float width = o_r - i_r;
		if (t != 0)
		{
			// Draw outlne.
		}


		// Draw fill.
		for (uint32_t u=0; u<width; u++)
		{
			float inc = 180.0f / (2.0f * M_PI * _r);
			for (float a = s; a < e; a += inc)
			{
				float rad = a * M_PI / 180.0f;
				uint32_t x1 = x + cos(rad) * _r;
				uint32_t y1 = y + sin(rad) * _r;
					surface->pixel(x1, y1, f);
			}
			_r--;
		}
	}


	/**
	 * Sets the range of the arc.
	 * @param s The start angle.
	 * @param e The end angle.
	 */
	void set_range(uint32_t s, uint32_t e)
	{
		this->start = s;
		this->end = e;
	}



protected:
	uint32_t outer_radius;
	uint32_t inner_radius;
	uint32_t start;
	uint32_t end;
};

#endif /* LIB_STM32_TOOLBOX_GRAPHICS_ARC_H_ */
