/**
 * 	@file       utility/print.h
 * 	@class      Print
 * 	@brief      An abstract class that can be inherited to provide minimalist printf functionality.
 *  @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
 *              firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
 *  @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE
 */


#ifndef INC_PRINT_HPP_
#define INC_PRINT_HPP_

#include <stdarg.h>
#include <stdint.h>
#include <math.h>

#define DEC 10
#define HEX 16
#define OCT 8  // Not supported.
#define BIN 2  // Not supported.


/// <summary>
/// An abstract class that can be inherited to provide minimalist printf functionality.
/// </summary>
/// <remarks>
/// The functionality of this class is optimized to be as small as possible for use with microcontrollers with limited
/// resources. As such, it should not be considered a substitute for a full printf function, either in terms of scope
/// of features, or compatibility.
///
/// As an abstract class, it has no value of its own. It must be inherited and the write method overwritten in order
/// to provide functionality. This may include writing to a file, stream, memory, or peripheral.
/// </remarks>


/**
 * @brief	An abstract class that can be inherited to provide minimalist printf functionality.
 * @remarks	The functionality of this class is optimized to be as small as possible for use with microcontrollers with limited
 *  		resources. As such, it should not be considered a substitute for a full printf function, either in terms of scope
 *			of features, or compatibility.
 */
class PrintLite
{
public:
	/**
	 * @brief	Outputs a formatted string.
	 * @param 	format A string that may include format specifiers.
	 * @param	... Value(s) to format.
	 * @returns The number of characters printed.
	 */
	uint16_t printf(const char *format, ...)
	{
		va_list a;
		va_start(a, format);
		int32_t n, dec;
		uint32_t u;
		uint16_t count = 0;
		double f;

		while(char c = *format++)
		{
			if(c == '%')
			{
				switch(c = *format++)
				{
				case 's':                       // String
					write(va_arg(a, char*));
					break;
				case 'c':                       // Char
					write(va_arg(a, int8_t));
					count++;
					break;
				case 'd':
				case 'i':                       // 16 bit signed integer
				case 'u':                       // 16 bit unsigned integer
					n = va_arg(a, int32_t);
					if(c == 'i' && n < 0) n = -n, write('-');
					count += xtoa((uint16_t)n, divisors + 5);
					break;
				case 'l':                       // 32 bit long signed integer
				case 'n':                       // 32 bit long unsigned integer
					n = va_arg(a, int32_t);
					if(c == 'l' && n < 0) n = -n, write('-');
					count += xtoa((uint32_t)n, divisors);
					break;
				case 'x':                       // 16 bit heXadecimal
					u = va_arg(a, uint32_t);
					puth(u >> 12);
					puth(u >> 8);
					puth(u >> 4);
					puth(u);
					count += 4;
					break;
				case 'y':                       // 8 bit heXadecimal
					u = va_arg(a, uint32_t);
					puth(u >> 4);
					puth(u);
					count += 2;
					break;
				case '.':						// float
					dec = *format++ - 0x30;
					f = va_arg(a, double);
					if(f < 0) f = -f, write('-');
					print((int16_t)f, DEC);
					if (dec > 0)
					{
						write('.');
						count++;
						print((f - (int16_t)f) * pow(10, dec), DEC);
					}
					format++;
					break;
				case 0:
					return count;
				default:
					goto bad_fmt;
				}
			}
			else
				bad_fmt:
			write(c);
		}
		va_end(a);
		return count;
	}


	/**
	 * @brief	Prints the specified string.
	 * @param	s The string.
	 */
	uint16_t print(const char* s)
	{
		return write((char*)s);
	}


	/**
	 * @brief	Prints the specified string.
	 * @param	s The string.
	 */
	uint16_t print(char* s)
	{
		return write(s);
	}


	/**
	 * @brief	Prints the supplied integer in decimal or hex notation, as specified.
	 * @param	c The integer value.
	 * @param	base 10 or 16.
	 */
	uint16_t print(unsigned int c, uint8_t base=HEX)
	{
		const char* format = base == HEX ? "%x" : "%u";
		return printf((char*)format, c);
	}


	/**
	 * @brief	Writes the specified byte to the underlying resource.
	 * @param	c The byte value
	 */
	virtual size_t write(uint8_t b) = 0;


	/**
	 * @brief	Writes the specified number of bytes from the provided buffer.
	 * @note	This method is safe for use with strings that are not NUL terminated.
	 * @param	buffer A pointer to the buffer
	 * @param	size The number of bytes to write.
	 */
	uint16_t write(const uint8_t *buffer, size_t size)
	{
		for (size_t i=0; i<size; i++)
			write(buffer[i]);
		return size;
	}


	/**
	 * @brief	Writes a NUL-terminated string.
	 * @param	string String to write.
	 */
	uint16_t write(const char *string)
	{
		uint16_t count = 0;
		const char *p = string;
		while(*p)
		{
			write((uint8_t)(*p));
			p++;
			count++;
		}
		return count;
	}

protected:
	/**
	 * Converts an integer to a string.
	 * @param value The integer to convert.
	 * @param string Pointer to place the string.
	 */
	uint16_t xtoa(uint32_t value, const uint32_t *string)
	{
		uint16_t count = 0;
		char c;
		uint32_t d;
		if(value)
		{
			while(value < *string) ++string;
			do
			{
				d = *string++;
				c = '0';
				while(value >= d)
					++c, value -= d;
				write(c);
				count++;
			} while(!(d & 1));
		}
		else
			write('0');
		return count;
	}


	static inline const uint32_t divisors[] = {
			//  4294967296      // 32 bit unsigned max
			1000000000,     // +0
			100000000,     // +1
			10000000,     // +2
			1000000,     // +3
			100000,     // +4
			//       65535      // 16 bit unsigned max
			10000,     // +5
			1000,     // +6
			100,     // +7
			10,     // +8
			1,     // +9
	};


	/**
	 * Prints the hex value (zero to A) corresponding to the specified value.
	 * @param value The value.
	 */
	void puth(uint8_t value)
	{
		static const char hex[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
		write(hex[value & 15]);
	}
};

#endif /* INC_PRINT_HPP_ */
