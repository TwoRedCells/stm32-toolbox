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

#include <stdarg.h>
#include <stm32-toolbox/utility/Print.h>
#include <string.h>
#include "cmsis_os.h"


/**
 * @brief	Allows a string to be built with one or more commands similar to writing to a stream.
 */
class StringBuilder : public PrintPrint
{
	const uint32_t BLOCKSIZE = 0x100;

public:
	/**
	 * @brief	Constructs a new StringBuilder instance.
	 */
	StringBuilder(void)
	{
		buffer = (char*) malloc(BLOCKSIZE);
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
		this->size = size;
		buffer[0] = 0;  // NUL terminate.
	}


	~StringBuilder()
	{
		free(buffer);
	}


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
			size += BLOCKSIZE;
			buffer = (char*) realloc(buffer, size);
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


private:
	char* buffer;
	uint32_t size;
	uint32_t length=0;
	bool reset = false;
};

#endif
