/**
 * \file       generics/Ring.h
 * \class      Ring
 * \brief      A variable-length ring buffer.
 */

#ifndef RING_H
#define RING_H


/**
 * A circular buffer
 * @tparam T The underlying type of the queue.
 */
template <class T> class Ring
{
public:
    /**
     * Default constructor.
     * set_buffer() must be called before the ring can be used.
     */
    Ring()
    {
        buffer = NULL;
        length = 0;
        position = 0;
    }

    /**
     * Sets the underlying buffer for this Ring.
     * @param buffer Pointer to a buffer.
     * @param length The size allocated to the buffer.
     */
    void set_buffer(T* buffer, uint32_t length)
    {
        this->buffer = buffer;
        buffer_length = length;
    }

    /**
     * Adds an item to the end of the ring.
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
     * Returns the next item in the ring.
     * @return The next item.
     */
    T next(void)
    {
        if (length == 0)
            return _default;

        position++;
        if (position == length)
            position = 0;
        return buffer[position];
    }

    /**
     * Returns the next item in the ring and moves the position forward.
     * @return The next item.
     */
    T current(void)
    {
        if (length == 0)
            return _default;

        return buffer[position];
    }

    /**
     * Returns the previous item in the ring and moves the position forward.
     * @return The previous item.
     */
    T previous(void)
    {
        if (length == 0)
            return _default;

        position--;
        if (position == -1)
            position = length-1;
        return buffer[position];
    }

    /**
     * Returns the item at the home position in the ring, moving to that position.
     * @return The first item in the ring.
     */
    T first(void)
    {
        position = 0;
        return buffer[position];
    }

    /**
     * Returns the length of the queue.
     * @return The length of the queue.
     */
    uint32_t get_length(void) 
    {
        return length;
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
        position = 0;
        length = 0;
    }

private:
    T* buffer;  // Pointer to the buffer.
    uint32_t buffer_length;  // Length of the buffer.
    int32_t  position;  // Current position in the ring.
    uint32_t length;  // The current size of the Ring..
    T _default;  // Empty value.
};
#endif

