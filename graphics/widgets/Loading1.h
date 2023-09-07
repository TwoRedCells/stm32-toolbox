///	@file       graphics/widgets/Loading1.h
///	@class      Loading1
///	@brief      A Loading... widget
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef LIB_STM32_TOOLBOX_GRAPHICS_LOADING1_H_
#define LIB_STM32_TOOLBOX_GRAPHICS_LOADING1_H_

#include <graphics/widgets/Container.h>
#include <stdint.h>
#include "IAnimation.h"
#include "generics/List.h"
#include "FilledRectangle.h"

template <class TColour>
class Loading1 : public Container<TColour>, public IOutline<TColour>, public IAnimation
{
public:
	/**
	 * Creates a Loading1 instance. This animation shows boxes in the shape of a square. The square that is filled rotates.
	 * @param x_origin The x-centre of the widget.
	 * @param y_origin The y-centre of the widget.
	 * @param size The width/height of the widget.
	 * @param across The number of squares across (and down).
	 * @param outline_colour The colour of the empty boxes.
	 * @param fill_colour THe colour of the full box.
	 */
	Loading1(uint32_t x_origin, uint32_t y_origin, uint32_t size, uint32_t across, TColour outline_colour, TColour empty_colour, TColour full_colour) :
		Container<TColour>(x_origin, y_origin)
	{
		this->x_origin = x_origin;
		this->y_origin = y_origin;
		this->size = size;
		this->across = across;
		this->outline_colour = outline_colour;
		this->empty_colour = empty_colour;
		this->full_colour = full_colour;

		uint32_t box_size = size / (across*2);

		// Top side.
		for (int32_t a=0; a <across; a++)
		{
			uint32_t x = x_origin - size/2 + a*box_size*2;
			uint32_t y = y_origin - size/2 + 0*box_size*2;
			FilledRectangle<TColour>* box = new FilledRectangle<TColour>(cxywh, x, y, box_size, box_size, 1, outline_colour, empty_colour);
			this->add(box);
		}

		// Right side.
		for (int32_t b=1; b <across; b++)
		{
			uint32_t x = x_origin - size/2 + (across-1)*box_size*2;
			uint32_t y = y_origin - size/2 + b*box_size*2;
			FilledRectangle<TColour>* box = new FilledRectangle<TColour>(cxywh, x, y, box_size, box_size, 1, outline_colour, empty_colour);
			this->add(box);
		}

		// Bottom side.
		for (int32_t a=across-2; a >= 0; a--)
		{
			uint32_t x = x_origin - size/2 + a*box_size*2;
			uint32_t y = y_origin - size/2 + (across-1)*box_size*2;
			FilledRectangle<TColour>* box = new FilledRectangle<TColour>(cxywh, x, y, box_size, box_size, 1, outline_colour, empty_colour);
			this->add(box);
		}

		// Left side.
		for (int32_t b=across-2; b >= 1; b--)
		{
			uint32_t x = x_origin - size/2 + 0*box_size*2;
			uint32_t y = y_origin - size/2 + b*box_size*2;
			FilledRectangle<TColour>* box = new FilledRectangle<TColour>(cxywh, x, y, box_size, box_size, 1, outline_colour, empty_colour);
			this->add(box);
		}
	}


	/**
	 * Renders the collection of widgets onto the drawing surface.
	 * @param surface The drawing surface.
	 */
	void render(IPaintable<TColour>* surface) override
	{
		if (this->enabled)
		{
			Container<TColour>::render(surface);
			next();
		}
	}


	/**
	 * Moves to the first cell in the animation.
	 */
	void first(void) override
	{
		position = 0;
	}


	/**
	 * Moves to the next cell in the animation.
	 */
	void next(void) override
	{
		for (uint32_t i=0; i<this->length; i++)
			((FilledRectangle<TColour>*)this->widgets[i])->set_fill_colour(i==position ? full_colour : empty_colour);
		position++;
		if (position == this->length)
			first();
	}

protected:
	uint32_t size;
	int32_t across;
	uint32_t position = 0;
	TColour empty_colour, full_colour;
};

#endif /* LIB_STM32_TOOLBOX_GRAPHICS_CANVAS_H_ */
