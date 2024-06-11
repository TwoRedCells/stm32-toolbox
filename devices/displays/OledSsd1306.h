///	@file       devices/OledSsd1306.h
///	@class      OledSsd1306
///	@brief      Controls an OLED display driven by the ssd1306 driver.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_STM32_TOOLBOX_DEVICES_OLEDSSD1306_H_
#define INC_STM32_TOOLBOX_DEVICES_OLEDSSD1306_H_

#include "utility/PrintLite.h"
#if OLED_SSD1306_ENABLE_TEXT
#ifndef PROGMEM
#define PROGMEM
#endif
#include "graphics/GFXFont.h"
#endif
#include "i2c.h"
#include <stdint.h>
#include <stdarg.h>
#include "toolbox.h"


class OledSsd1306 : public PrintLite
{
public:
	static constexpr uint8_t MemoryMode = 0x20;
	static constexpr uint8_t ColumnAddr = 0x21;
	static constexpr uint8_t PageAddr = 0x22;
	static constexpr uint8_t SetContrast = 0x81;
	static constexpr uint8_t ChargePump = 0x8d;
	static constexpr uint8_t EnableChargePump = 0x14;
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

	static constexpr uint32_t DefaultTimeout = 250;

	/**
	 * @brief	Initializes this instance.
	 * @param	i2c A pointer to the i2c handle.
	 */
	OledSsd1306(I2C_HandleTypeDef* i2c, uint16_t i2cadr)
	{
		this->i2c = i2c;
		this->i2cadr = i2cadr << 1;
		this->width = OLED_SSD1306_WIDTH;
		this->height = OLED_SSD1306_HEIGHT;
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
	}

	void pixel(uint16_t x, uint16_t y, bool colour)
	{
		if (((rotation == 0 || rotation == 180) && (x > width-1 || y > height-1))
			|| ((rotation == 90 || rotation == 270) && (x > height-1 || y > width-1)))
			return;
		// Translate
		uint16_t a=x, b=y;
		if (rotation == 180)
		{
			a = width - x;
			b = height - y - 1;
		}
		else if (rotation == 90)
		{
			a = y;
			b = height - x - 1;
		}
		else if (rotation == 270)
		{
			a = width - y - 1;
			b = x;
		}

		uint16_t address = b / 8 * width + a;
		if (!colour) pixels[address] &= ~(1 << (b%8));
		else pixels[address] |= 1 << (b%8);
	}

	void rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool fill=false)
	{
		if (fill)
		{
			for (uint16_t b=y; b<y+h; b++)
				hline(x, b, w-1);
		}
		else
		{
			hline(x, y, w-1);
			hline(x+1, y+h-1, w-1);
			vline(x, y+1, h-1);
			vline(x+w-1, y, h-1);
		}
	}

	void hline(uint16_t x, uint16_t y, uint16_t w)
	{
		for (uint16_t a=x; a<x+w; a++)
			pixel(a, y, colour);
	}


	void vline(uint16_t x, uint16_t y, uint16_t h)
	{
		for (uint16_t b=y; b<y+h; b++)
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

#if OLED_SSD1306_ENABLE_TEXT
	void clear_line(void)
	{
		bool c = colour;
		set_colour(!c);
		rectangle(x, y, width-x, font->yAdvance+1, true);
		set_colour(c);
	}
#endif

	void refresh(void)
	{
		command(MemoryMode, 0x00);
		command(PageAddr, 0, 7);
		command(ColumnAddr, 0, width-1);
		HAL_I2C_Mem_Write(i2c, i2cadr, 0x40, 1, pixels, width/8 * height, timeout);
	}

	void set_contrast(uint8_t value)
	{
		command(SetContrast, value);
	}

	void set_precharge(uint8_t value)
	{
		command(SetPreCharge, value);
	}

	void set_charge_pump(uint8_t value)
	{
		command(ChargePump, value);
		command(EnableChargePump);
		command(DisplayOn);
	}


	void set_colour(bool colour)
	{
		this->colour = colour;
	}


	void move(uint16_t x, uint16_t y)
	{
		this->x = x;
		this->y = y;
	}


#if OLED_SSD1306_ENABLE_TEXT
	size_t write(uint8_t ch) override
	{
		uint8_t width = font->glyph[' '-font->first].xAdvance;
		if (ch == '\r')
		{
			x = 0;
		}
		else if (ch == '\t')
		{
			x += width/2;
		}
		else if (ch == '\n')
		{
			y += font->yAdvance+1;
		}
		else if (ch == '\b')
		{
			clear_line();
		}
		else if (ch == '\v')
		{
			y += (font->yAdvance+1) / 2 - 1;
		}
		else
		{
			uint8_t bit=0, bits=0;
			uint8_t c = ch - font->first;  // De-index the character
			GFXglyph glyph = font->glyph[c];
			uint16_t bitmap_offset = glyph.bitmapOffset;
			for (uint16_t i=0; i<glyph.height; i++)
				for (uint16_t j=0; j<glyph.width; j++)
				{
			        if (!(bit++ & 7))
			        	bits = font->bitmap[bitmap_offset++];
			        if (bits & 0x80)
					    pixel(x+j+glyph.xOffset, i+y+glyph.yOffset+font->yAdvance+1, colour);
			        bits <<= 1;
				}
			x += glyph.xAdvance;
		}
		return 1;
	}



	void set_font(const GFXfont* font)
	{
		this->font = font;
	}
#endif

	void set_rotation(uint16_t degrees)
	{
		assert(degrees < 360 && degrees % 90 == 0);
		rotation = degrees;
	}

private:
	void command(uint8_t cmd)
	{
//		HAL_I2C_Mem_Write(i2c, i2cadr, 0x00, 1, &cmd, 1, timeout);
		HAL_I2C_Master_Transmit(i2c, i2cadr, &cmd, 1, timeout);
	}

	void command(uint8_t cmd, uint8_t arg)
	{
		uint8_t msg[2] { cmd, arg };
//		HAL_I2C_Mem_Write(i2c, i2cadr, 0x00, 1, msg, 2, timeout);
		HAL_I2C_Master_Transmit(i2c, i2cadr, msg, 2, timeout);
	}

	void command(uint8_t cmd, uint8_t arg1, uint8_t arg2)
	{
		uint8_t msg[3] { cmd, arg1, arg2 };
//		HAL_I2C_Mem_Write(i2c, i2cadr, 0x00, 1, msg, 3, timeout);
		HAL_I2C_Master_Transmit(i2c, i2cadr, msg, 3, timeout);
	}

	bool mutex = false;
	I2C_HandleTypeDef* i2c;
	uint16_t width, height;
	uint16_t i2cadr;
	uint32_t timeout = DefaultTimeout;
	uint8_t pixels[OLED_SSD1306_WIDTH/8 * OLED_SSD1306_HEIGHT];
	uint16_t x=0, y=0;
	bool colour = true;
#if OLED_SSD1306_ENABLE_TEXT
	const GFXfont* font;
#endif
	uint16_t rotation = 0;
};



#endif /* INC_STM32_TOOLBOX_DEVICES_OLEDSSD1306_H_ */
