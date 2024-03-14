/*
 * Conversion.h
 *
 *  Created on: Mar 14, 2024
 *      Author: YvanRodriguez
 */

#ifndef LIB_STM32_TOOLBOX_UTILITY_CONVERSION_H_
#define LIB_STM32_TOOLBOX_UTILITY_CONVERSION_H_

#include <stdint.h>


class Conversion
{
public:
	/**
	 * Converts LSB word into uint16.
	 * @param data Pointer to the first byte of the word.
	 * @return The value.
	 */
	static uint16_t lsb_uint16_to_uint16(uint8_t* data)
	{
		return data[0] | (data[1] << 8);
	}


	/**
	 * Converts uint8_t pointer to uint8_t.
	 * @param data Pointer to the byte.
	 * @return The value.
	 */
	static uint8_t lsb_uint8_to_uint8(uint8_t* data)
	{
		return *data;
	}


	/**
	 * Converts uint8_t pointer to uint8_t.
	 * @param data Pointer to the byte.
	 * @return The value.
	 */
	static int8_t lsb_int8_to_int8(uint8_t* data)
	{
		return *data;
	}


	/**
	 * Converts LSB word into int16.
	 * @param data Pointer to the first byte of the word.
	 * @return The value.
	 */
	static int16_t lsb_int16_to_int16(uint8_t* data)
	{
		return data[0] | data[1] << 8;
	}


	/**
	 * Converts LSB word into uint16.
	 * @param data Pointer to the first byte of the word.
	 * @return The value.
	 */
	static uint32_t lsb_uint32_to_uint32(uint8_t* data)
	{
		return data[0] | data[1] << 8 | data[2] << 16 | data[3];
	}


	/**
	 * Converts LSB word into int16.
	 * @param data Pointer to the first byte of the word.
	 * @return The value.
	 */
	static int32_t lsb_int32_to_int32(uint8_t* data)
	{
		return data[0] | data[1] << 8 | data[2] << 16 | data[3];
	}


	/**
	 * Converts LSB word with implicit decimal into float.
	 * @param data Pointer to the first byte of the word.
	 * @param divisor Value to divide the integer value by to translate it to float.
	 *        The default is 1 (zero decimal places).
	 * @return The value.
	 */
	static float lsb_int16_to_float(uint8_t* data, uint16_t divisor=1)
	{
		int16_t u = lsb_int16_to_int16(data);
		return ((float)u) / (float)divisor;
	}


	/**
	 * Converts LSB word with implicit decimal into float.
	 * @param data Pointer to the first byte of the word.
	 * @param divisor Value to divide the integer value by to translate it to float.
	 *        The default is 1 (zero decimal places).
	 * @return The value.
	 */
	static float lsb_uint16_to_float(uint8_t* data, uint16_t divisor=1)
	{
		uint16_t u = lsb_uint16_to_uint16(data);
		return ((float)u) / (float)divisor;
	}


	/**
	 * Converts LSB double word with implicit decimal into float.
	 * @param data Pointer to the first byte of the word.
	 * @param divisor Value to divide the integer value by to translate it to float.
	 *        The default is 1 (zero decimal places).
	 * @return The value.
	 */
	static float lsb_uint32_to_float(uint8_t* data, uint16_t divisor=1)
	{
		uint32_t u = lsb_uint32_to_uint32(data);
		return ((float)u) / (float)divisor;
	}
};

#endif /* LIB_STM32_TOOLBOX_UTILITY_CONVERSION_H_ */
