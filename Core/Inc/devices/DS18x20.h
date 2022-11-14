///	@file       devices/DS18x20.h
///	@class      DS18x20
///	@brief      Encapsulates communications with DS18x20 temperature measurement devices.
/// @note       See the DS18B20 datasheet for details how this class is implemented.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_SENSORS_DS18B20_H_
#define INC_SENSORS_DS18B20_H_

#include <math.h>
#include "stm32-toolbox/comms/OneWire.h"


struct DS18x20_ROM_Read_Response
{
	uint8_t family;
	uint8_t serial[6];
	uint8_t crc;
};

struct DS18x20_Scratchpad
{
	uint8_t temperature_lsb;
	uint8_t temperature_msb;
	uint8_t th_register;
	uint8_t tl_register;
	uint8_t configuration_register;
	uint8_t reserved0;
	uint8_t reserved1;
	uint8_t reserved2;
	uint8_t crc;
};


class DS18x20 : public OneWire
{
public:
	static const uint8_t FamilyCode_DS18B20 = 0x28;
	static const uint8_t FamilyCode_DS18S20 = 0x10;
	static const uint8_t ROM_Read = 0x33;
	static const uint8_t ROM_Match = 0x55;
	static const uint8_t ROM_Search = 0xf0;
	static const uint8_t ROM_AlarmSearch = 0xec;
	static const uint8_t ROM_Skip = 0xcc;
	static const uint8_t Func_ConvertTemperature = 0x44;
	static const uint8_t Func_CopyScratchpad = 0x48;
	static const uint8_t Func_WriteScratchpad = 0x4e;
	static const uint8_t Func_ReadScratchpad = 0xbe;
	static const uint8_t Func_RecallEeprom = 0xb8;
	static const uint8_t Func_ReadPowerSupply = 0xb4;
	static const uint8_t Resolution9bits = 0b00011111;
	static const uint8_t Resolution10bits = 0b00111111;
	static const uint8_t Resolution11bits = 0b01011111;
	static const uint8_t Resolution12bits = 0b01111111;

	bool crc_ok;

	using OneWire::OneWire;

	/**
	 * Gets the current temperature from the hardware.
	 * @param value A reference to the variable to fill.
	 * @return True on success; otherwise false.
	 */
	bool get_temperature(float &value)
	{
		if (!reset())
		{
			//log_e("Slave not responding on OneWire network.");
			return false;
		}

		rom_read();
		if (rom.family != DS18x20::FamilyCode_DS18B20)
		{
			//log_e("DS18B20 reported family code is incorrect.");
			return false;
		}

        //log_d("%x   %x:%x:%x:%x:%x:%x   %x", rom.family, rom.serial[0], rom.serial[1], rom.serial[2], rom.serial[3], rom.serial[4], rom.serial[5], rom.crc);
		if (!crc_ok)
		{
			//log_e("rom_read failed CRC check.");
			//log_d("%x   %x:%x:%x:%x:%x:%x   %x", rom.family, rom.serial[0], rom.serial[1], rom.serial[2], rom.serial[3], rom.serial[4], rom.serial[5], rom.crc);
			return false;
		}

		convert_temperature();
		osDelay(750);

		osKernelLock();
		reset();
		rom_skip();
		read_scratchpad();
        //log_d("%x:%x   %x:%x   %x   %x:%x:%x   %x", scr.temperature_lsb, scr.temperature_msb, scr.th_register, scr.tl_register, scr.configuration_register, scr.reserved0, scr.reserved1, scr.reserved2, scr.crc);
		if (!crc_ok)
		{
			//log_e("read_scratchpad failed CRC check.");
			//log_d("%x:%x   %x:%x   %x   %x:%x:%x   %x", scr.temperature_lsb, scr.temperature_msb, scr.th_register, scr.tl_register, scr.configuration_register, scr.reserved0, scr.reserved1, scr.reserved2, scr.crc);
			return false;
		}

		int16_t raw = get_temperature_raw();
		osKernelUnlock();
		bool negative = raw & (0b11110000 << 8);
		if (negative)
			raw |= (0b11110000 << 8);
		value = raw * 0.0625;
		if (negative)
			value *= -1;

		return true;
	}

private:
	/**
	 * Sends the ROM READ message then populates the structure with the results.
	 */
	void rom_read(void)
	{
		write_byte(DS18x20::ROM_Read);
		rom.family = read_byte();
		rom.serial[0] = read_byte();
		rom.serial[1] = read_byte();
		rom.serial[2] = read_byte();
		rom.serial[3] = read_byte();
		rom.serial[4] = read_byte();
		rom.serial[5] = read_byte();
		rom.crc = read_byte();
		crc_ok = crc8((uint8_t*)&rom, 7) == rom.crc;
	}


	/**
	 * Tells the device that we would like to skip unnecessary communications.
	 * This is useful when there is only one device on the network.
	 */
	void rom_skip(void)
	{
		write_byte(DS18x20::ROM_Skip);
	}


	/**
	 * Sends the convert temperature command to the device.
	 * The data structure will be ready to read subsequently.
	 */
	void convert_temperature(void)
	{
		write_byte(DS18x20::Func_ConvertTemperature);
	}


	/**
	 * Reads the data from the devices memory.
	 */
	void read_scratchpad(void)
	{
		write_byte(DS18x20::Func_ReadScratchpad);
		scr.temperature_lsb = read_byte();
		scr.temperature_msb = read_byte();
		scr.th_register = read_byte();
		scr.tl_register = read_byte();
		scr.configuration_register = read_byte();
		scr.reserved0 = read_byte();  // always 0xff.
		scr.reserved1 = read_byte();
		scr.reserved2 = read_byte();  // always 0x10.
		scr.crc = read_byte();
		crc_ok = crc8((uint8_t*)&scr, 8) == scr.crc && scr.reserved0 == 0xff && scr.reserved2 == 0x10;
	}


	/**
	 * Gets the raw temperature as reported by the hardware.
	 * @return The temperature.
	 */
	uint16_t get_temperature_raw(void)
	{
		return scr.temperature_lsb | (scr.temperature_msb << 8);
	}


	/**
	 * Calculates the CRC of the specified data using a CRC-8 check with the polynomial.
	 * @param data Pointer to the data to check.
	 * @param length Number of bytes of data.
	 * @return The CRC-8 calculation.
	 */
	uint8_t crc8(const uint8_t *data, uint8_t length)
	{
		uint8_t crc = 0x00;
		uint8_t extract;
		uint8_t sum;
		for(int i=0; i< length; i++)
		{
			extract = *data;
			for (uint8_t tempI = 8; tempI; tempI--)
			{
				sum = (crc ^ extract) & 0x01;
				crc >>= 1;
				if (sum)
					crc ^= crc_polynomial_reversed;
				extract >>= 1;
			}
			data++;
		}
		return crc;
	}


	DS18x20_ROM_Read_Response rom;
	DS18x20_Scratchpad scr;
	uint8_t crc_polynomial_reversed = 0x8c;
};

#endif /* INC_SENSORS_DS18B20_H_ */
