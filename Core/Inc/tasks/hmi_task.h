/**
 * \file       tasks/hmi_task.h
 * \brief      Presents the user interface using the LCD HMI.
 */

#ifndef INC_THREADS_HMI_H_
#define INC_THREADS_HMI_H_

#include "screens/BatteryVoltageScreen.h"
#include "screens/BatteryStatusScreen.h"
#include "screens/BatteryCurrentScreen.h"
#include "screens/SystemScreen.h"
#include "screens/TestScreen.h"
#include "screens/TemperatureScreen.h"
#include "screens/VerticalMenuScreen.h"
#include "screens/BootScreen.h"
#include "screens/OutputsScreen.h"
#include "screens/menus/ResetMenuItem.h"
#if ENABLE_SETTINGS
	#include "screens/SettingsScreen.h"
#endif

/**
 * @brief  Function implementing the blink01 thread.
 * @param  argument: Not used
 * @retval None
 */

void hmi_task(void *argument)
{
	log_d("HMI task started.");
	lcd.setup();

	// Instantiate screens.
	BootScreen boot_screen;
	VerticalMenuScreen main_menu_screen;  // Main menu screen.
	VerticalMenuScreen diagnostics_menu_screen;  // Diagnostics menu screen.
	SystemScreen system_screen;
	TemperatureScreen temperature_screen;
	BatteryVoltageScreen battery_voltage_screen;
	BatteryCurrentScreen battery_current_screen;
	BatteryStatusScreen battery_status_screen;
	TestScreen test_screen;
	OutputsScreen output_screen;

	// Construct main menu.
    ScreenMenuItem start_test(STR_START_TEST, &test_screen);
	ScreenMenuItem system(STR_SYSTEM_STATUS, &system_screen);
	ScreenMenuItem diagnostics(STR_DIAGNOSTICS, &diagnostics_menu_screen);
	main_menu_screen.add(&start_test);
	main_menu_screen.add(&system);
	main_menu_screen.add(&diagnostics);
	main_menu_screen.setup(&lcd);

	// Construct diagnostics menu.
	ScreenMenuItem back(STR_BACK_MENU, &main_menu_screen);
	diagnostics_menu_screen.menu.active_index = 1;  // Place the cursor on the following item.
	diagnostics_menu_screen.add(&back);
	ScreenMenuItem temperatures("Temperatures", &temperature_screen);
	diagnostics_menu_screen.add(&temperatures);
	ScreenMenuItem battery_status("Battery Status", &battery_status_screen);
	diagnostics_menu_screen.add(&battery_status);
	ScreenMenuItem battery_voltage("Battery Voltage", &battery_voltage_screen);
	diagnostics_menu_screen.add(&battery_voltage);
	ScreenMenuItem battery_current("Battery Current", &battery_current_screen);
	diagnostics_menu_screen.add(&battery_current);
	ScreenMenuItem outputs("Outputs", &output_screen);
	diagnostics_menu_screen.add(&outputs);
	ResetMenuItem reset("Reset");
	diagnostics_menu_screen.add(&reset);

	// Configure screens.
	boot_screen.setup(&lcd);
	diagnostics_menu_screen.setup(&lcd);
	system_screen.setup(&lcd, &main_menu_screen);
	test_screen.setup(&lcd, &main_menu_screen);
	temperature_screen.setup(&lcd, &diagnostics_menu_screen);
	battery_voltage_screen.setup(&lcd, &diagnostics_menu_screen);
	battery_current_screen.setup(&lcd, &diagnostics_menu_screen);
	battery_status_screen.setup(&lcd, &diagnostics_menu_screen);
	output_screen.setup(&lcd, &diagnostics_menu_screen);

#if ENABLE_SETTINGS
	SettingsScreen settings_screen;
    ScreenMenuItem settings(STR_SETTINGS, &settings_screen);
	main_menu_screen.add(&settings);
	settings_screen.setup(&lcd, &main_menu_screen);
#endif

	// Start by showing the boot screen, then system screen.
	boot_screen.show();
	osDelay(4000);
	system_screen.show();

	// Each screen gets a slot to update itself.
	for(;;)
	{
        watchdog.feed(WATCHDOG_HMI_TASK_FLAG);

		main_menu_screen.loop();
		test_screen.loop();
		diagnostics_menu_screen.loop();
		system_screen.loop();
		temperature_screen.loop();
		battery_voltage_screen.loop();
		battery_current_screen.loop();
		battery_status_screen.loop();
		output_screen.loop();
#if ENABLE_SETTINGS
		settings_screen.loop();
#endif
		osDelay(25);
	}
}


#endif /* INC_THREADS_HMI_H_ */
