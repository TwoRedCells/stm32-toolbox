/**
 * @file 		rad/Graphics.h
 * @author 		Yvan Rodrigues <yvan.r@radskunkworks.com>
 * @brief 		Connects the widget library to the Oled library.
 * @date 		2023-09-20
 * @copyright 	Copyright (c) 2023 Robotic Assistance Devices
 */

#ifndef INC_RAD_GRAPHICS_H_
#define INC_RAD_GRAPHICS_H_

#include "graphics/widgets/IPaintable.h"
#include "graphics/widgets/ICanvas.h"
#include "devices/displays/OledSsd1306.h"


class OledSsd1306Canvas : public OledSsd1306, public IPaintable<OledColour>, public ICanvas<OledColour>
{
public:
	OledSsd1306Canvas(I2C_HandleTypeDef* i2c, uint16_t i2cadr) : OledSsd1306(i2c, i2cadr)
	{
	}

	void pixel(uint32_t x, uint32_t y, OledColour hue)
	{
		if (in_transaction)
		{
			if (x >= region_x && x < region_x + region_w && y >= region_y && y < region_y + region_h)
			{
				uint32_t offset = (y-region_y) * region_w + x-region_x;
				if (offset < buffer_size)
					buffer[offset] = hue;
			}
		}
		else if (x < OLED_SSD1306_WIDTH && y < OLED_SSD1306_HEIGHT)
		{
			OledSsd1306::pixel(x, y, hue);
		}
	}

	uint32_t get_buffer_size(void)
	{
		return buffer_size * sizeof(OledColour);
	}
private:

	static constexpr const uint32_t buffer_size = OLED_SSD1306_WIDTH * OLED_SSD1306_HEIGHT;
	OledColour buffer[buffer_size];
};

#endif /* INC_RAD_GRAPHICS_H_ */
