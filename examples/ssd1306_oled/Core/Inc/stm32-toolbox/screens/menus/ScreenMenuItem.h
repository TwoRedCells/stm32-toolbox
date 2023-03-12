/**
 * \file       screens/ScreenMenuItem.h
 * \class      ScreenMenuItem
 * \brief      A menu item that links to another screen when clicked.
 */

#ifndef INC_SCREENS_SCREENMENUITEM_H_
#define INC_SCREENS_SCREENMENUITEM_H_

#include "screens/menus/MenuItem.h"
#include "screens/Screen.h"


class ScreenMenuItem : public MenuItem
{
public:
	Screen* screen;
	static constexpr uint8_t TypeId = 0x02;


	/**
	 * Instantiates a \ref ScreenMenuItem.
	 * @param name The name of the menu item.
	 * @param screen Pointer to the screen that will open when this menu item is selected.
	 */
	ScreenMenuItem(const char* name, Screen* screen) : MenuItem(name)
	{
		this->screen = screen;
	}


    /**
     * Gets the unique type of this \ref MenuItem.
     * @return The TypeID
     */
	uint8_t get_typeid(void) override
	{
	    return ScreenMenuItem::TypeId;
	}
};


#endif /* INC_SCREENS_SCREENMENUITEM_H_ */
