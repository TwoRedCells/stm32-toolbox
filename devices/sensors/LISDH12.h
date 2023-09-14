///	@file       devices/LISDH12.h
///	@class      LISDH12
///	@brief      Device driver for ST MEMS accelerometer.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef LIB_STM32_TOOLBOX_DEVICES_LISDH12_H_
#define LIB_STM32_TOOLBOX_DEVICES_LISDH12_H_

struct AccelerometerData
{
	int16_t x;
	int16_t y;
	int16_t z;
};


class LISDH12
{
public:
	static constexpr uint8_t ControlReg1_DataRate_PowerDown = 0x00;
	static constexpr uint8_t ControlReg1_DataRate_1Hz = 0x01 << 4;
	static constexpr uint8_t ControlReg1_DataRate_10Hz = 0x02 << 4;
	static constexpr uint8_t ControlReg1_DataRate_25Hz = 0x03 << 4;
	static constexpr uint8_t ControlReg1_DataRate_50Hz = 0x04 << 4;
	static constexpr uint8_t ControlReg1_DataRate_100Hz = 0x05 << 4;
	static constexpr uint8_t ControlReg1_DataRate_200Hz = 0x06 << 4;
	static constexpr uint8_t ControlReg1_DataRate_400Hz = 0x07 << 4;
	static constexpr uint8_t ControlReg1_DataRate_LowPower_1620Hz = 0x08 << 4;
	static constexpr uint8_t ControlReg1_DataRate_Normal_1334Hz = 0x09 << 4;
	static constexpr uint8_t ControlReg1_DataRate_LowPower_5376Hz = 0x09<< 4;
	static constexpr uint8_t ControlReg1_LowPowerMode_Enable = 0x01 << 3;
	static constexpr uint8_t ControlReg1_ZAxis_Enable = 0x01 << 2;
	static constexpr uint8_t ControlReg1_YAxis_Enable = 0x01 << 1;
	static constexpr uint8_t ControlReg1_XAxis_Enable = 0x01;

	LISDH12(I2C_HandleTypeDef* i2c, uint16_t address)
	{
		this->i2c = i2c;
		this->address = address << 1;
	}

	bool control1(uint8_t value)
	{
		return HAL_I2C_Mem_Write(i2c, address, 0x20, 1, &value, 1, timeout) == HAL_OK;
	}

	AccelerometerData poll(void)
	{
		uint8_t xl, xh, yl, yh, zl, zh;
		HAL_I2C_Mem_Read(i2c, address, 0x28, 1, &xl, 1, timeout);
		HAL_I2C_Mem_Read(i2c, address, 0x29, 1, &xh, 1, timeout);
		HAL_I2C_Mem_Read(i2c, address, 0x2a, 1, &yl, 1, timeout);
		HAL_I2C_Mem_Read(i2c, address, 0x2b, 1, &yh, 1, timeout);
		HAL_I2C_Mem_Read(i2c, address, 0x2c, 1, &zl, 1, timeout);
		HAL_I2C_Mem_Read(i2c, address, 0x2d, 1, &zh, 1, timeout);
		return AccelerometerData {
			.x = (int16_t)(xl | xh << 8),
			.y = (int16_t)(yl | yh << 8),
			.z = (int16_t)(zl | zh << 8)
		};
	}

private:
	I2C_HandleTypeDef* i2c;
	uint16_t address;
	uint32_t timeout = 100;
};

#endif /* LIB_STM32_TOOLBOX_DEVICES_LISDH12_H_ */
