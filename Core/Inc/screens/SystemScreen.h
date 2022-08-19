/**
 * \file       screens/SystemScreen.h
 * \class      SystemScreen
 * \brief      Shows general system information at-a-glance on an LCD screen.
 *
 * \author     Yvan Rodrigues <yrodrigues@clearpath.ai>
 * \copyright  Copyright (c) 2021, Clearpath Inc., All rights reserved.
 */

#ifndef INFOSCREEN_H
#define INFOSCREEN_H

#include "screens/Screen.h"

/**
 * A screen with general information such as the version and uptime.
 */
class SystemScreen : public Screen
{
public:
	/**
	 * Shows this screen on the LCD.
	 */
	void show(void)
	{
		lcd->clear();
		Screen::show();
		lcd->set_cursor(false, false);
		encoder.reset();
		refresh_timer.set(milliseconds(100));
	}

	/**
	 * Performs any work for this screen, including updating the display if applicable.
	 */
	void loop(void)
	{
		if (refresh_timer.is_elapsed())
		{
			if (is_visible())
			{
                // Voltages / SOC.
                lcd->printat(0, 0, "%.1fv/%.1fv %u%% %u%%", batteries[0].module_voltage, batteries[1].module_voltage, batteries[0].relative_state_of_charge, batteries[1].relative_state_of_charge);

                // Temperatures.
                lcd->printat(1, 0, "%.1fc int %.1fc ext", int_temp, ext_temp);

			    // Fan speed and duty cycle.
				lcd->printat(2, 0, "%u/%urpm %.1f%%  ", fan.get_target_speed(), fan.get_measured_speed(), fan.get_duty_cycle()*100);

				// Relay states.
				lcd->printat(3, 19, contactor.get() ? STR_C : STR_O);
				lcd->printat(3, 18, pilot.get() ? STR_C: STR_O);

				// Faults
				uint32_t faults = fault.get();
				lcd->printat(3, 9, STR_HEX_HEX, faults >> 16, faults & 0xffff);

				// Uptime.
				uint8_t hours = uptime / 3600;
				uint8_t minutes = (uptime - hours*3600) / 60;
				uint8_t seconds = (uptime - hours*3600 - minutes*60);
				lcd->printat(3, 0, "%u:%u:%u ", hours, minutes, seconds);
			}
			refresh_timer.restart();
		}
		Screen::loop();
	}
};
#endif
