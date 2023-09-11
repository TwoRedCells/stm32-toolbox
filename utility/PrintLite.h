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
		bool zero_before_decimal = false;

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
					write(va_arg(a, int));
					count++;
					break;
				case 'd':
				case 'i':                       // 16 bit signed integer
				case 'u':                       // 16 bit unsigned integer
					n = va_arg(a, int32_t);
					if(c == 'i' && n < 0) n = -n, write('-');
					count += xtoa((uint16_t)n);
					break;
				case 'l':                       // 32 bit long signed integer
				case 'n':                       // 32 bit long unsigned integer
					n = va_arg(a, int32_t);
					if(c == 'l' && n < 0) n = -n, write('-');
					count += xtoa((uint32_t)n);
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
				case '0':
					zero_before_decimal = true;
					format++;
				case '.':						// float
				{
					uint32_t dec = *format++ - 0x30;  // Number of digits to the right of the decimal.
					double f = va_arg(a, double);
					if (f < 0) f = -f, write('-'), count++;  // Negative.
					uint32_t whole = (uint32_t)f;
					if (count_digits(whole) == 0 && zero_before_decimal)
						write('0'), count++;
					count += xtoa(f);
					if (dec > 0)
					{
						write('.'), count++;
						count += xtoa((f - (int16_t)f) * pow(10, dec), dec);
					}
					format++;  // Discard the f.
					zero_before_decimal = false;
					break;
				}
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
	 * @brief	Outputs a formatted string.
	 * @param 	format A string that may include format specifiers.
	 * @param	... Value(s) to format.
	 * @returns The number of characters printed.
	 */
	static uint16_t vsprintf(char* buffer, const char *format, va_list a)
	{
		char* p = buffer;  // Pointer to current character.
		bool zero_before_decimal = false;

		while(char c = *format++)
		{
			if(c == '%')
			{
				switch(c = *format++)
				{
				case 's':                       // String
				{
					char* str = va_arg(a, char*);
					while (*str != '\0')
						*p++ = *str++;
					break;
				}
				case 'c':                       // Char
					*p++ = va_arg(a, int);
					break;
				case 'd':
				case 'i':                       // 16 bit signed integer
				case 'u':                       // 16 bit unsigned integer
				{
					int32_t n = va_arg(a, int32_t);
					if(c == 'i' && n < 0) n = -n, *p++ = '-';
					p += xtoa((uint16_t)n, p);
					break;
				}
				case 'l':                       // 32 bit long signed integer
				case 'n':                       // 32 bit long unsigned integer
				{
					int32_t n = va_arg(a, int32_t);
					if(c == 'l' && n < 0) n = -n, *p++ = '-';
					p += xtoa((uint32_t)n, p);
					break;
				}
				case 'x':                       // 16 bit heXadecimal
				{
					uint32_t u = va_arg(a, uint32_t);
					*p++ = hex[u >> 12 & 0xf];
					*p++ = hex[u >> 8 & 0xf];
					*p++ = hex[u >> 4 & 0xf];
					*p++ = hex[u & 0xf];
					break;
				}
				case 'y':                       // 8 bit heXadecimal
				{
					uint32_t u = va_arg(a, uint32_t);
					*p++ = hex[u >> 4 & 0xf];
					*p++ = hex[u & 0xf];
					break;
				}
				case '0':
					zero_before_decimal = true;
					format++;
				case '.':						// float
				{
					uint32_t dec = *format++ - 0x30;  // Number of digits to the right of the decimal.
					double f = va_arg(a, double);
					if (f < 0) f = -f, *p++ = '-';  // Negative.
					uint32_t whole = (uint32_t)f;
					if (count_digits(whole) == 0 && zero_before_decimal)
						*p++ = '0';
					p += xtoa(f, p);
					if (dec > 0)
					{
						*p++ = '.';
						p += xtoa((f - (int16_t)f) * pow(10, dec), p, dec);
					}
					format++;  // Discard the f.
					zero_before_decimal = false;
					break;
				}
				case 0:
					return p - buffer;
				default:
					goto bad_fmt;
				}
			}
			else
			{
				bad_fmt:
				*p++ = c;  // Add the character verbatim.
			}
		}
		va_end(a);
		*p = '\0';  // Terminate with NUL.
		return p - buffer;
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
	uint16_t xtoa(uint32_t value, int8_t digits=10)
	{
		uint32_t v = value;
		for (uint8_t d=digits; d>=0; d--)
		{
			uint32_t exp = pow(10, d);
			char x = v / exp;
			write('0'+x);
			v -= x * exp;
		}
	}


	/**
	 * Converts an integer to a string.
	 * @param value The integer to convert.
	 * @param string Pointer to place the string.
	 * @param digits Number of digits to force (10 for automatic).
	 */
	static uint16_t xtoa(uint32_t value, char *p, int8_t digits=Auto)
	{
		if (digits == -1)
			digits = count_digits(value);
		char* q = p;
		uint32_t v = value;
		for (int8_t d=digits-1; d>=0; d--)
		{
			uint32_t exp = pow(10, d);
			char x = v / exp;
			*q++ = '0'+x;
			v -= x * exp;
		}
		return digits;
	}


	/**
	 * Returns the number of digits in the specified value.
	 * @param value The value as assess.
	 * @returns The number of digits.
	 */
	static uint8_t count_digits(uint32_t value)
	{
		uint8_t d;
		for (d=0; value>0; d++)
			value /= 10;
		return d == 0 ? 1 : d;
	}


	/**
	 * Prints the hex value (zero to A) corresponding to the specified value.
	 * @param value The value.
	 */
	void puth(uint8_t value)
	{
		write(hex[value & 15]);
	}

private:
	static constexpr char hex[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	static constexpr const int8_t Auto = -1;
};

#endif /* INC_PRINT_HPP_ */
