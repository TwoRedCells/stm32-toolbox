/*
 * Crc.h
 *
 *  Created on: Jan 25, 2024
 *      Author: YvanRodriguez
 */

#ifndef LIB_STM32_TOOLBOX_UTILITY_CRC_H_
#define LIB_STM32_TOOLBOX_UTILITY_CRC_H_

#include <stdint.h>


class Crc
{
public:

	static uint16_t crc16_modbus(const void* buffer, uint32_t len, uint16_t start=0xffff)
	{
		uint8_t* buf = (uint8_t*) buffer;
		uint16_t crc = start;
		unsigned int i = 0;
		char bit = 0;

		for (i = 0; i < len; i++)
		{
			crc ^= buf[i];

			for (bit = 0; bit < 8; bit++)
			{
				if (crc & 0x0001)
				{
					crc >>= 1;
					crc ^= 0xa001;
				}
				else
				{
					crc >>= 1;
				}
			}
		}

		return crc;
	}

	static uint32_t crc32(const void* buffer, uint32_t len, uint32_t polynomial, uint32_t start=0)
	{
		uint8_t* buf = (uint8_t*) buffer;
		uint32_t crc = ~start;
		unsigned int i = 0;
		char bit = 0;

		for (i = 0; i < len; i++)
		{
			crc ^= buf[i];

			for (bit = 0; bit < 8; bit++)
			{
				if (crc & 0x0001)
				{
					crc >>= 1;
					crc ^= polynomial;
				}
				else
				{
					crc >>= 1;
				}
			}
		}

		return ~crc;
	}

	static uint32_t crc32_ethernet(const void* buffer, uint32_t len, uint32_t start=0)
	{
		return crc32(buffer, len, 0xedb88320, start);
	}
};


#endif /* LIB_STM32_TOOLBOX_UTILITY_CRC_H_ */
