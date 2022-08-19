/**
 * \file       screens/Menu.h
 * \class      Menu
 * \brief      Encapsulate a simple menu, which is a list of menu items.
 */

#include <string.h>
#include "generics/List.h"
#include "screens/menus/MenuItem.h"


#ifndef INC_SCREENS_MENU_H_
#define INC_SCREENS_MENU_H_

#define BUFFER_LENGTH    (10) // Max menu items

typedef uint8_t menu_orientation;

class Menu : public List<MenuItem*>
{
public:
	uint8_t active_index = 0;  // The active menu item.
	uint8_t first_index = 0;  // FIrst visible menu item.


	/**
	 * Instantiates a menu.
	 * @note #define the buffer length to the maximum number of menu items you will encounter.
	 */
	Menu(void)
	{
		set_buffer(buffer, BUFFER_LENGTH);
	}


	/**
	 * Moves the active index to the previous menu item.
	 */
	void previous(void)
	{
		active_index = active_index == 0 ? active_index : active_index-1;
	}


    /**
     * Moves the active index to the next menu item.
     */
	void next(void)
	{
		uint8_t l = get_length();
		active_index = active_index == l-1 ? active_index : active_index+1;
	}

private:
	MenuItem* buffer[BUFFER_LENGTH];
};

#endif /* INC_SCREENS_MENU_H_ */
