#ifndef _DS18B10_h
#define _DS18B10_h


/**
 ******************************************************************************
 * @file           : OneWire.h
 * @brief          : HAL library to work with OneWire bus
 ******************************************************************************
 * @attention
 * Copyright 2021 Konstantin Toporov
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom 
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR 
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE 
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ********************* Description *************************
 * This library use the library OneWire to communicate with DS18B20 
 * temperature sensors.
 * 
 * Create Ds18B20_t structure and pass it to Init function.
 * @attention there is no need to create OneWire_t structure
 */

#include <comms/OneWire_UART.h>

/**
 * Maxumum devices that could be used. Define more before including this file
 */
#ifndef MAX_DS18B20_SENSORS
#define MAX_DS18B20_SENSORS 3
#endif

///Use this to select all devices on the bus
#define DS18B20_MEASUREALL 0xff

/** Since the minimal temperature that the sensor can measure is -55 degrees centigade
 * (raw value -880) this constants is used to indicate the errors
 */
#define DS18B20_TEMP_NOT_READ -1000
#define DS18B20_TEMP_ERROR -1500
#define DS18B20_TEMP_CRC_ERROR -1550

/**
 *  The precision of the sensor and therefor the time for converting the temperature
 */
#define DS18B20_12BITS 0b01111111 //750ms
#define DS18B20_11BITS 0b01011111 //375ms
#define DS18B20_10BITS 0b00111111 //187.5ms
#define DS18B20_9BITS  0b00011111 //93.75ms


/* OneWire commands */
#define DS18B20_CMD_READSCRATCHPAD		0xBE
#define DS18B20_CMD_WRITESCRATCHPAD		0x4E
#define DS18B20_CMD_CPYSCRATCHPAD		0x48
#define DS18B20_CMD_RECEEPROM			0xB8
#define DS18B20_CMD_RPWRSUPPLY			0xB4
#define DS18B20_CMD_SEARCHROM			0xF0
#define DS18B20_CMD_READROM				0x33
#define DS18B20_CMD_MATCHROM			0x55
#define DS18B20_CMD_SKIPROM				0xCC

#define DS18B20_COVERTTEMP 0x44


class DS18B20 : public OneWire
{
public:

	DS18B20(UART_HandleTypeDef *huart) : OneWire(huart)
	{
	}


	void initialize(uint8_t precision)
	{
		uint8_t status = first();

		//Looking for the sensors while there are avalable and their amount could be stored
		while (status && sensors_found < MAX_DS18B20_SENSORS)
		{
			//Save all ROMs
			get_full_rom(ROMS[sensors_found]);
			//Check the CRC
			if (crc8(ROMS[sensors_found], 7) == ROMS[sensors_found][7])
				sensors_found++;
			//Looking for the next
			status = next();
		}

		uint8_t data[] = {
				OW_CMD_SKIPROM,
				OW_CMD_WSCRATCHPAD,
				0x7F, //0b0111 1111 //temp high
				0xFF, //0b1111 1111 //temp low
				precision
		};

		if (reset())
		{
			//Select all sensors. It's faster
			send_bytes(data, sizeof(data));
		}

		switch (precision)
		{
		case DS18B20_11BITS : timeNeeded = 380; break;
		case DS18B20_10BITS : timeNeeded = 195; break;
		case DS18B20_9BITS  : timeNeeded = 100; break;
		case DS18B20_12BITS :
		default:
			timeNeeded = 760; break;
		}
	}


	uint8_t get_number_of_sensors(void)
	{
		return sensors_found;
	}


	void start_measure(uint8_t sensor)
	{
		if (sensor < sensors_found || sensor == DS18B20_MEASUREALL)
		{
			uint32_t now = HAL_GetTick();
			if (sensor == DS18B20_MEASUREALL)
			{
				if (reset())
				{
					//Select all sensors. It's faster
					send_byte(OW_CMD_SKIPROM);
					send_byte(DS18B20_COVERTTEMP);
					for(uint8_t i=0;i<sensors_found;i++)
						lastTimeMeasured[i] = now;
				}
			}
			else
			{
				if (reset())
				{
					select(ROMS[sensor]);
					send_byte(DS18B20_COVERTTEMP);
					lastTimeMeasured[sensor] = now;
				}
			}
		}
	}

	uint8_t is_temperature_ready(uint8_t sensor)
	{
		if (sensor != DS18B20_MEASUREALL && sensor >= sensors_found)
			return 0;

		if (sensor == DS18B20_MEASUREALL)
			sensor = 0;

		return ((HAL_GetTick() - lastTimeMeasured[sensor]) >= timeNeeded);
	}


	int16_t get_raw_temperature(uint8_t sensor=0)
	{
		if (sensor != DS18B20_MEASUREALL && sensor >= sensors_found)
			return DS18B20_TEMP_NOT_READ;

		if (!reset())
			return DS18B20_TEMP_NOT_READ;

		if (sensor == DS18B20_MEASUREALL)
			send_byte(DS18B20_CMD_SKIPROM);
		else
			select(ROMS[sensor]);

		send_byte(OW_CMD_RSCRATCHPAD);

		uint8_t data[12];
		uint16_t s = 0;

		for (uint8_t j = 0; j < 9; j++)
		{           // we need 9 bytes
			data[j] = receive_byte();
			s += data[j];
		}
		//The CRC algorithm has an error. If all bytes are zeros the CRC will be ok
		//So this check is agains it
		if (s == 0)
			return DS18B20_TEMP_CRC_ERROR;

		if (crc8(data, 8) != data[8])
			return DS18B20_TEMP_ERROR;

		//temp calculation
		int16_t raw = (data[1] << 8) | data[0];
		uint8_t cfg = (data[4] & 0x60);

		// at lower res, the low bits are undefined, so let's zero them
		if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
		else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
		else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
		//// default is 12 bit resolution, 750 ms conversion time
		return raw + correction[sensor == DS18B20_MEASUREALL ? 0 : sensor];
	}

	bool read(float& value, uint8_t sensor=0)
	{
		int16_t raw = get_raw_temperature(sensor);
		if (raw < -100)
			return false;

		value = (float) DS18B20_convertToDouble(raw);
		return true;
	}

	void set_correction(uint8_t sensor, int16_t cor)
	{
		if (sensor>=sensors_found)
			return;
		correction[sensor] = cor;
	}

	///Convertion of the raw value to degrees centigare
	inline static double DS18B20_convertToDouble(int16_t t)
	{
		return (double)t * 0.0625;
	};

	///Convertion of degrees centigare to raw value
	inline static int16_t DS18B20_convertToInt(double t)
	{
		return (int16_t)((t/0.0625)+0.5);
	};

	uint8_t sensors_found = 0;
	int16_t correction[MAX_DS18B20_SENSORS] = {0};
	uint32_t lastTimeMeasured[MAX_DS18B20_SENSORS] = {0};
	uint8_t ROMS[MAX_DS18B20_SENSORS][8] = {0};
	uint16_t timeNeeded = 0;
};
#endif
