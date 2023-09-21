///	@file       generics/Ring.h
///	@class      Ring
///	@brief      A variable-length ring buffer.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef RING_H
#define RING_H

#include "ICollection.h"

/**
 * A circular buffer
 * @tparam T The underlying type of the queue.
 */
template <class T>
class Ring : public ICollection<T>
{
public:
#if GENERICS_ALLOW_NEW
	Ring(uint32_t size) : ICollection<T>(size)
	{
	}
#endif

	Ring(T* buffer, uint32_t size) : ICollection<T>(buffer, size)
	{
	}

    /**
     * Adds an item to the end of the ring.
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
            return false;

        this->buffer[this->length++] = value;
        return true;
    }

    /**
     * Returns the next item in the ring and moves the position forward.
     * @return The next item.
     */
    T next(void)
    {
        if (this->length == 0)
            return this->_default;

        position++;
        if (position == (int32_t)this->length)
            position = 0;
        return this->buffer[position];
    }

    /**
     * Returns the next item in the ring..
     * @return The next item.
     */
    T current(void)
    {
        if (this->length == 0)
            return this->_default;

        return this->buffer[position];
    }

    /**
     * Returns the previous item in the ring and moves the position backward.
     * @return The previous item.
     */
    T previous(void)
    {
        if (this->length == 0)
            return this->_default;

        position--;
        if (position == -1)
            position = this->length-1;
        return this->buffer[position];
    }

    /**
     * Returns the item at the home position in the ring, moving to that position.
     * @return The first item in the ring.
     */
    T first(void)
    {
        position = 0;
        return this->buffer[position];
    }


    /**
     * Goes to the first item that has the specified value.
     * @param value The value to go to.
     * @returns true if the value is found; otherise false.
     */
    bool find(T value)
    {
    	for (uint32_t p=0; p < this->length; p++)
    		if (this->buffer[p] == value)
    		{
    			position = p;
    			return true;
    		}
    	return false;
    }


    void clear(void) override
    {
    	ICollection<T>::clear();
    	position = 0;
    }


private:
    int32_t position = 0;  // Current position in the ring.
};
#endif

