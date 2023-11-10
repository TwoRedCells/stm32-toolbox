/**
 * @file 		rad/widgets/BatteryIcon.h
 * @class		BatteryIcon
 * @author 		Yvan Rodrigues <yvan.r@radskunkworks.com>
 * @brief 		A widget that shows the absolute and relative instantanous current.
 * @date 		2023-09-11
 * @copyright 	Copyright (c) 2023 Robotic Assistance Devices
 */


#ifndef RAD_WIDGETS_BATTERYICON_H_
#define RAD_WIDGETS_BATTERYICON_H_

#include "graphics/widgets/Container.h"
#include "graphics/widgets/Line.h"
#include "graphics/widgets/Pixel.h"


template <class TColour>
class BatteryIcon : Container<RGB>
{
public:
	/**
	 * Creates a battery icon.
	 * @param x The x-origin.
	 * @param y The y-origin.
	 */
	BatteryIcon(uint32_t x, uint32_t y, float low_threshold, TColour outline_colour, TColour good_colour, TColour low_colour) : Container<TColour>(x, y)
	{
		this->x_origin = x;
		this->y_origin = y;
		this->low_threshold = low_threshold;
		this->outline_colour = outline_colour;
		this->good_colour = good_colour;
		this->low_colour = low_colour;
		this->width = 20;
		this->height = 10;
	}


	/**
	 * Renders this widget on a canvas.
	 * @param canvas The canvas to paint on.
	 */
	void render(IPaintable<RGB>* surface) override
	{
		if (this->enabled)
			render(surface, this->x_origin, this->y_origin, level, low_threshold, outline_colour, good_colour, low_colour);
	}


	/**
	 * Renders the widget.
	 * @param surface The drawing surface.
	 * @param x The centre x-coordinate.
	 * @param y The centre y-coordinate.
	 * @param level The level.
	 */
	static void render(IPaintable<RGB>* surface, uint32_t x, uint32_t y, float level, bool charging, float low_threshold, TColour outline_colour, TColour good_colour, TColour low_colour)
	{
		const uint16_t body_width = 18;
		const uint16_t terminal_width = 2;
		const uint16_t terminal_inset = 3;
		const uint16_t height = 10;

		// Draw outline.
		Line<TColour>::render(surface, x, y, x+body_width, y, outline_colour);  // Top.
		Line<TColour>::render(surface, x, y+height-1, x+body_width, y+height-1, outline_colour);  // Bottom.
		Line<TColour>::render(surface, x, y, x, y+height-1, outline_colour);  // Left.
		Line<TColour>::render(surface, x+body_width, y, x+body_width, y+terminal_inset, outline_colour);  // Terminal 1/5.
		Line<TColour>::render(surface, x+body_width, y+terminal_inset, x+body_width+terminal_width, y+terminal_inset, outline_colour);  // Terminal 2/5.
		Line<TColour>::render(surface, x+body_width+terminal_width, y+terminal_inset, x+body_width+terminal_width, y+height-terminal_inset-1, outline_colour);  // Terminal 3/5.
		Line<TColour>::render(surface, x+body_width, y+height-terminal_inset-1, x+body_width+terminal_width, y+height-terminal_inset-1, outline_colour);  // Terminal 4/5.
		Line<TColour>::render(surface, x+body_width, y+height-terminal_inset-1, x+body_width, y+height-1, outline_colour);  // Terminal 5/5.
		TColour colour = level <= low_threshold ? low_colour : good_colour;
		for (uint8_t i=2; i<(body_width-1)*level; i++)
			Line<TColour>::render(surface, x+i, y+2, x+i, y+height-3, colour);

		// Draw lightning bolt.
		if (charging)
		{
			Pixel<TColour>::render(surface, x+3, y+6, outline_colour);
			Pixel<TColour>::render(surface, x+4, y+6, outline_colour);
			Pixel<TColour>::render(surface, x+5, y+5, outline_colour);
			Pixel<TColour>::render(surface, x+6, y+5, outline_colour);
			Pixel<TColour>::render(surface, x+7, y+4, outline_colour);
			Pixel<TColour>::render(surface, x+8, y+4, outline_colour);
			Pixel<TColour>::render(surface, x+8, y+5, outline_colour);
			Pixel<TColour>::render(surface, x+8, y+6, outline_colour);
			Pixel<TColour>::render(surface, x+9, y+3, outline_colour);
			Pixel<TColour>::render(surface, x+9, y+4, outline_colour);
			Pixel<TColour>::render(surface, x+9, y+5, outline_colour);
			Pixel<TColour>::render(surface, x+10, y+5, outline_colour);
			Pixel<TColour>::render(surface, x+11, y+4, outline_colour);
			Pixel<TColour>::render(surface, x+12, y+4, outline_colour);
			Pixel<TColour>::render(surface, x+13, y+3, outline_colour);
			Pixel<TColour>::render(surface, x+14, y+3, outline_colour);
		}
	}

	/**
	 * Sets the value.
	 * @param value The new value.
	 */
	void set_level(float level)
	{
		this->level = level;
	}


private:
	double level;
	TColour outline_colour;
	TColour good_colour;
	TColour low_colour;
	float low_threshold;
};

#endif /* RAD_WIDGETS_BATTERYICON_H_ */
