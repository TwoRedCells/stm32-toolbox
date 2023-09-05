///	@file       graphics/widgets/Rectangle.h
///	@class      Rectangle
///	@brief      A 4-sided polygon having horizontal and vertical sides.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef LIB_STM32_TOOLBOX_GRAPHICS_RECTANGLE_H_
#define LIB_STM32_TOOLBOX_GRAPHICS_RECTANGLE_H_

#include <stdint.h>
#include "IWidget.h"
#include "IOutline.h"
#include "Line.h"
#include "math.h"

enum RectangleType {
	xyxy,  // Upper-left and lower-right coordinates are specified.
	xywh,  // Upper-left and width/height are specified.
	cxywh  // Centre point and width/height are specified.
};

template <class TColour>
class Rectangle : public IWidget<TColour>, public IOutline<TColour>
{
public:
	Rectangle()
	{}
	/**
	 * Constructs a rectangle.
	 * @param a The first coordinate parameter.
	 * @param b The second coordinate parameter.
	 * @param c The third coordinate parameter.
	 * @param d The fourth coordinate parameter.
	 * @param thickness The thickness.
	 * @param c The colour.
	 */
	Rectangle(RectangleType type, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t thickness, TColour colour)
	{
		// All rectangle types get converted into xyxy.
		if (type == xyxy)
		{
			this->x1 = a;
			this->y1 = b;
			this->x_origin = a;
			this->y_origin = a;
			this->x2 = c;
			this->y2 = d;

		}
		else if (type == xywh)
		{
			this->x1 = a;
			this->y1 = b;
			this->x2 = x1+c;
			this->y2 =  y1+d;
			this->x_origin = this->x1;
			this->y_origin = this->y2;

		}
		else if (type == cxywh)
		{
			this->x_origin = a;
			this->y_origin = b;
			this->x1 = a - c/2;
			this->y1 = b - d/2;
			this->x2 = a + c/2;
			this->y2 = b + d/2;
		}

		this->width = x2 - x1+1;
		this->height = y2 - y1+1;
		this->outline_colour = colour;
		this->thickness = thickness;
	}


	/**
	 * Renders this widget on a canvas.
	 * @param canvas The canvas to paint on.
	 */
	void render(IPaintable<TColour>* surface) override
	{
		render(surface, x1, y1, x2, y2, thickness, this->outline_colour);
	}

	static void render(IPaintable<TColour>* surface, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t thickness, TColour colour)
	{
		surface->start_region(x1,  y1, x2-x1+1, y2-y1+1);
		for(uint32_t t=0; t < thickness; t++)
		{
			Line<TColour>::render(surface, x1+t, y1+t, x2-t, y1+t, colour);  // Top.
			Line<TColour>::render(surface, x2-t, y1+t, x2-t, y2-t, colour);  // Right.
			Line<TColour>::render(surface, x1+t, y2-t, x2-t, y2-t, colour);  // Bottom.
			Line<TColour>::render(surface, x1+t, y1+t, x1+t, y2-t, colour);  // Left.
		}
		surface->end_region();
	}


protected:
	uint32_t x1;
	uint32_t y1;
	uint32_t x2;
	uint32_t y2;
	uint32_t thickness;
};

#endif /* LIB_STM32_TOOLBOX_GRAPHICS_RECTANGLE_H_ */
