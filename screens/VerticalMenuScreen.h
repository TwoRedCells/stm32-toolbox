/**
 * \file       screens/VerticalMenuScreen.h
 * \class      VerticalMenuScreen
 * \brief      Visualizes a Menu in vertical scrolling format on an LCD screen.
 *
 * \author     Yvan Rodrigues <yrodrigues@clearpath.ai>
 * \copyright  Copyright (c) 2021, Clearpath Inc., All rights reserved.
 */

#ifndef VERTICALMENUSCREEN_H
#define VERTICALMENUSCREEN_H

#include <math.h>
#include "screens/menus/Menu.h"
#include "screens/menus/ScreenMenuItem.h"
#include "screens/menus/ValueMenuItem.h"
#include "screens/Screen.h"
#include "constants.h"


/**
 * A screen with vertical menu selection.
 */
class VerticalMenuScreen : public Screen
{
public:
	bool is_editing = false;
	uint8_t current_row;
	Menu menu;

	/**
	 * Shows this screen on the LCD.
	 */
	void show(void)
	{
		Screen::show();
		lcd->set_cursor(false,  false);
		encoder.reset();
		show_menu();
		refresh_timer.set(milliseconds(100));
	}


	/**
	 * Shows the menu on the screen and moves the cursor to the current item.
	 */
	void show_menu(void)
	{
		lcd->clear();
		uint32_t menu_length = menu.get_length() - menu.first_index;
		uint8_t l = menu_length < lcd->row_count ? menu_length : lcd->row_count;
		for (uint8_t r=0; r<l; r++)
		{
			MenuItem* mi = menu.get(r + menu.first_index);
			// Show name/key.
			lcd->printat(r,  1, mi->name);
			// Show value.
			if (mi->get_typeid() == ValueMenuItem::TypeId)
				show_value(r, value_column, (ValueMenuItem*)mi);
		}
		highlight_position();
	}


	/**
	 * Shows the value of the specified \ref ValueMenuItem at the specified location on screen.
	 * @param row The row position for the value.
	 * @param col THe column position for the value.
	 * @param vmi The \ref ValueMenuItem.
	 */
	void show_value(uint8_t row, uint8_t col, ValueMenuItem* vmi)
	{
		lcd->clear_to_eol(row, col);
		switch (vmi->value_type)
		{
		case ValueMenuItem::Int:
			lcd->printat(row, col, "%i", vmi->get_int());
			break;
		case ValueMenuItem::Uint:
			lcd->printat(row, col, "%u", vmi->get_uint());
			break;
		case ValueMenuItem::Float:
			int16_t maj, min;
			maj = vmi->get_int();
			min = (vmi->value - maj + 0.00005) * pow(10, vmi->decimals);
			lcd->printat(row, col, "%i", maj);
			lcd->print(".");
			lcd->print("%u", min);
			break;
		case ValueMenuItem::Bool:
			lcd->printat(row, col, "%s", vmi->get_bool() ? vmi->true_string : vmi->false_string);
			break;
		}
	}


	/**
	 * Indicates the current row by placing and arrow to the left of it.
	 */
	void highlight_position(void)
	{
		uint32_t menu_length = menu.get_length() - menu.first_index;
		uint8_t l = menu_length < lcd->row_count ? menu_length : lcd->row_count;
		for (uint8_t r=0; r<l; r++)
		{
			uint8_t m = r + menu.first_index;
			lcd->printat(r, 0, m == menu.active_index ? STR_RIGHT_ARROW : STR_SPACE);
			if (m == menu.active_index)
				current_row = r;
		}
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
				if (!is_editing)
				{
					refresh();
				}
				else
				{
					edit_value();
				}

				if (encoder.is_pressed(true))
				{
			        MenuItem *mi = menu.get(menu.active_index);
					on_pressed(mi);
				}
			}
			refresh_timer.restart();
		}
	}


	/**
	 * Forces the refresh of the menu on screen.
	 */
	void refresh(void)
	{
		int32_t v = encoder.get_value();
		uint8_t first = menu.first_index;

		while (v != 0)
		{
			// Encoder has been turned. Move the cursor, and scroll if necessary.
			if (v < 0)
			{
				menu.previous();
				if (menu.active_index - menu.first_index < 0)
					menu.first_index--;
				v++;
			}
			else if (v > 0)
			{
				menu.next();
				if (menu.active_index - menu.first_index > lcd->row_count-1)
					menu.first_index++;
				v--;
			}
		}

		// We scrolled, so have to update whole screen.
		if (first != menu.first_index)
		{
			show_menu();
		}

		// Selection changed, so move cursor.
		if (encoder.get_value() != 0)
		{
			highlight_position();
			encoder.reset();
	        MenuItem *mi = menu.get(menu.active_index);
			on_selected_changed(mi);
		}
	}


	/**
	 * Allows the user to change the value using the control.
	 */
	void edit_value(void)
	{
		ValueMenuItem* vmi = (ValueMenuItem*)menu.get(menu.active_index);
		int32_t v = encoder.get_value();

		while (v != 0)
		{
			// Encoder has been turned. Move the cursor, and scroll if necessary.
			if (v < 0)
			{
				if (vmi->value_type == ValueMenuItem::Bool)
					vmi->value = vmi->value == 0 ? 1 : 0;
				else
					vmi->value -= vmi->increment;
				v++;
			}
			else if (v > 0)
			{
				if (vmi->value_type == ValueMenuItem::Bool)
					vmi->value = vmi->value == 0 ? 1 : 0;
				else
					vmi->value += vmi->increment;
				v--;
			}
			show_value(current_row, value_column, vmi);
			on_value_changing(vmi);
		}
		encoder.reset();
	}


	/**
	 * Adds an item to the menu.
	 * @param item The \ref MenuItem.
	 */
	void add(MenuItem* item)
	{
		menu.add(item);
	}


	/**
	 * Invoked when the selected \ref MenuItem has changed as a user scrolls
	 * through available options.
	 * @param menu_item Pointer to the newly selected \ref MenuItem.
	 */
	virtual void on_selected_changed(MenuItem *menu_item)
	{
	}


	/**
	 * Invoked when the button is pressed for the selected \ref MenuItem.
	 * @param menu_item Pointer to the selected \ref MenuItem.
	 */
	virtual void on_pressed(MenuItem *menu_item)
	{
		if (menu_item->get_typeid() == ScreenMenuItem::TypeId)
		{
			ScreenMenuItem* smi = (ScreenMenuItem*)menu_item;
			smi->screen->show();
		}

		if (menu_item->get_typeid() == ValueMenuItem::TypeId)
		{
		    // Switch in or out of editing mode.
		    ValueMenuItem* vmi = (ValueMenuItem*)menu_item;
			is_editing = !is_editing;
			show_value(current_row, value_column, vmi);  // Move the cursor next to the value.
			lcd->set_cursor(false, is_editing);
			if (!is_editing)
			    on_value_changed(vmi);
		}

		// The menu item may implement an action.
		menu_item->on_pressed();
	}


    /**
     * Invoked when the value of a \ref ValueMenuItem is being changed but before the
     * change is committed.
     * @param The \ref ValueMenuItem.
     */
	virtual void on_value_changing(ValueMenuItem *menuitem)
	{
	}


    /**
     * Invoked after the change in value of a \ref ValueMenuItem has been committed.
     * @param The \ref ValueMenuItem.
     */
	virtual void on_value_changed(ValueMenuItem *menuitem)
	{
	}

protected:
	uint8_t value_column = 10;  // How far to the right ValueMenuItem values should be.
};
#endif
