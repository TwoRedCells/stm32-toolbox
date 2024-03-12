#ifndef _MAC_ADDRESS_H
#define _MAC_ADDRESS_H

#include <stdint.h>
#include <memory.h>
#include "utility/PrintLite.h"
#include "utility/ImmutableString.h"


class MacAddress
{
public:
	MacAddress(uint8_t aa, uint8_t bb, uint8_t cc, uint8_t dd, uint8_t ee, uint8_t ff)
	{
		mac[0] = aa;
		mac[1] = bb;
		mac[2] = cc;
		mac[3] = dd;
		mac[4] = ee;
		mac[5] = ff;
	}


	MacAddress(const uint8_t *address)
	{
		memcpy(this->mac, address, 6);
	}

	MacAddress(ImmutableString address)
	{
		assert(address[17] == 0);
		for (int i=0; i<6; i++)
		{
			mac[i] = hex_to_byte(address[3*i+0]) << 4;
			mac[i] |= hex_to_byte(address[3*i+1]);
		}
	}


	bool operator==(const uint8_t* address) const
	{
		return memcmp(address, mac, 6) == 0;
	}


	uint8_t* raw(void) { return mac; };

	char* to_string(void)
	{
		PrintLite::vsprintf(string_value, "%2x:%2x:%2x:%2x:%2x:%2x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		return string_value;
	}

	bool is_empty(void)
	{
		return !mac[0] && !mac[1] && !mac[2] && !mac[3] && !mac[4] && !mac[5];
	}

	bool is_broadcast(void)
	{
		return mac[0] == 0xff && mac[1] == 0xff && mac[2] == 0xff && mac[3] == 0xff && mac[4] == 0xff && mac[5] == 0xff;
	}

private:
	uint8_t hex_to_byte(char c)
	{
		if (c >= 0x41 && c <= 0x46) return c - 0x37;
		if (c >= 0x61 && c <= 0x66) return c - 0x57;
		if (c >= 0x30 && c <= 0x39) return c - 0x30;
		return 0;
	}

	uint8_t mac[6] = {0};
	char string_value[18];

};

#endif
