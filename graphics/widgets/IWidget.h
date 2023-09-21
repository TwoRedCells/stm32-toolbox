///	@file       graphics/widgets/IWidget.h
///	@class      IWidget
///	@brief      Interface for a general-purpose user-interface element.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef LIB_STM32_TOOLBOX_GRAPHICS_IWIDGET_H_
#define LIB_STM32_TOOLBOX_GRAPHICS_IWIDGET_H_

#include <stdint.h>
#include "IPaintable.h"


template <class TColour>
class IWidget
{
public:
	/**
	 * Gets the x-origin.
	 * @returns The x-origin.
	 */
	uint32_t get_x_origin(void)
	{
		return x_origin;
	}


	/**
	 * Gets the y-origin.
	 * @returns The y-origin.
	 */
	uint32_t get_y_origin(void)
	{
		return y_origin;
	}


	/**
	 * Gets the width.
	 * @returns The width.
	 */
	uint32_t get_width(void)
	{
		return width;
	}


	/**
	 * Gets the height.
	 * @returns The height.
	 */
	uint32_t get_height(void)
	{
		return height;
	}


	/**
	 * Renders this widget on a canvas.
	 * @param canvas The canvas to paint on.
	 */
	virtual void render(IPaintable<TColour>* surface)
	{
		// Abstract type. Do nothing.
	}


	/**
	 * Enables this widget.
	 */
	void enable(void)
	{
		enabled = true;
	}


	/**
	 * Disables this widget.
	 */
	void disable(void)
	{
		enabled = false;
	}


protected:
	uint32_t x_origin;
	uint32_t y_origin;
	uint32_t width;
	uint32_t height;
	bool enabled = true;
};

#endif /* LIB_STM32_TOOLBOX_GRAPHICS_IWIDGET_H_ */
