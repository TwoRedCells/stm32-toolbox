///	@file       graphics/widgets/ICanvas.h
///	@class      ICanvas
///	@brief      Interface for a general-purpose graphics drawing surface.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef LIB_STM32_TOOLBOX_GRAPHICS_ICANVAS_H_
#define LIB_STM32_TOOLBOX_GRAPHICS_ICANVAS_H_

#include <stdint.h>
#include "IWidget.h"
#include "toolbox.h"


template <class TColour>
class ICanvas //: public IPaintable<TColour>
{
public:
	/**
	 * Adds a widget to the collection.
	 * @param widget The widget.
	 */
	void add(IWidget<TColour>* widget)
	{
		widgets[length++] = widget;
	}

	/**
	 * Renders the collection of widgets onto the drawing surface.
	 */
	void render(IPaintable<TColour>* surface)
	{
		for (uint32_t i=0; i<this->length; i++)
			this->widgets[i]->render(surface);
	}


protected:
	uint32_t length = 0;
	IWidget<TColour>* widgets[ICANVAS_MAX_WIDGETS];
};

#endif /* LIB_STM32_TOOLBOX_GRAPHICS_ICANVAS_H_ */
