///	@file       generics/List.h
///	@class      List
///	@brief      A variable-length generic list with external buffer.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef GENERIC_LIST_H
#define GENERIC_LIST_H

#include "ICollection.h"

/**
 * A list.
 * @tparam T The underlying type of the list.
 */
template <class T>
class List : public ICollection<T>
{
public:
#if GENERICS_ALLOW_NEW
	/**
	 * Default constructor for dynamic allocation without an initial length.
	 */
	List(void)
	{
	}


	/**
	 * Constructs a collection and pre-allocates an initial number of objects.
	 */
	List(uint32_t initial_length)
	{
		this->allocate(initial_length);
	}
#endif

	/**
	 * Constructor for passing a statically allocated buffer.
	 * @param buffer Pointer to the buffer.
	 * @param legnth The length of the buffer in objects (not bytes).
	 */
	List(T* buffer, uint32_t length) : ICollection<T>(buffer, length)
	{
	}


    /**
     * Adds an item to the end of the list.
     * @param value The value to add.
     * @return true if successful; otherwise false.
     */
    bool add(T value)
    {
        // No buffer set.
        if (this->buffer == NULL)
            return false;

        // Buffer overflow.
        if (this->length == this->buffer_length)
        {
#if GENERICS_ALLOW_NEW
        	if (!this->allocate())
        		return false;
#else
        	return false;
#endif
        }

        this->buffer[this->length] = value;
        this->length++;
        return true;
    }

    /**
     * Returns the specified item from the list.
     * @param index Index of the item to retrieve.
     * @return The item at the specified index.
     */
    T get(uint32_t index)
    {
    	return this->buffer[index];
    }


    /**
     * Returns the specified item from the list.
     * @param index Index of the item to retrieve.
     * @return The item at the specified index.
     */
    T operator[] (uint32_t index)
    {
    	return get(index);
    }
};
#endif

