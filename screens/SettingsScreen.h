/**
 * \file       screens/SettingsScreen.h
 * \class      SettingsScreen
 * \brief      Facilitates change of operating parameters.
 *
 * \author     Yvan Rodrigues <yrodrigues@clearpath.ai>
 * \copyright  Copyright (c) 2021, Clearpath Inc., All rights reserved.
 */

#ifndef INC_SETTINGSSCREEN_H
#define INC_SETTINGSSCREEN_H

#include "screens/VerticalMenuScreen.h"
#include "screens/menus/ValueMenuItem.h"
#include "globals.hpp"

/**
 * A screen alter settings.
 */
class SettingsScreen : public VerticalMenuScreen
{
public:
    /**
     * Instantiates an \ref OutputScreen instance.
     */
	SettingsScreen(void) :
        back(STR_BACK_MENU, nullptr),
		charge_time_mi(STR_CHARGE_TIME, ValueMenuItem::Uint)
    {
        value_column = 14;
        //fan_mi.increment = 100;
    }


    /**
     * Configures the screen.
     * @param lcd Pointer to the \ref Lcd instance.
     * @param parent Pointer to the screen that owns this screen.
     */
    void setup(Lcd *lcd, Screen *parent)
    {
        VerticalMenuScreen::setup(lcd, parent);
        back.screen = parent;
        menu.add(&back);
        menu.add(&charge_time_mi);
		charge_time_mi.value = settings.ChargeTime;
    }


	/**
	 * Shows this screen on the LCD.
	 */
	void show(void)
	{
		lcd->clear();
		VerticalMenuScreen::show();
		lcd->set_cursor(false,  false);
		encoder.reset();
		refresh_timer.set(milliseconds(100));
	}


	/**
	 * Called when hiding this screen.
	 */
	void hide(void) override
	{
#if ENABLE_PERSISTENCE
	    settings.save();
#endif
	}

	/**
	 * Invoked when the value of a menu item is being changed.
	 * @param The \ref ValueMenuItem.
	 */
	void on_value_changing(ValueMenuItem* mi) override
	{
	    if (mi == &charge_time_mi)
	        settings.ChargeTime = charge_time_mi.get_uint();
	}

private:
	ScreenMenuItem back;
	ValueMenuItem charge_time_mi;

};
#endif
