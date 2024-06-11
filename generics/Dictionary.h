///	@file       generics/Dictionary.h
///	@class      Dictionary
///	@brief      A dictionary.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef GENERIC_DICTIONARY_H
#define GENERIC_DICTIONARY_H


/**
 * A dictionary.
 * @tparam T The underlying type of the key.
 * @tparam U The underlying type of the values.
 */
template <class T, class U> class Dictionary
{
public:
    /**
     * Default constructor.
     * set_buffer() must be called before the list can be used.
     */
	Dictionary()
    {
        keys = nullptr;
        values = nullptr;
        length = 0;
    }


    /**
     * Dynamic constructor allocates memory from the heap.
     * @param length The number of objects in the list.
     */
	Dictionary(uint32_t length)
    {
    	dynamic = true;
    	T* keys = (T*) malloc(length*sizeof(T));
    	U* values = (U*) malloc(length*sizeof(T));
    	set_buffers(keys, values, length);
    	this->length = 0;
    }


    ~Dictionary()
    {
    	if (dynamic)
    	{
    		free(keys);
    		free(values);
    	}
    }

    /**
     * Sets the internal buffer to the specified pointer.
     * @param buffer Pointer to the buffer.
     * @param length The length of the allocated buffer.
     */
    void set_buffers(T* keys, U* values, uint32_t length)
    {
        this->keys = keys;
        this->values = values;
        this->buffer_length = length;
    }

    /**
     * Adds an item to the end of the list.
     * @param value The value to add.
     * @return true if successful; otherwise false.
     */
    bool add(T key, U value)
    {
    	assert(keys != nullptr);
    	assert(values != nullptr);
		assert(length < buffer_length);

		if (!key_exists(key))
		{
			keys[length] = key;
			values[length] = value;
			length++;
			return true;
		}
    	return false;
    }


    /**
     * Checks whether the specified key exists.
     */
    bool key_exists(T key)
    {
    	for (uint32_t i=0; i<length; i++)
    		if (equals(keys[i], key))
    			return true;
    	return false;
    }

    /**
     * Returns the length of the list.
     * @return The length of the list.
     */
    uint32_t get_length(void) 
    {
        return length;
    }


    /**
     * Returns the specified item from the list.
     * @param key The key to retrieve.
     * @return The value of the specified key.
     */
    U get(T key)
    {
    	for (uint32_t i=0; i<length; i++)
    		if (equals(keys[i], key))
    			return values[i];
    	return _default;
    }


    /**
     * Sets the value of an existing key to a new value.
     * @param key	The key.
     * @param value	The new value.
     */
    void set(T key, U value)
    {
    	assert(key_exists(key));
    	uint32_t index = get_index(key);
    	values[index] = value;
    }


    /**
     * Gets the index of the specified key.
     * @param key	The key to search for.
     * @returns The index of the key's position if found, otherwise -1.
     */
    int32_t get_index(T key)
    {
    	for (uint32_t i=0; i<length; i++)
    		if (equals(keys[i], key))
    			return i;
    	return -1;
    }


    /**
     * Gets the buffer of keys.
     * @returns The keys.
     */
    T* get_keys(void)
    {
    	return keys;
    }


    /**
     * Gets the buffer of values.
     * @returns The values.
     */
    U* get_values(void)
    {
    	return values;
    }


    /**
     * Returns the specified item from the list.
     * @param key The key to retrieve.
     * @return The value of the specified key.
     */
    T operator[] (T key)
    {
    	return get(index);
    }

    /**
     * Returns the state of the queue.
     * @return true if the queue is empty; otherwise false.
     */
    bool is_empty()
    {
        return length == 0;
    }

    /**
     * Clears the queue, setting its length to zero.
     */
    void clear(void)
    {
        length = 0;
    }


protected:
    virtual bool equals(T a, T b)
    {
    	return a == b;
    }


private:
    T* keys;
    U* values;
    uint32_t buffer_length;  // Length of the buffer.
    uint32_t length;  // The current length of the queue.
    U _default = nullptr;  // Empty value.
    bool dynamic = false;  // True if the buffer was allocated at runtime.
};
#endif

