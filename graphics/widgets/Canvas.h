///	@file       graphics/widgets/Canvas.h
///	@class      Canvas
///	@brief      A widget that can contain other widgets.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef LIB_STM32_TOOLBOX_GRAPHICS_CANVAS_H_
#define LIB_STM32_TOOLBOX_GRAPHICS_CANVAS_H_

#include "ICanvas.h"
#include <stdint.h>
#include "IWidget.h"

template <class TColour>
class Canvas : public ICanvas<TColour>, public IWidget<TColour>
{
public:
	Canvas(uint32_t x_origin, uint32_t y_origin)
	{
		this->x_origin = x_origin;
		this->y_origin = y_origin;
	}


	/**
	 * Renders the collection of widgets onto the drawing surface.
	 * @param surface The drawing surface.
	 */
	void render(IPaintable<TColour>* surface) override
	{
		for (uint32_t i=0; i<this->length; i++)
			this->widgets[i]->render(surface);
	}


	/**
	 * Renders the collection of widgets onto the drawing surface.
	 */
	void render(void)
	{
		render(this);
	}

protected:

};

#endif /* LIB_STM32_TOOLBOX_GRAPHICS_CANVAS_H_ */
