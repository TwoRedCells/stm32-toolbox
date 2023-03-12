/*
 * Graphics.h
 *
 *  Created on: Jul 11, 2022
 *      Author: YvanRodriguez
 */

#ifndef INC_GRAPHICS_H_
#define INC_GRAPHICS_H_

#include "PicoGFX.h"
#include "ili9488.h"

class Graphics : public PicoGFX, public ILI9488
{
public:
	Graphics(uint16_t width, uint16_t height,
			GPIO_TypeDef* cs_port, uint16_t cs_pin,
			GPIO_TypeDef* rst_port, uint16_t rst_pin,
			GPIO_TypeDef* dc_port, uint16_t dc_pin,
			SPI_HandleTypeDef *spi) : PicoGFX(width, height), ILI9488(width, height, cs_port, cs_pin, rst_port, rst_pin, dc_port, dc_pin, spi)
	{

	}

	void pixel(uint16_t x, uint16_t y, uint16_t hue) override
	{
		ILI9488::pixel(x, y, hue);
	}
};



#endif /* INC_GRAPHICS_H_ */
