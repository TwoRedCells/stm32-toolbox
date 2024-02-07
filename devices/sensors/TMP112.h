/**
 * \file       devices/Tmp101.h
 * \class      Tmp101
 * \brief      Interface to a TI TMP101 or similar digital thermometer.
 */

#ifndef INC_DEVICES_TMP112_H_
#define INC_DEVICES_TMP112_H_


#include "toolbox.h"
#include "diagnostics/Log.h"
#include "i2c.h"


class TMP112
{
public:
	static constexpr uint8_t TemperatureRegister = 0;  // read-only
	static constexpr uint8_t ConfigurationRegister = 1;
	static constexpr uint8_t LowerLimitRegister = 2;
	static constexpr uint8_t UpperLimitRegister = 3;

	TMP112(I2C_HandleTypeDef *hi2c, uint8_t address)
	{
		this->hi2c = hi2c;
		assert(address >= 0x48 && address <= 0x4b);
		this->address = address << 1;  // Make it a 7-bit address
	}

    /**
     * Reads the temperature from the device.
     * @param A reference to the value to populate.
     * @return True on success; otherwise false.
     */
	bool read(double &value)
	{
		HAL_StatusTypeDef ret;

		uint8_t buf[2] = { 0 };
		ret = HAL_I2C_Mem_Read(hi2c, address, TemperatureRegister, 2, buf, 2, HAL_MAX_DELAY);
		if (ret != HAL_OK)
			return false;

		//Combine the bytes
		int val = ((int16_t)buf[0]) << 4 | buf[1] >> 4;

		// Convert to 2's complement, since temperature can be negative
		if (val > 0x7FF )
			val |= 0xF000;

		// Convert to float temperature value (Celsius)
		value = last = (double)val * 0.0625;
		return true;
	}


	/**
	 * Gets the last reported temperature, without requesting an update.
	 * @return The last temperature
	 *
     */
	float get_last(void)
	{
		return last;
	}

private:
	double last;
	I2C_HandleTypeDef *hi2c;
	uint8_t address;
};

#endif /* INC_DEVICES_TMP101_H_ */
