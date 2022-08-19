/**
 * \file       generics/Queue.h
 * \class      Queue
 * \brief      A variable-length FIFO queue with external buffer.
 */

#ifndef QUEUE_H
#define QUEUE_H


/**
 * A queue with circular pointers.
 * @tparam T The underlying type of the queue.
 */
template <class T> class Queue
{
public:
    /**
     * Default constructor.
     * set_buffer() must be called before the queue can be used.
     */
    Queue()
    {
        buffer = NULL;
        first = 0;
        next = 0;
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
        first = 0;
    }

    /**
     * Adds an item to the end of the queue.
     * @param value The value to add.
     * @return true if successful; otherwise false.
     */
    bool enqueue(T value)
    {
        // No buffer set.
        if (buffer == NULL)
            return false;

        // Buffer overflow.
        if (length == buffer_length)
            return false;

        buffer[next++] = value;
        if (next == buffer_length)
            next = 0;

        length++;
        return true;
    }

    /**
     * Getches an item from the front of the queue.
     * @return The next item.
     */
    T dequeue(void)
    {
        // Nothing to return.
        if (length == 0)
            return T();

        T value = buffer[first++];
        if (first == buffer_length)
            first = 0;

        length--;
        return value;
    }

    /**
     * Returns the item at the front of the queue, without removing it from the queue.
     * @return The next item.
     */
    T peek(void)
    {
        // Nothing to return.
        if (length == 0)
            return _default;

        return buffer[first];
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
        first = 0;
        next = 0;
        length = 0;
    }

private:
    T* buffer;  // Pointer to the buffer.
    uint32_t buffer_length;  // Length of the buffer.
    uint32_t first, next;  // Positions of the head of the queue and the next available position.
    uint32_t length;  // The current length of the queue.
    T _default;  // Empty value.
};
#endif

