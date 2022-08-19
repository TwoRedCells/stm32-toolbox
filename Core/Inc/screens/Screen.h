/**
 * \file       screens/Screen.h
 * \class      Screen
 * \brief      Abstract base class for showing information on an LCD screen.
 *
 * \author     Yvan Rodrigues <yrodrigues@clearpath.ai>
 * \copyright  Copyright (c) 2021, Clearpath Inc., All rights reserved.
 */

#ifndef SCREEN_H
#define SCREEN_H

#include "utility/Timer.h"
#include "devices/Lcd.h"
#include "devices/Encoder.h"
#include "globals.hpp"


/**
 * Abstract class for LCD UI screen.
 */
class Screen
{
public:
	static constexpr uint32_t refresh_timer_interval = milliseconds(250);
	static Screen* active;

    /**
     * Activates this screen and shows this screen on the LCD.
     */
    virtual void show(void)
    {
        active = this;
    }

    /**
     * Deactivates this screen.
     */
    virtual void hide(void)
    {
    	active = nullptr;
    	lcd->clear();
    }


    /**
     * Used to determine if this screen is currently visible.
     * @return True if visible; otherwise false.
     */
    bool is_visible(void)
    {
        return active == this;
    }

    /**
     * Performs one-time initialization.
     * @param lcd A pointer to the Lcd instance.
     * @param parent A pointer to the parent screen.
     */
    virtual void setup(Lcd *lcd, Screen* parent=nullptr)
    {
        Screen::lcd = lcd;
        this->parent = parent;
        refresh_timer.start(refresh_timer_interval);
    }

    /**
     * Performs any work for this screen. This can include any background work.
     * If updating the screen, check whether is_visible is true.
     */
    virtual void loop(void)
    {
    	if (is_visible() && encoder.is_pressed(true) && parent != nullptr)
    	{
    		parent->show();
    	}
    }


protected:
    Lcd *lcd;  // Pointer to the Lcd instance.
    Screen *parent = nullptr;
	Timer refresh_timer;
};

Screen* Screen::active;

#endif
