/*
  IPAddress.h - Base class that provides IPAddress
  Copyright (c) 2011 Adrian McEwen.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef IPAddress_h
#define IPAddress_h

#include <stdint.h>
#include <memory.h>
#include "utility/ImmutableString.h"
#include "utility/PrintLite.h"

// A class to make it easier to handle and pass around IP addresses

class IPAddress
{
public:
	IPAddress()
	{
		_address.dword = 0;
	}

	IPAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet)
	{
		_address.bytes[0] = first_octet;
		_address.bytes[1] = second_octet;
		_address.bytes[2] = third_octet;
		_address.bytes[3] = fourth_octet;
	}

	IPAddress(uint32_t address)
	{
		_address.dword = address;
	}

	IPAddress(const uint8_t *address)
	{
		memcpy(_address.bytes, address, sizeof(_address.bytes));
	}

	IPAddress(ImmutableString address)
	{
		uint16_t acc = 0; // Accumulator
		uint8_t dots = 0;

		const char*s = address.raw();
		while (*s)
		{
			char c = *s++;
			if (c >= '0' && c <= '9')
			{
				acc = acc * 10 + (c - '0');
				if (acc > 255)
				{
					// Value out of [0..255] range
					is_valid = false;
					return;
				}
			}
			else if (c == '.')
			{
				if (dots == 3)
				{
					// Too much dots (there must be 3 dots)
					is_valid = false;
					return;
				}
				_address.bytes[dots++] = acc;
				acc = 0;
			}
			else
			{
				// Invalid char
				is_valid = false;
				return;
			}
		}

		if (dots != 3)
		{
			// Too few dots (there must be 3 dots)
			is_valid = false;
			return;
		}
		_address.bytes[3] = acc;
		is_valid = true;
	}

	// Overloaded copy operators to allow initialisation of IPAddress objects from other types
	IPAddress& operator=(const uint8_t *address)
	{
		memcpy(_address.bytes, address, sizeof(_address.bytes));
		return *this;
	}

	IPAddress& operator=(uint32_t address)
	{
		_address.dword = address;
		return *this;
	}

	bool operator==(const uint8_t* addr) const
	{
		return memcmp(addr, _address.bytes, sizeof(_address.bytes)) == 0;
	}



	// Overloaded cast operator to allow IPAddress objects to be used where a pointer
	// to a four-byte uint8_t array is expected
	operator uint32_t() const { return _address.dword; };
	bool operator==(const IPAddress& addr) const { return _address.dword == addr._address.dword; };

	// Overloaded index operator to allow getting and setting individual octets of the address
	uint8_t operator[](int index) const { return _address.bytes[index]; };
	uint8_t& operator[](int index) { return _address.bytes[index]; };


	// Access the raw byte array containing the address.  Because this returns a pointer
	// to the internal structure rather than a copy of the address this function should only
	// be used when you know that the usage of the returned uint8_t* will be transient and not
	// stored.
	uint8_t* raw_address() { return _address.bytes; };

	char* to_string(void)
	{
		PrintLite::vsprintf(string_value, "%d.%d.%d.%d", _address.bytes[0], _address.bytes[1], _address.bytes[2], _address.bytes[3]);
		return string_value;
	}

private:
	union {
		uint8_t bytes[4];  // IPv4 address
		uint32_t dword;
	} _address;

	char string_value[16];
	bool is_valid = true;
};

const IPAddress INADDR_NONE(0,0,0,0);

#endif
