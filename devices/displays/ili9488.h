/*
 *  ili9488.h
 *  @description A no-crap, no-frills STM32 driver for ILI9488 chipset TFT LCD displays.
 *
 *	@copyright Copyright 2022 Yvan Rodrigues <yvan.r at radskunkworks dot com>
 *	               Robotic Assistance Devices, AITX
 *
 *	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 *	following conditions are met:
 *
 *	Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 *	disclaimer.
 *
 *	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
 *	disclaimer in the documentation and/or other materials provided with the distribution.
 *
 *	Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
 *	products derived from this software without specific prior written permission.
 *
 *	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 *	USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef INC_ILI9488_H_
#define INC_ILI9488_H_

#include "toolbox.h"

#ifdef FREERTOS_CONFIG_H
	#define delay(x) osDelay(x)
#else
	#define delay(x) HAL_Delay(x)
#endif

#if ENABLE_ILI9488_DMA
	#define WriteSPI(port, data, len)  HAL_SPI_Transmit_DMA(port, data, len)
#else
	#define WriteSPI(port, data, len)  HAL_SPI_Transmit(port, data, len, 1)
#endif

#define fast_hline(x, y, w, hue) fast_fill(x, y, w, 1, hue)
#define fast_vline(x, y, h, hue) fast_fill(x, y, 1, h, hue)

#include <stdint.h>

struct RGB
{
	RGB()
	{
	}

	RGB(uint16_t hue)
	{
		r = (((hue & 0xff00) >> 11) * 0xff) / 31;
		g = (((hue & 0x07e0) >> 5) * 0xff) / 63;
		b = ((hue & 0x001f) * 0xff) / 31;
	}


//	RGB(uint32_t hue)
//	{
//		r = (hue >> 16) & 0xff;
//		g = (hue >> 8) & 0xff;
//		b = hue & 0xff;
//	}


	RGB(uint8_t r, uint8_t g, uint8_t b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}

//	uint32_t to_msb(void)
//	{
//		//return b << 16 | g << 8 | r ;
//		return r >> 2 << 11 | g << 3 << 5 | b >> 2;
//	}
//
//	static RGB from16(uint16_t hue)
//	{
//		return RGB((((hue & 0xff00) >> 11) * 0xff) / 31,
//				(((hue & 0x07e0) >> 5) * 0xff) / 63,
//				((hue & 0x001f) * 0xff) / 31);
//	}

	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;

	RGB operator =(uint16_t hue)
	{
		return RGB(hue);
	}

//	RGB operator =(RGB hue)
//	{
//		return RGB(hue.r, hue.g, hue.b);
//	}

	// See http://embeddednotepad.com/page/rgb565-color-picker
	static constexpr uint16_t Black =       0x0000;
	static constexpr uint16_t Navy =        0x000f;
	static constexpr uint16_t DarkGreen =   0x03e0;
	static constexpr uint16_t DarkCyan =    0x03ef;
	static constexpr uint16_t Maroon =      0x7800;
	static constexpr uint16_t Purple =      0x780f;
	static constexpr uint16_t Olive =       0x7be0;
	static constexpr uint16_t LightGrey =   0xc618;
	static constexpr uint16_t MediumGrey =  0x7bef;
	static constexpr uint16_t DarkGrey =    0x4208;
	static constexpr uint16_t Blue =        0x003f;
	static constexpr uint16_t Green =       0x07E0;
	static constexpr uint16_t Cyan =        0x07ff;
	static constexpr uint16_t Red =         0xf800;
	static constexpr uint16_t Magenta =     0xf81f;
	static constexpr uint16_t Yellow  =     0xffe0;
	static constexpr uint16_t White =       0xffff;
	static constexpr uint16_t Orange =      0xfd20;
	static constexpr uint16_t GreenYellow = 0xafe5;
	static constexpr uint16_t Pink =        0xf81f;
};


class ILI9488
{
public:
	static constexpr uint8_t VERTICAL = 0;
	static constexpr uint8_t HORIZONTAL = 1;
	static constexpr uint8_t VERTICAL_FLIPPED = 2;
	static constexpr uint8_t HORIZONTAL_FLIPPED = 3;

	static constexpr uint8_t NOP = 0x00;
	static constexpr uint8_t SOFTWARE_RESET = 0x01;
	static constexpr uint8_t READ_IDENTIFICATION_INFORMATON = 0x04;
	static constexpr uint8_t READ_STATUS = 0x09;

	static constexpr uint8_t SLEEP_IN = 0x10;
	static constexpr uint8_t SLEEP_OUT = 0x11;
	static constexpr uint8_t PARTIAL_MODE_ON = 0x12;
	static constexpr uint8_t NORMAL_MODE_ON = 0x13;

	static constexpr uint8_t READ_POWER_MODE = 0x0a;
	static constexpr uint8_t READ_MEMORY_ACCESS = 0x0b;
	static constexpr uint8_t READ_PIXEL_FORMAT = 0x0c;
	static constexpr uint8_t READ_IMAGE_MODE = 0x0d;
	static constexpr uint8_t READ_DISPLAY_SELF_DIAGNOSTIC_RESULT = 0x0f;

	static constexpr uint8_t INVERT_OFF = 0x20;
	static constexpr uint8_t INVERT_ON = 0x21;
	static constexpr uint8_t ALL_PIXELS_OFF = 0x22;
	static constexpr uint8_t ALL_PIXELS_ON = 0x23;
	static constexpr uint8_t GAMMA_SET = 0x26;
	static constexpr uint8_t DISPLAY_OFF = 0x28;
	static constexpr uint8_t DISPLAY_ON = 0x29;

	static constexpr uint8_t COLUMN_ADDRESS_SET = 0x2a;
	static constexpr uint8_t PAGE_ADDRESS_SET = 0x2b;
	static constexpr uint8_t MEMORY_WRITE = 0x2c;
	static constexpr uint8_t MEMORY_READ = 0x2d;

	static constexpr uint8_t PARTIAL_AREA = 0x30;
	static constexpr uint8_t MEMORY_ACCESS = 0x36;
	static constexpr uint8_t PIXEL_FORMAT = 0x3a;

	static constexpr uint8_t INTERFACE_MODE = 0xb0;
	static constexpr uint8_t FRAME_RATE_1 = 0xb1;
	static constexpr uint8_t FRAME_RATE_2 = 0xb2;
	static constexpr uint8_t FRAME_RATE_3 = 0xb3;
	static constexpr uint8_t INVERSION = 0xb4;
	static constexpr uint8_t DISPLAY_FUNCTION = 0xb6;

	static constexpr uint8_t POWER_1 = 0xc0;
	static constexpr uint8_t POWER_2 = 0xc1;
	static constexpr uint8_t POWER_3 = 0xc2;
	static constexpr uint8_t POWER_4 = 0xc3;
	static constexpr uint8_t POWER_5 = 0xc4;
	static constexpr uint8_t VCOM = 0xc5;
	//const uint8_t VMCTR2 = 0xc7;

	//const uint8_t RDID1 = 0xda;
	//const uint8_t RDID2 = 0xdb;
	//const uint8_t RDID3 = 0xdc;
	//const uint8_t RDID4 = 0xdd;

	static constexpr uint8_t POSITIVE_GAMMA = 0xe0;
	static constexpr uint8_t NEGATIVE_GAMMA = 0xe1;
	static constexpr uint8_t SET_IMAGE = 0xe9;
	static constexpr uint8_t ADJUST_CONTROL_3 = 0xf7;

	static constexpr uint8_t MEMORY_ACCESS_MY = 0x80;
	static constexpr uint8_t MEMORY_ACCESS_MX = 0x40;
	static constexpr uint8_t MEMORY_ACCESS_MV = 0x20;
	static constexpr uint8_t MEMORY_ACCESS_ML = 0x10;
	static constexpr uint8_t MEMORY_ACCESS_RGB = 0x00;
	static constexpr uint8_t MEMORY_ACCESS_BGR = 0x08;
	static constexpr uint8_t MEMORY_ACCESS_MH = 0x04;

	static constexpr uint8_t BRIGHTNESS = 0x51;
	static constexpr uint8_t WRITE_CONTROL = 0x53;
	static constexpr uint8_t READ_CONTROL = 0x54;
	static constexpr uint8_t WRITE_CABC = 0x55;
	static constexpr uint8_t READ_CABC = 0x56;
	static constexpr uint8_t WRITE_CABC_MINIMUM_BRIGHTNESS = 0x5e;
	static constexpr uint8_t READ_CABC_MINIMUM_BRIGHTNESS = 0x5f;


	ILI9488(uint16_t width, uint16_t height,
			GPIO_TypeDef* cs_port, uint16_t cs_pin,
			GPIO_TypeDef* rst_port, uint16_t rst_pin,
			GPIO_TypeDef* dc_port, uint16_t dc_pin,
			SPI_HandleTypeDef *spi)
	{
		this->_width = width;
		this->width = width;
		this->_height = height;
		this->height = height;
		this->cs_port = cs_port;
		this->cs_pin = cs_pin;
		this->rst_port = rst_port;
		this->rst_pin = rst_pin;
		this->dc_port = dc_port;
		this->dc_pin = dc_pin;
		this->spi = spi;
	}

	void begin(void)
	{
		rst(false);
		delay(10);
		rst(true);

		command(POSITIVE_GAMMA);
		data(0x00);
		data(0x03);
		data(0x09);
		data(0x08);
		data(0x16);
		data(0x0a);
		data(0x3f);
		data(0x78);
		data(0x4c);
		data(0x09);
		data(0x0a);
		data(0x08);
		data(0x16);
		data(0x1a);
		data(0x0f);

		command(NEGATIVE_GAMMA);
		data(0x00);
		data(0x16);
		data(0x19);
		data(0x03);
		data(0x0f);
		data(0x05);
		data(0x32);
		data(0x45);
		data(0x46);
		data(0x04);
		data(0x0e);
		data(0x0d);
		data(0x35);
		data(0x37);
		data(0x0f);

		command(POWER_1);
		data(0x17);
		data(0x15);

		command(POWER_2);
		data(0x41);

		command(VCOM);
		data(0x00);
		data(0x12);
		data(0x80);

		command(MEMORY_ACCESS);
		data(0x48);

		command(PIXEL_FORMAT);
		data(0x66);

		command(INTERFACE_MODE);
		data(0x80);

		command(FRAME_RATE_1);
		data(0xa0);

		command(INVERSION);
		data(0x02);

		command(DISPLAY_FUNCTION);
		data(0x02);
		data(0x02);

		command(SET_IMAGE);
		data(0x00);

		command(ADJUST_CONTROL_3);
		data(0xa9);
		data(0x51);
		data(0x2c);
		data(0x82);

		wake();
		delay(120);
		on();  // Display on.
	}


	void off(void)
	{
		command(DISPLAY_OFF);
	}


	void on(void)
	{
		command(DISPLAY_ON);
	}


	void sleep(void)
	{
		command(SLEEP_IN);
	}


	void wake(void)
	{
		command(SLEEP_OUT);
	}

	void control(bool brightness, bool dimming, bool backlight)
	{
		command(WRITE_CONTROL);
		uint8_t value = 0;
		if (brightness) value |= 0x20;
		if (dimming) value |= 0x08;
		if (backlight) value |= 0x04;
		data(value);
	}

	void brightness(uint8_t value)
	{
		command(WRITE_CABC);
		data(value);
	}


	void pixel(int16_t x, int16_t y, RGB hue)
	{
		//assert(x < width && y < height);
		window(x, y, x + 1, y + 1);
		dc(true);
		cs(false);
		pixel(hue);
		cs(true);
	}


	void pixel(RGB hue)
	{
		pixel(&hue);
	}


	void fast_fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, RGB hue)
	{
		RGB buffer[w];
		window(x, y, x + w - 1, y + h - 1);
		dc(true);
		cs(false);
		for (uint16_t i = 0; i < w; i++)
			buffer[i] = hue;
		for (uint16_t j = 0; j < h; j++)
			write(buffer, w);
		cs(true);
	}


	void write(RGB* buffer, uint32_t pixels)
	{
		WriteSPI(spi, (uint8_t*)buffer, pixels * sizeof(RGB));
	}


	void rotate(uint8_t rotation)
	{
		//assert(rotation < 4);
		command(MEMORY_ACCESS);

		switch (rotation % 4)
		{
		case 0:
			data(MEMORY_ACCESS_MX | MEMORY_ACCESS_BGR);
			width = _width;
			height = _height;
			break;
		case 1:
			data(MEMORY_ACCESS_MV | MEMORY_ACCESS_BGR);
			width = _height;
			height = _width;
			break;
		case 2:
			data(MEMORY_ACCESS_MY | MEMORY_ACCESS_BGR);
			width = _width;
			height = _height;
			break;
		case 3:
			data(MEMORY_ACCESS_MX | MEMORY_ACCESS_MY | MEMORY_ACCESS_MV | MEMORY_ACCESS_BGR);
			width = _height;
			height = _width;
			break;
		}
	}


protected:
	void pixel(uint8_t r, uint8_t g, uint8_t b)
	{
		uint8_t rgb[3];
		rgb[0] = r;
		rgb[1] = g;
		rgb[2] = b;
		WriteSPI(spi, rgb, 3);
	}

	void pixel(RGB* hue)
	{
		WriteSPI(spi, (uint8_t*) hue, 3);
	}

	void cs(bool state)
	{
		HAL_GPIO_WritePin(cs_port, cs_pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
	}

	void rst(bool state)
	{
		HAL_GPIO_WritePin(rst_port, rst_pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
	}

	void dc(bool state)
	{
		HAL_GPIO_WritePin(dc_port, dc_pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
	}

	void command(uint8_t cmd)
	{
		dc(false);
		cs(false);
		WriteSPI(spi, &cmd, 1);
		cs(true);
	}

	void data(uint8_t data)
	{
		dc(true);
		cs(false);
		WriteSPI(spi, &data, 1);
		cs(true);
	}

	void window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
	{
		command(COLUMN_ADDRESS_SET);
		data(x0 >> 8);
		data(x0 & 0xff);
		data(x1 >> 8);
		data(x1 & 0xff);
		command(PAGE_ADDRESS_SET);
		data(y0 >> 8);
		data(y0 & 0xff);
		data(y1 >> 8);
		data(y1 & 0xff);
		command(MEMORY_WRITE);
	}

	SPI_HandleTypeDef *spi;
private:
	GPIO_TypeDef *cs_port, *rst_port, *dc_port;
	uint16_t cs_pin, rst_pin, dc_pin;
	uint16_t width, height, _width, _height;
};

#endif /* INC_ILI9488_H_ */
