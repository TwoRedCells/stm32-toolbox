/**
 * \file       screens/BootScreen
 * \class      BootScreen
 * \brief      Displays briefly on bootup.
 *
 * \author     Yvan Rodrigues <yrodrigues@clearpath.ai>
 * \copyright  Copyright (c) 2021, Clearpath Inc., All rights reserved.
 */

#ifndef FAULTSCREEN_H
#define FAULTSCREEN_H

#include "screens/Screen.h"
#include "constants.h"


class BootScreen : public Screen
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
		lcd->printat(0, 0, STR_TITLE);
		lcd->printat(1, 0, "Hardware v" HARDWARE_VERSION);
		lcd->printat(2, 0, "Firmware v" FIRMWARE_VERSION);
	}
};
#endif
