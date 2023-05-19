/**
 * @file		utility/StringBuilder.h
 * @class		StringBuilder
 * @brief		Allows a string to be built with one or more commands similar to writing to a stream.
 * @note		This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
 * 				firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
 * @copyright	See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE
 */

#ifndef INC_STM32_TOOLBOX_UTILITY_STRINGBUILDER_H_
#define INC_STM32_TOOLBOX_UTILITY_STRINGBUILDER_H_

//#include <stdarg.h>
#include <utility/PrintLite.h>
#include <toolbox.h>
#include "cmsis_os.h"


/**
 * @brief	Allows a string to be built with one or more commands similar to writing to a stream.
 */
class StringBuilder : public PrintLite
{
public:
#if ENABLE_STRINGBUILDER_MALLOC
	/**
	 * @brief	Constructs a new StringBuilder instance.
	 */
	StringBuilder(void)
	{
		buffer = (char*) malloc(STRINGBUILDER_BLOCK_SIZE);
		if (buffer == nullptr)
			trip_watchdog();

		this->size = size;
		buffer[0] = 0;  // NUL terminate.
	}


	/**
	 * @brief	Constructs a new StringBuilder instance.
	 * @param	size The initial size of the internal buffer.
	 */
	StringBuilder(uint32_t size)
	{
		buffer = (char*) malloc(size);
		if (buffer == nullptr)
			trip_watchdog();

		this->size = size;
		buffer[0] = 0;  // NUL terminate.
	}
#endif


	/**
	 * @brief	Constructs a new StringBuilder instance using a static buffer.
	 * @param	buffer Pointer to the buffer.
	 * @param	length Length of the buffer.
	 */
	StringBuilder(char* buffer, uint32_t length)
	{
		this->buffer = buffer;
		this->size = length;
		buffer[0] = 0;  // NUL terminate.
	}

#if ENABLE_STRINGBUILDER_MALLOC
	~StringBuilder()
	{
		if (buffer != nullptr)
			free(buffer);
	}
#endif

	/**
	 * @brief	Writes a byte to the string buffer.
	 * @param	c The byte to add to the buffer.
	 */
	void write(uint8_t c)
	{
		if (reset)
		{
			reset = false;
			length = 0;
			buffer[0] = 0;
		}

		length++;
		if (length+1 >= size)
		{
#if ENABLE_STRINGBUILDER_MALLOC
			size += BLOCKSIZE;
			buffer = (char*) realloc(buffer, size);
			if (buffer == nullptr)
				trip_watchdog();
#else
			trip_watchdog();
#endif
		}
		buffer[length-1] = c;
		buffer[length] = 0;
	}


	/**
	 * @brief	Returns the string (character buffer) and resets the length to zero.
	 * @note	The content of the returned buffer should be copied if persistence is needed.
	 * @returns Pointer to the generated string
	 */
	const char* out(void)
	{
		reset = true;
		return buffer;
	}


	/**
	 * @brief	Gets the current length of the string.
	 * @returns	The string length.
	 */
	uint32_t get_length(void)
	{
		return length;
	}


private:
	void trip_watchdog(void)
	{
		while(true);
	}

	char* buffer = nullptr;
	uint32_t size;  // Size of the buffer.
	uint32_t length=0;  // Current length of the string.
	bool reset = false;
};

#endif
