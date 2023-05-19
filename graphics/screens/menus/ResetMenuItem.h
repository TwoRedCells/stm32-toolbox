/**
 * \file       screens/ScreenMenuItem.h
 * \class      ScreenMenuItem
 * \brief      A menu item that resets the microcontroller when pressed.
 */

#ifndef INC_SCREENS_RESETMENUITEM_H_
#define INC_SCREENS_RESETMENUITEM_H_

#include "screens/menus/MenuItem.h"
#include "screens/Screen.h"
#include "cmsis_os.h"

class ResetMenuItem : public MenuItem
{
public:
    static constexpr uint8_t TypeId = 0x02;

    // Inherit public constructor.
    using MenuItem::MenuItem;

    /**
     * Invoked to reset the microcontroller when pressed.
     */
	void on_pressed(void) override
	{
	    void HAL_NVIC_SystemReset();
	}


	/**
	 * Gets the unique type ID of this menu item.
	 * @return
	 */
    uint8_t get_typeid(void) override
    {
        return ScreenMenuItem::TypeId;
    }
};


#endif /* INC_SCREENS_SCREENMENUITEM_H_ */
