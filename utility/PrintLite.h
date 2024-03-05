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
#include "IWrite.h"
#include "utility/ImmutableString.h"

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
 *
 *			Usage:
 *			%% - The percent sign
 *			%c - An ASCII character (char)
 *			%s - A NUL-terminated string (char *)
 *			%d - An integer (uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t)
 *			%04d - An integer padded with preceding zeros to a width of 4 characters.
 *
 */
class PrintLite : public IWrite
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
		uint16_t count = 0;

		// Internal state.
		bool zero_padding = false;
		bool capitalize = false;
		uint8_t decimals = 0;
		int8_t fixed_width = Auto;


		while(char c = *format++)
		{
			if(c == '%')
			{
				for(bool formatting=true; formatting;)
				{
					switch(c = *format++)
					{
					case 's':                       // String
						write(va_arg(a, char*));
						formatting = false;
						break;
					case 'S':                       // ImmutableString
						write(va_arg(a, ImmutableString));
						formatting = false;
						break;
					case 'c':                       // Char
						write(va_arg(a, int));
						count++;
						formatting = false;
						break;
					case 'i':                       // signed integer
					case 'u':                       // unsigned integer
					case 'd':						// signed integer
					case 'l':                       // 32 bit long signed integer
					case 'n':                       // 32 bit long unsigned integer
					{
						int32_t n = va_arg(a, int32_t);
						if(c == 'l' && n < 0) n = -n, write('-');
						count += xtoa((uint32_t)n, fixed_width);
						fixed_width = Auto;
						zero_padding = false;
						formatting = false;
						break;
					}
					case 'X':
						capitalize = true;
					case 'x':                       // 16 bit heXadecimal
					{
						uint32_t u = va_arg(a, uint32_t);
						if (fixed_width == 8 || fixed_width == Auto)
						{
							puth(u >> 28 & 0xf, capitalize);
							puth(u >> 24 & 0xf, capitalize);
							puth(u >> 20 & 0xf, capitalize);
							puth(u >> 16 & 0xf, capitalize);
							count += 4;
						}
						if (fixed_width == 4 || fixed_width == 8 || fixed_width == Auto)
						{
							puth(u >> 12, capitalize);
							puth(u >> 8, capitalize);
							count += 2;
						}

						puth(u >> 4, capitalize);
						puth(u, capitalize);
						count += 2;

						fixed_width = Auto;
						zero_padding = false;
						formatting = false;
						capitalize = false;
						break;
					}

					case '0':
						zero_padding = true;
						break;
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						fixed_width = c - 0x30;
						break;
					case '.':
						c = *(format); // float
						decimals = c - 0x30;  // Number of digits to the right of the decimal.
						break;
					case 'f':
					{
						double f = va_arg(a, double);
						if (f < 0) f = -f, write('-'), count++;  // Negative.

						// Output the whole part of the number. If the number is zero, optionally output 0 depending on formatting specified.
						uint32_t whole = (uint32_t)f;
						if (whole == 0 && zero_padding)
							write('0'), count++;
						else
							count += xtoa(f, Auto, false);
						if (decimals > 0)
						{
							write('.'), count++;
							count += xtoa((f - (int16_t)f) * pow(10, decimals), decimals);
						}
						zero_padding = false;
						formatting = false;
						break;
					}
					case 0:
						return count;
					default:
						goto bad_fmt;
					}
				}
			}
			else
			{
				bad_fmt:
				count++;
				write(c);
			}
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
    template<typename... Args>
	static uint16_t vsprintf(char* buffer, const char *format, Args... args)
	{
		PrintLite lite(buffer);
		uint16_t ret = lite.printf(format, args...);
		buffer[ret] = 0; // NUL-termination.
		return ret;
	}

	/**
	 * @brief	Prints the specified string.
	 * @param	s The string.
	 * @param	nl Whether to add a newline at the end.
	 */
	uint16_t print(const char* s)
	{
		return write((char*)s);
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
	 * @brief Returns a to the power of b.
	 * @param a The base.
	 * @param b The exponent.
	 * @returns The power.
	 */
	static double pow(double a, double b)
	{
		double r = 1;
		for (uint32_t i=0; i<b; i++)
			r *= a;
		return r;
	}


	/**
	 * @brief	Writes the specified byte to the underlying resource.
	 * @param	c The byte value
	 */
	virtual size_t write(uint8_t b)
	{
		*p++ = b;
		return 1;
	}


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
	uint16_t write(ImmutableString string)
	{
		write(string.raw());
		return string.length();
	}


	/**
	 * @brief	Writes a NUL-terminated string.
	 * @param	string String to write.
	 */
	uint16_t write(const char *string)
	{
		uint16_t count = 0;
		const char *pp = string;
		while(*pp)
		{
			write((uint8_t)(*pp));
			pp++;
			count++;
		}
		if (p != nullptr)  // NUL terminate when using vsprintf.
			write((uint8_t)0);
		return count;
	}


protected:
	PrintLite(void)
	{
	}

	/**
	 * Converts an integer to a string.
	 * @param value The integer to convert.
	 * @param string Pointer to place the string.
	 */
	uint16_t xtoa(uint32_t value, int8_t digits=Auto, bool zero=true)
	{
		if (!zero && value == 0)
			return 0;

		if (digits == -1)
			digits = count_digits(value);
		uint32_t v = value;
		uint16_t count = 0;
		for (int8_t d=digits; d>0; d--)
		{
			uint32_t exp = pow(10, d-1);
			char x = v / exp;
			write('0'+x);
			count++;
			v -= x * exp;
		}
		return count;
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
	 * @param value The value to assess.
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
	void puth(uint8_t value, bool capitalize=false)
	{
		if (capitalize)
			write(hex_upper[value & 15]);
		else
			write(hex_lower[value & 15]);
	}

private:
	// This constructor and variables are used when this class is not subclassed.
	// This is only used for vsprintf, which needs slightly different handling.
	PrintLite(char* p)
	{
		this->p = p;
	}
	char* p = nullptr;
	static constexpr char hex_lower[16] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
	static constexpr char hex_upper[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	static constexpr const int8_t Auto = -1;
};

#endif /* INC_PRINT_HPP_ */
