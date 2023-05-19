///	@file       devices/OledSsd1306.h
///	@class      OledSsd1306
///	@brief      Controls an OLED display driven by the ssd1306 driver.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_STM32_TOOLBOX_DEVICES_OLEDSSD1306_H_
#define INC_STM32_TOOLBOX_DEVICES_OLEDSSD1306_H_

#include "utility/Print.h"
#include <fonts/Font6x8.h>
#include <stdint.h>
#include <memory.h>

class OledSsd1306 : public Print
{
public:
	static constexpr uint8_t MemoryMode = 0x20;
	static constexpr uint8_t ColumnAddr = 0x21;
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

	enum Colour { Black, White, Inverse };

	static constexpr uint32_t DefaultTimeout = 100000;


	/**
	 * @brief	Initializes this instance.
	 * @param	i2c A pointer to the i2c handle.
	 */
	OledSsd1306(I2C_HandleTypeDef* i2c, uint16_t i2cadr, uint8_t width=128, uint8_t height=64)
	{
		this->i2c = i2c;
		this->i2cadr = i2cadr << 1;
		this->width = width;
		this->height = height;

//		command(DisplayOff);
//		command(SetDisplayClockDiv, 0x80);
//		command(SetMultiplex);
//		//command(height - 1);
//		command(SetDisplayOffset, 0x00);
//		command(SetStartLine);
//		command(ChargePump);
//		command(MemoryMode, 0x00);
//		command(SegRemap | 0x01);
//		command(ComScanDec);
//		command(SetComPins, 0x12);
//		command(SetContrast, 0x9f);
//		command(SetPreCharge, 0x22);
//		command(SetVcomDetect, 0x40);
//		command(DisplayAllOnResume);
//		command(NormalDisplay);
//		command(DeactivateScroll);
//		command(DisplayOn);



	}

	void initialize(void)
	{
		uint8_t init_commands[] = {
			DisplayOff,
			SetDisplayClockDiv, 0x80,
			SetMultiplex, 0x3f,
			SetDisplayOffset, 0x00,
			SetStartLine,
			ChargePump, 0x14,
			MemoryMode, 0x00,
			SegRemap | 0x01,
			ComScanDec,
			SetComPins, 0x12,
			SetContrast, 0xcf,
			SetPreCharge, 0xf1,
			SetVcomDetect, 0x40,
			DisplayAllOnResume,
			NormalDisplay,
			DisplayOn
		};
		HAL_I2C_Mem_Write(i2c, i2cadr, 0x00, 1, init_commands, sizeof(init_commands), timeout);

		pixels[0] = 0xff;
		pixels[1] = 0xff;
		pixels[2] = 0xff;
		pixels[3] = 0xff;
		pixels[4] = 0xff;
		pixels[5] = 0xff;
	}

	void pixel(uint16_t x, uint16_t y, Colour colour)
	{
		uint16_t address = y / 8 * 0x80 + x;
		if (colour == Black) pixels[address] &= ~1 << y%8;
		else pixels[address] |= 1 << (y%8);
	}

	void rectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool fill=false)
	{
		if (fill)
		{
			for (uint8_t b=y; b<y+h; b++)
				for (uint8_t a=x; a<x+w; a++)
					pixel(a, b, colour);
		}
		else
		{
			hline(x, w-1, y);
			hline(x, w-1, y+h-1);
			vline(x, y, h-1);
			vline(x+w-1, y, h-1);
		}
	}

	void hline(uint8_t x, uint8_t w, uint8_t )
	{
		for (uint8_t a=x; a<x+w; a++)
			pixel(a, y, colour);
	}


	void vline(uint8_t x, uint8_t y, uint8_t h)
	{
		for (uint8_t b=y; b<y+h; b++)
			pixel(x, b, colour);
	}

	void fill(void)
	{
		for (uint16_t i=0; i < sizeof(pixels); i++)
			pixels[i] = 0x55;
		refresh();
	}

	void clear(void)
	{
		for (uint16_t i=0; i < sizeof(pixels); i++)
			pixels[i] = 0x00;
		refresh();
	}

	void refresh(void)
	{
		command(MemoryMode, 0x00);
		command(PageAddr, 0, 7);
		command(ColumnAddr, 0, width-1);

		//HAL_I2C_Mem_Write_DMA(i2c, i2cadr, 0x40, 1, pixels, sizeof(pixels));
		HAL_I2C_Mem_Write(i2c, i2cadr, 0x40, 1, pixels, 1024, timeout);
	}

	void string(uint8_t x, uint8_t y, const char* str)
	{
		this->x = x;
		this->y = y;

		for (const char* s=str; *s != 0; s++)
		{
			uint8_t c = *s;
			write(c);
		}

	}

	void string(const char* str)
	{
		string(x, y, str);
	}

	void set_colour(Colour colour)
	{
		this->colour = colour;
	}

	void move(uint8_t x, uint8_t y)
	{
		this->x = x;
		this->y = y;
	}


private:

	size_t write(uint8_t ch)
	{
		if (ch == '\r')
		{
			x = 0;
		}
		else if (ch == '\n')
		{
			y += font6x8.height;
		}
		else
		{
			uint8_t* glyph = font6x8.glyphs[ch];
			for (uint8_t i=0; i<8; i++)
				for (uint8_t j=0; j<8; j++)
					if (glyph[i] & (1 << j))
						pixel(x+j, i+y, colour);
			x += font6x8.width;
		}
	}

	void command(uint8_t cmd)
	{
		HAL_I2C_Mem_Write(i2c, i2cadr, 0x00, 1, &cmd, 1, timeout);
	}

	void command(uint8_t cmd, uint8_t arg)
	{
		uint8_t msg[2] { cmd, arg };
		HAL_I2C_Mem_Write(i2c, i2cadr, 0x00, 1, msg, 2, timeout);
	}

	void command(uint8_t cmd, uint8_t arg1, uint8_t arg2)
	{
		uint8_t msg[3] { cmd, arg1, arg2 };
		HAL_I2C_Mem_Write(i2c, i2cadr, 0x00, 1, msg, 3, timeout);
	}

//	void command(uint8_t cmd)
//	{
//		HAL_I2C_Mem_Write(i2c, i2cadr, cmd, sizeof(uint8_t), 0, 0, timeout);
//	}
//
//	void command(uint8_t cmd, uint8_t arg)
//	{
//		HAL_I2C_Mem_Write(i2c, i2cadr, cmd, sizeof(uint8_t), &arg, sizeof(uint8_t), timeout);
//	}
//
//	void command(uint8_t cmd, uint8_t arg1, uint8_t arg2)
//	{
//		uint8_t msg[2] { arg1, arg2 };
//		HAL_I2C_Mem_Write(i2c, i2cadr, cmd, sizeof(uint8_t), msg, sizeof(uint8_t)*2, timeout);
//	}

	I2C_HandleTypeDef* i2c;
	uint16_t width, height;
	uint16_t i2cadr;
	uint32_t timeout = DefaultTimeout;
	uint8_t pixels[128/8*64];
	uint8_t x=0, y=0;
	Colour colour = White;
	Font6x8 font6x8;
};



#endif /* INC_STM32_TOOLBOX_DEVICES_OLEDSSD1306_H_ */
