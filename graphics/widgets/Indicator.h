///	@file       graphics/widgets/Indicator.h
///	@class      Indicator
///	@brief      A rectangle with text where the background changes colour based on state.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef LIB_STM32_TOOLBOX_GRAPHICS_INDICATOR_H_
#define LIB_STM32_TOOLBOX_GRAPHICS_INDICATOR_H_

#include <stdint.h>
#include "IWidget.h"
#include "IOutline.h"
#include "IFill.h"
#include "Line.h"
#include "math.h"
#include "FilledRectangle.h"


template <class TColour>
class Indicator : public FilledRectangle<TColour>
{
public:
	/**
	 * Constructs a rectangle.
	 * @param x The upper-left x-coordinate.
	 * @param y The upper-left y-coordinate.
	 * @param w The width.
	 * @param h The height.
	 * @param scale The font size.
	 * @param label The text label.
	 * @param foreground The text colour.
	 * @param inactive The background colour when inactive.
	 * @param active The background colour when active.
	 */
	Indicator(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t scale, const char* label, TColour foreground, TColour inactive, TColour active)
		: FilledRectangle<TColour>(xywh, x, y, w, h, 0, inactive, inactive)
	{
		this->label = label;
		this->inactive = inactive;
		this->active = active;
		this->scale = scale;
		this->foreground = foreground;
	}


	/**
	 * Renders this widget on a canvas.
	 * @param canvas The canvas to paint on.
	 */
	void render(IPaintable<TColour>* surface) override
	{
		if (this->enabled)
			render(surface, this->x1, this->y1, this->x2, this->y2, scale, label, inactive, active, state);
	}


	/**
	 * Renders the control.
	 * @param surface Pointer to the drawing surface.
	 * @param x1 The upper-left x-coordinate.
	 * @param y1 The upper-left y-coordinate.
	 * @param x2 The lower-left x-coordinate.
	 * @param y2 The lower-left y-coordinate.
	 * @param scale The font size.
	 * @param label The text label.
	 * @param foreground The text colour.
	 * @param inactive The background colour when inactive.
	 * @param active The background colour when active.
	 * @param state The control state.
	 */
	static void render(IPaintable<TColour>* surface, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t scale, const char* label, TColour foreground, TColour inactive, TColour active, bool state)
	{
		TColour colour = state ? active : inactive;
		FilledRectangle<TColour>::render(surface, x1, y1, x2, y2, 0, colour, colour);
		Text<TColour>::render(surface, x1+(x2-x1)/2, y1+(y2-y1)/2-scale*8/2, Centre, foreground, scale, label);
	}


	/**
	 * Renders the control.
	 * @param surface Pointer to the drawing surface.
	 * @param x1 The upper-left x-coordinate.
	 * @param y1 The upper-left y-coordinate.
	 * @param x2 The lower-left x-coordinate.
	 * @param y2 The lower-left y-coordinate.
	 * @param scale The font size.
	 * @param label The text label.
	 * @param foreground The text colour.
	 * @param inactive The background colour when inactive.
	 * @param active The background colour when active.
	 * @param state The control state.
	 */
	static void render_fast(IPaintable<TColour>* surface, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t scale, const char* label, TColour foreground, TColour inactive, TColour active, bool state)
	{
		while(surface->tile_region(x1, y1, x2-x1, y2-y1))
			render(surface, x1, y1, x2, y2, scale, label, foreground, inactive, active, state);
	}


	/**
	 * Sets the state of this control.
	 * @param active True if active.
	 */
	void set_state (bool active)
	{
		this->state = active;
		set_fill_colour(this>state ? this->active : this->inactive);
	}


private:
	TColour inactive;
	TColour active;
	TColour foreground;
	const char* label;
	uint8_t scale;
	bool state;
};

#endif /* LIB_STM32_TOOLBOX_GRAPHICS_RECTANGLE_H_ */
