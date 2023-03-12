///	@file       devices/sensors/Mpu9250.h
///	@class      Mpu9250
///	@brief      Driver for MPU-9250 IMUs.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE
/// @source		http://www.lucidarme.me/?p=5057

#ifndef INC_STM32_TOOLBOX_DEVICES_MPU9250_H_
#define INC_STM32_TOOLBOX_DEVICES_MPU9250_H_

#include <stdint.h>

class Mpu9250
{
public:
	static constexpr uint8_t GyroFullScale250Dps = 0x00;
	static constexpr uint8_t GyroFullScale500Dps = 0x08;
	static constexpr uint8_t GyroFullScale1000Dps = 0x10;
	static constexpr uint8_t GyroFullScale2000Dps = 0x18;
	static constexpr uint8_t AccelFullScale2g = 0x00;
	static constexpr uint8_t AccelFullScale4g = 0x08;
	static constexpr uint8_t AccelFullScale8g = 0x10;
	static constexpr uint8_t AccelFullScale16g = 0x18;

	typedef struct
	{
		int16_t ax;
		int16_t ay;
		int16_t az;
		int16_t gx;
		int16_t gy;
		int16_t gz;
		int16_t mx;
		int16_t my;
		int16_t mz;
		int16_t t;
	} ImuData;

	Mpu9250(I2C_HandleTypeDef* handle, uint8_t address, uint8_t gyroscale=GyroFullScale1000Dps, uint8_t accelscale=AccelFullScale4g, uint32_t timeout=10)
	{
		this->handle = handle;
		this->address = address<<1;
		this->timeout = timeout;

		// Set low-pass filters to 5 Hz.
		write_register(0x1d, 0x06);
		write_register(0x1a, 0x06);

		// Set sensor ranges.
		write_register(0x1b, gyroscale);
		write_register(0x1c, accelscale);

		// Set magnetometer bypass.
		write_register(0x37, 0x02);

		// Request continuous magnetometer measurements in 16 bits.
		write_register(0x0a, 0x16);
	}

	ImuData* read_values(void)
	{
		uint8_t mag_address = 0x02 << 1;
		static ImuData sensor;
		uint8_t buf[14];
		HAL_I2C_Mem_Read(handle, address, 0x3b, 1, buf, 14, timeout);
		sensor.ax = -(buf[0]<<8 | buf[1]);
		sensor.ay = -(buf[2]<<8 | buf[3]);
		sensor.az = buf[4]<<8 | buf[5];
		sensor.gx = -(buf[8]<<8 | buf[9]);
		sensor.gy = -(buf[10]<<8 | buf[11]);
		sensor.gz = buf[12]<<8 | buf[13];
		sensor.t = -(buf[6]<<8 | buf[7]);

		// Wait for DRDY.
		for (uint8_t st=1; !(st&1); )
			HAL_I2C_Mem_Read(handle, mag_address, 0x02, 1, &st, 4, timeout);

		HAL_I2C_Mem_Read(handle, mag_address, 0x03, 1, buf, 6, timeout);
		sensor.mx = -(buf[3]<<8 | buf[2]) + 200;
		sensor.my = -(buf[1]<<8 | buf[0]) - 70;
		sensor.mz = (buf[5]<<8 | buf[4]) - 700;
		return &sensor;
	}

protected:
	void write_byte(uint8_t byte)
	{
		HAL_I2C_Master_Transmit(handle, address<<1, &byte, 1, timeout);
	}

	void write_register(uint8_t _register, uint8_t value)
	{
		uint8_t data[2] { _register, value };
		HAL_I2C_Master_Transmit(handle, address, data, 2, timeout);
	}

private:
	I2C_HandleTypeDef* handle;
	uint8_t address;
	uint32_t timeout;
};



#endif /* INC_STM32_TOOLBOX_DEVICES_MPU9250_H_ */
