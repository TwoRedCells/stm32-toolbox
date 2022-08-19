/**
 * \file       screens/TemperatureScreen.h
 * \class      TemperatureScreen
 * \brief      Shows current temperature data on an LCD screen.
 *
 * \author     Yvan Rodrigues <yrodrigues@clearpath.ai>
 * \copyright  Copyright (c) 2021, Clearpath Inc., All rights reserved.
 */

#ifndef TEMPERATURESCREEN_H
#define TEMPERATURESCREEN_H

#include "screens/Screen.h"
#include "globals.hpp"


/**
 * A screen showing reported temperatures.
 */
class TemperatureScreen : public Screen
{
public:
	/**
	 * Shows this screen on the LCD.
	 */
	void show(void)
	{
		lcd->clear();
		Screen::show();
		lcd->set_cursor(false,  false);
		encoder.reset();
		refresh_timer.set(milliseconds(500));
	}

	/**
	 * Performs any work for this screen, including updating the display if applicable.
	 */
	void loop(void)
	{
		Screen::loop();
		if (refresh_timer.is_elapsed())
		{
			if (is_visible())
			{

				lcd->printat(0, 0, "INT " STR_ONE_DECIMAL_FLOAT, int_temp);
				lcd->printat(1, 0, "EXT " STR_ONE_DECIMAL_FLOAT, ext_temp);
				lcd->printat(0, 10, "B1C " STR_ONE_DECIMAL_FLOAT, batteries[0].cell_temperature);
				lcd->printat(1, 10, "B1F " STR_ONE_DECIMAL_FLOAT, batteries[0].fet_temperature);
				lcd->printat(2, 10, "B2C " STR_ONE_DECIMAL_FLOAT, batteries[1].cell_temperature);
				lcd->printat(3, 10, "B2F " STR_ONE_DECIMAL_FLOAT, batteries[1].fet_temperature);
			}
			refresh_timer.restart();
		}
	}
};
#endif
