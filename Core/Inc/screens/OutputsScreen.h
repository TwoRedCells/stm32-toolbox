/**
 * \file       screens/OutputsScreen.h
 * \class      OutputsScreen
 * \brief      Facilitates testings of outputs.
 *
 * \author     Yvan Rodrigues <yrodrigues@clearpath.ai>
 * \copyright  Copyright (c) 2021, Clearpath Inc., All rights reserved.
 */

#ifndef INC_OUTPUTSCREEN_H
#define INC_OUTPUTSCREEN_H

#include "screens/VerticalMenuScreen.h"
#include "screens/menus/ValueMenuItem.h"
#include "globals.hpp"

/**
 * A screen to test outputs.
 */
class OutputsScreen : public VerticalMenuScreen
{
public:
    /**
     * Instantiates an \ref OutputScreen instance.
     */
    OutputsScreen(void) :
        back(STR_BACK_MENU, nullptr),
        pilot_mi(STR_CHARGE_PILOT, ValueMenuItem::Bool),
        contactor_mi(STR_CONTACTOR, ValueMenuItem::Bool),
        discharge_mi(STR_DISCHARGE, ValueMenuItem::Bool),
        fan_mi("Fan", ValueMenuItem::Uint)
    {
        value_column = 14;
        fan_mi.increment = 100;
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
        menu.add(&pilot_mi);
        menu.add(&contactor_mi);
        menu.add(&discharge_mi);
        menu.add(&fan_mi);
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
		fan_mi.value = fan.get_target_speed();
		fan_override = true;
	}


	void hide(void) override
	{
	    fan_override = false;
	}

	/**
	 * Invoked when the value of a menu item is being changed.
	 * @param The \ref ValueMenuItem.
	 */
	void on_value_changing(ValueMenuItem* mi) override
	{
	    if (mi == &pilot_mi)
	        pilot.set(pilot_mi.get_bool());
	    else if (mi == &contactor_mi)
	        contactor.set(contactor_mi.get_bool());
	    else if (mi == &discharge_mi)
	        discharge.set(discharge_mi.get_bool());
	    else if (mi == &fan_mi)
	        fan.set_target_speed(fan_mi.get_uint());
	}

private:
	ScreenMenuItem back;
	ValueMenuItem pilot_mi;
    ValueMenuItem contactor_mi;
    ValueMenuItem discharge_mi;
    ValueMenuItem fan_mi;
};
#endif
