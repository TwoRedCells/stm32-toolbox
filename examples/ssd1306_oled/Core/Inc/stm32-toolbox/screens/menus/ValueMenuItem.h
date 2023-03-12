/**
 * \file       screens/ValueMenuItem.h
 * \class      ValueMenuItem
 * \brief      A menu item having a value that can be altered when clicked.
 */

#ifndef INC_SCREENS_VALUEMENUITEM_H_
#define INC_SCREENS_VALUEMENUITEM_H_

#include "screens/menus/MenuItem.h"

class ValueMenuItem : public MenuItem
{
public:
	enum types { Int, Uint, Float, Bool };
	types value_type;
	float value = 0;
	uint8_t decimals = 0;
	float increment = 1.0;
	const char* true_string = "On";
	const char* false_string = "Off";
	static constexpr uint8_t TypeId = 0x03;


	/**
	 * Instantiates a \ref ValueMenuItem.
	 * @param name The name of the menu item.
	 * @param value_type The type of the underlying menu item from the \ref types enum.
	 */
	ValueMenuItem(const char* name, ValueMenuItem::types value_type) : MenuItem(name)
	{
		this->value_type = value_type;
	}

	/**
	 * Retrieves the TypeID that uniquely identifies the type of this class.
	 * @return The TypeID.
	 */
	uint8_t get_typeid(void) override
	{
		return TypeId;
	}


	/**
	 * Gets the current value, cast as an int16_t.
	 * @return The current value.
	 */
	int16_t get_int(void)
	{
	    return (int16_t)value;
	}

    /**
     * Gets the current value, cast as a uint16_t.
     * @return The current value.
     */
	uint16_t get_uint(void)
	{
	    return (uint16_t)value;
	}

    /**
     * Gets the current value, cast as a float.
     * @return The current value.
     */
	float get_float(void)
	{
	    return value;
	}

    /**
     * Gets the current value, cast as a bool.
     * @return The current value.
     */
	bool get_bool(void)
	{
	    return value < 0.1 && value > -0.1 ? false : true;
	}
};


#endif /* INC_SCREENS_SCREENMENUITEM_H_ */
