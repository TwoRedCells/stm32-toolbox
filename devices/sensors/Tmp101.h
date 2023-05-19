/**
 * \file       devices/Tmp101.h
 * \class      Tmp101
 * \brief      Interface to a TI TMP101 or similar digital thermometer.
 */

#ifndef INC_DEVICES_TMP101_H_
#define INC_DEVICES_TMP101_H_


#include "globals.hpp"
#include "constants.h"
#include "diagnostics/Log.h"
#include "comms/I2C.h"

class Tmp101 : public i2c
{
public:
    /**
     * Reads the temperature from the device.
     * @param A reference to the value to populate.
     * @return True on success; otherwise false.
     */
	bool read(float &value)
	{
		HAL_StatusTypeDef ret;

		static const uint8_t TMP101_ADDR = 0x48 << 1; // Use 7-bit address

		// Tell TMP102 that we want to read from the temperature register
		uint8_t buf[12] = "\0";
		//log_d("Sending I2C packet to %x... ", TMP101_ADDR);
		ret = HAL_I2C_Master_Transmit(handle, TMP101_ADDR, buf, 1, HAL_MAX_DELAY);
		if (ret != HAL_OK)
		{
			log_e("TMP101: I2C transmit failed.");
			return false;
		}

		// Read 2 bytes from the temperature register
		//log_d("Reading I2C packet from %x... ", TMP101_ADDR);
		ret = HAL_I2C_Master_Receive(handle, TMP101_ADDR, buf, 2, HAL_MAX_DELAY);
		if ( ret != HAL_OK )
		{
			log_e("TMP101: I2C receive failed.");
			return false;
		}

		//Combine the bytes
		int val = ((int16_t)buf[0] << 4) | (buf[1] >> 4);

		// Convert to 2's complement, since temperature can be negative
		if ( val > 0x7FF )
			val |= 0xF000;

		// Convert to float temperature value (Celsius)
		value = last = val * 0.0625;
		//log_d("Internal temperature is %.2f C.", value);
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
	float last;
};

#endif /* INC_DEVICES_TMP101_H_ */
