/**
 * \file       screens/MenuItem.h
 * \class      MenuItem
 * \brief      Base class for an item in a menu.
 */

#ifndef INC_SCREENS_MENUITEM_H_
#define INC_SCREENS_MENUITEM_H_

#include <string.h>
#include <stdint.h>

class MenuItem
{
public:
	const char* name;
	uint8_t length;
	uint8_t position = 0;
	uint8_t index;

	/**
	 * Virtual constructor.
	 * @param name The text to appear in the menu.
	 */
	MenuItem(const char* name)
	{
		this->name = name;
		this->length = strlen(name);
	}


	/**
	 * Invoked when this menu item has been pressed.
	 */
	virtual void on_pressed(void)
	{
	}


	/**
	 * Gets the unique type of this \ref MenuItem.
	 * @return The type ID.
	 */
	virtual uint8_t get_typeid(void) = 0;
};



#endif /* INC_SCREENS_MENUITEM_H_ */
