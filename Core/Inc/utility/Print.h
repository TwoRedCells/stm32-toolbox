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
#include <stdlib.h>
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
class PrintPrint
{
public:
	/**
	 * @brief	Outputs a formatted string.
	 * @param 	format A string that may include format specifiers.
	 * @param	... Value(s) to format.
	 */
	void printf(const char *format, ...)
	{
		va_list a;
		va_start(a, format);

		int i, dec;
		long n;
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
					write(va_arg(a, int));
					break;
				case 'i':                       // 16 bit Integer
				case 'u':                       // 16 bit Unsigned
					i = va_arg(a, int);
					if(c == 'i' && i < 0) i = -i, write('-');
					xtoa((unsigned)i, divisors + 5);
					break;
				case 'l':                       // 32 bit Long
				case 'n':                       // 32 bit uNsigned loNg
					n = va_arg(a, long);
					if(c == 'l' && n < 0) n = -n, write('-');
					xtoa((unsigned long)n, divisors);
					break;
				case 'x':                       // 16 bit heXadecimal
					i = va_arg(a, int);
					puth(i >> 12);
					puth(i >> 8);
					puth(i >> 4);
					puth(i);
					break;
				case '.':						// float
					dec = *format++ - 0x30;
					f = va_arg(a, double);
					if(f < 0) f = -f, write('-');
					print((int)f, DEC);
					write('.');
					print((f - (int)f) * pow(10, dec), DEC);
					format++;
					break;
				case 0: return;
				default: goto bad_fmt;
				}
			}
			else
				bad_fmt:
				write(c);
		}
		va_end(a);
	}


	/**
	 * @brief	Prints the specified string.
	 * @param	s The string.
	 */
	void print(const char* s)
	{
		write((char*)s);
	}


	/**
	 * @brief	Prints the specified string.
	 * @param	s The string.
	 */
	void print(char* s)
	{
		write(s);
	}


	/**
	 * @brief	Prints the supplied integer in decimal or hex notation, as specified.
	 * @param	c The integer value.
	 * @param	base 10 or 16.
	 */
	void print(unsigned int c, uint8_t base=HEX)
	{
		const char* format = base == HEX ? "%x" : "%u";
		printf((char*)format, c);
	}


	/**
	 * @brief	Writes the specified byte to the underlying resource.
	 * @param	c The bye value
	 */
	virtual void write(uint8_t b) = 0;


	/**
	 * @brief	Writes the specified number of bytes from the provided buffer.
	 * @note	This method is safe for use with strings that are not NUL terminated.
	 * @param	buffer A pointer to the buffer
	 * @param	size The number of bytes to write.
	 */
	void write(const uint8_t *buffer, size_t size)
	{
		for (size_t i=0; i<size; i++)
			write(buffer[i]);
	}


	/**
	 * @brief	Writes a NUL-terminated string.
	 * @param	s String to write.
	 */
	void write(const char *s)
	{
		const char *p = s;
		while(*p)
		{
			write((uint8_t)(*p));
			p++;
		}
	}

protected:
	void xtoa(unsigned long x, const unsigned long *dp)
	{
		char c;
		unsigned long d;
		if(x)
		{
			while(x < *dp) ++dp;
			do
			{
				d = *dp++;
				c = '0';
				while(x >= d) ++c, x -= d;
				write(c);
			} while(!(d & 1));
		} else
			write('0');
	}

	static inline const unsigned long divisors[] = {
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

	void puth(unsigned n)
	{
		static const char hex[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
		write(hex[n & 15]);
	}
};

#endif /* INC_PRINT_HPP_ */
