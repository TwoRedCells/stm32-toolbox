///	@file       generics/ICollection.h
///	@class      ICollection
///	@brief      Interface for a collection of objects.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef LIB_STM32_TOOLBOX_GENERICS_ICOLLECTION_H_
#define LIB_STM32_TOOLBOX_GENERICS_ICOLLECTION_H_

#include "toolbox.h"


#if GENERICS_ALLOW_NEW
#include <malloc.h>
#endif

template <class T>
class ICollection
{
public:
#if GENERICS_ALLOW_NEW
	/**
	 * Default constructor for dynamic allocation without an initial length.
	 */
	ICollection(void)
	{
	}


	/**
	 * Constructs a collection and pre-allocates an initial number of objects.
	 */
	ICollection(uint32_t initial_length)
	{
		allocate(initial_length);
	}
#endif

	/**
	 * Constructor for passing a statically allocated buffer.
	 * @param buffer Pointer to the buffer.
	 * @param legnth The length of the buffer in objects (not bytes).
	 */
	ICollection(T* buffer, uint32_t length)
	{
		this->buffer = buffer;
		this->buffer_length = length;
	}

	~ICollection()
	{
		if (buffer != nullptr)
			free(buffer);
	}

#if GENERICS_ALLOW_NEW
	/**
	 * Allocates space for the collection.
	 * @param objects The number of objects to allocate.
	 */
	bool allocate(uint32_t objects)
	{
		buffer_length = objects * sizeof(T);
		if (buffer == nullptr)
			buffer = (T*) malloc(objects);
		else
			buffer = (T*) realloc(buffer, objects);
		if (buffer == nullptr)
			return false;
		buffer_length = objects;
		return true;
	}


	/**
	 * Increases the allocation by one.
	 */
	bool allocate(void)
	{
		return allocate(buffer_length+1);
	}
#endif

//    /**
//     * Sets the underlying buffer for this Ring.
//     * @param buffer Pointer to a buffer.
//     * @param length The size allocated to the buffer.
//     */
//    void set_buffer(T* buffer, uint32_t length)
//    {
//        this->buffer = buffer;
//        buffer_length = length;
//    }


    /**
     * Returns the length of the collection.
     * @return The length of the collection.
     */
    virtual uint32_t get_length(void)
    {
        return length;
    }

    /**
     * Returns the state of the collection.
     * @return true if the queue is empty; otherwise false.
     */
    virtual bool is_empty()
    {
        return length == 0;
    }

    /**
     * Clears the collection, setting its length to zero.
     */
    virtual void clear(void)
    {
        length = 0;
    }


protected:
    T* buffer = nullptr;  // Pointer to the buffer.
    uint32_t buffer_length = 0;  // Length of the buffer.
    T _default;
    uint32_t length = 0;
};

#endif /* LIB_STM32_TOOLBOX_GENERICS_ICOLLECTION_H_ */
