///	@file       devices/OledSsd1306.h
///	@class      OledSsd1306
///	@brief      Controls an OLED display driven by the ssd1306 driver.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_STM32_TOOLBOX_DEVICES_OLEDSSD1306_H_
#define INC_STM32_TOOLBOX_DEVICES_OLEDSSD1306_H_


#include <stdint.h>
#include "main.h"

class OledSsd1306
{
public:
	static constexpr uint8_t MemoryMode = 0x20;
	static constexpr uint8_t ColumnAdd = 0x21;
	static constexpr uint8_t PageAddr = 0x22;
	static constexpr uint8_t SetContrast = 0x81;
	static constexpr uint8_t ChargePump = 0x8d;
	static constexpr uint8_t SegRemap = 0xa0;
	static constexpr uint8_t DisplayAllOnResume = 0xa4;
	static constexpr uint8_t DisplayAllOn = 0xa5;
	static constexpr uint8_t NormalDisplay = 0xa6;
	static constexpr uint8_t InvertDisplay = 0xa7;
	static constexpr uint8_t SetMultiplex = 0xa8;
	static constexpr uint8_t DisplayOff = 0xae;
	static constexpr uint8_t DisplayOn = 0xaf;
	static constexpr uint8_t ComScanInc = 0xc0;
	static constexpr uint8_t ComScanDec = 0xc8;
	static constexpr uint8_t SetDisplayOffset = 0xd3;
	static constexpr uint8_t SetDisplayClockDiv = 0xd5;
	static constexpr uint8_t SetPreCharge = 0xd9;
	static constexpr uint8_t SetComPins = 0xda;
	static constexpr uint8_t SetVcomDetect = 0xdb;
	static constexpr uint8_t SetLowColumn = 0x00;
	static constexpr uint8_t SetHighColumn = 0x10;
	static constexpr uint8_t SetStartLine = 0x40;
	static constexpr uint8_t ExternalVcc = 0x01;
	static constexpr uint8_t SwitchCapVcc = 0x02;
	static constexpr uint8_t RightHorizontalScroll = 0x26;
	static constexpr uint8_t LeftHorizontalScroll = 0x27;
	static constexpr uint8_t VerticalAndRightHorizontalScroll = 0x29;
	static constexpr uint8_t VerticalAndLeftHorizontalScroll = 0x2a;
	static constexpr uint8_t DeactivateScroll = 0x2e;
	static constexpr uint8_t ActivateScroll = 0x2f;
	static constexpr uint8_t SetVerticalScrollArea = 0xa3;

	static constexpr uint16_t Black = 0x00;
	static constexpr uint16_t White = 0x01;
	static constexpr uint16_t Inverse = 0x02;

	static constexpr uint32_t DefaultTimeout = 100000;


	/**
	 * @brief	Initializes this instance.
	 * @param	i2c A pointer to the i2c handle.
	 */
	void initialize(I2C_HandleTypeDef* i2c, uint16_t i2cadr, uint8_t width=128, uint8_t height=64)
	{
		this->i2c = i2c;
		this->i2cadr = i2cadr;
		this->width = width;
		this->height = height;

		command(DisplayOff);
		command(SetDisplayClockDiv, 0x80);
		command(SetMultiplex);
		command(height - 1);
		command(SetDisplayOffset, 0x00);
		command(SetStartLine);
		command(ChargePump);
		command(MemoryMode, 0x00);
		command(SegRemap | 0x01);
		command(ComScanDec);
		command(SetComPins, 0x12);
		command(SetContrast, 0x9f);
		command(SetPreCharge, 0x22);
		command(SetVcomDetect, 0x40);
		command(DisplayAllOnResume);
		command(NormalDisplay);
		command(DeactivateScroll);
		command(DisplayOn);

	}

	void pixel(uint16_t x, uint16_t y, uint16_t colour)
	{
		command(PageAddr, 0x00, 0xff);
		command(ColumnAdd, 0x00, width-1);
		command(ColumnAdd, 0x40);
	}

	void test(void)
	{
		command(PageAddr, 0x00, 0xff);
		command(ColumnAdd, 0x00, width-1);
		command(0x40);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
		command(0x55);
	}

	void command(uint8_t cmd)
	{
		HAL_I2C_Master_Transmit(i2c, i2cadr, &cmd, sizeof(uint8_t), timeout);
	}

	void command(uint8_t cmd, uint8_t arg)
	{
		uint8_t msg[2] { cmd, arg };
		HAL_I2C_Master_Transmit(i2c, i2cadr, msg, sizeof(uint8_t)*2, timeout);
	}

	void command(uint8_t cmd, uint8_t arg1, uint8_t arg2)
	{
		uint8_t msg[3] { cmd, arg1, arg2 };
		HAL_I2C_Master_Transmit(i2c, i2cadr, msg, sizeof(uint8_t)*3, timeout);
	}






private:
	I2C_HandleTypeDef* i2c;
	uint16_t width, height;
	uint16_t i2cadr;
	uint32_t timeout = DefaultTimeout;
};



#endif /* INC_STM32_TOOLBOX_DEVICES_OLEDSSD1306_H_ */
