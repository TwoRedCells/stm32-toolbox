/**
 * \file       generics/List.h
 * \class      List
 * \brief      A variable-length generic list with external buffer.
 */

#ifndef GENERIC_LIST_H
#define GENERIC_LIST_H


/**
 * A list.
 * @tparam T The underlying type of the list.
 */
template <class T> class List
{
public:
    /**
     * Default constructor.
     * set_buffer() must be called before the list can be used.
     */
    List()
    {
        buffer = NULL;
        length = 0;
    }

    /**
     * Sets the internal buffer to the specified pointer.
     * @param buffer Pointer to the buffer.
     * @param length The length of the allocated buffer.
     */
    void set_buffer(T* buffer, uint32_t length)
    {
        this->buffer = buffer;
        this->buffer_length = length;
    }

    /**
     * Adds an item to the end of the list.
     * @param value The value to add.
     * @return true if successful; otherwise false.
     */
    bool add(T value)
    {
        // No buffer set.
        if (buffer == NULL)
            return false;

        // Buffer overflow.
        if (length == buffer_length)
            return false;

        buffer[length++] = value;
        return true;
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
     * Returns a pointer to the buffer.
     */
    T* get_buffer(void)
    {
        return buffer;
    }

    /**
     * Returns the specified item from the list.
     * @param index Index of the item to retrieve.
     * @return The item at the specified index.
     */
    T get(uint32_t index)
    {
    	return buffer[index];
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

private:
    T* buffer;  // Pointer to the buffer.
    uint32_t buffer_length;  // Length of the buffer.
    uint32_t length;  // The current length of the queue.
    T _default;  // Empty value.
};
#endif

