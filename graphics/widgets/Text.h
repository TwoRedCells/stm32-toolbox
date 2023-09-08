///	@file       graphics/widgets/Text.h
///	@class      Text
///	@brief      One or more characters
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef LIB_STM32_TOOLBOX_GRAPHICS_TEXT_H_
#define LIB_STM32_TOOLBOX_GRAPHICS_TEXT_H_

#include <stdint.h>
#include <stdarg.h>
#include "IWidget.h"
#include "utility/PrintLite.h"


template <class TColour>
class Text : public IWidget<TColour>
{
public:
	enum Alignment { Left, Centre, Right };
	static constexpr uint32_t default_buffer_length = 100;
	static const Font6x8 font6x8;

	/**
	 * Constructs a line.
	 * @param x The x-coordinate of the first point
	 * @param 1 The y-coordinate of the first point.
	 * @param w The width of the space allocated to the text.
	 * @param c The colour.
	 * @param str The text string.
	 */
	Text(uint32_t x, uint32_t y, uint32_t w, Alignment a, TColour c, uint8_t scale, char* format, ...)
	{
		this->x = x;
		this->y = y;
		this-w = w;
		this->colour = c;
		this->alignment = a;
		this->str = str;
		this->scale = scale;

		va_list p;
		va_start(p, format);
		PrintLite::vsprintf(buffer, format, p);
		va_end(p);
	}


	/**
	 * Constructs a line.
	 * @param x The x-coordinate of the first point
	 * @param 1 The y-coordinate of the first point.
	 * @param c The colour.
	 * @param str The text string.
	 */
	Text(uint32_t x, uint32_t y, TColour c, uint8_t scale, char* format, ...)
	{
		this->x = x;
		this->y = y;
		this-w = 0;
		this->colour = c;
		this->alignment = Left;
		this->str = str;
		this->scale = scale;

		va_list p;
		va_start(p, format);
		uint32_t ret = PrintLite::vsprintf(buffer, format, p);
		va_end(p);
	}


	/**
	 * Renders this widget on a canvas.
	 * @param canvas The canvas to paint on.
	 */
	void render(IPaintable<TColour>* surface) override
	{
		if (this->enabled)
			render(surface, x, y, w, alignment, colour, scale, buffer);
	}


	/**
	 * Draws a string of characters.
	 * @notes This function does not allow newline characters.
	 * @param surface Pointer to the drawing surface.
	 * @param x The x-coordinate of the first point
	 * @param y The y-coordinate of the first point.
	 * @param w Width of the area into which the text will be aligned.
	 * @param a The text alignment.
	 * @param colour The text colour.
	 * @param scale The font scale.
	 * @param format The format string.
	 * @param args The arguments.
	 */
	static void render(IPaintable<TColour>* surface, uint32_t x, uint32_t y, uint32_t w, Alignment a, TColour colour, uint8_t scale, const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		render(surface, x, y, w, a, colour, scale, format, args);
		va_end(args);
	}


	/**
	 * Draws a string of characters.
	 * @notes This function does not allow newline characters.
	 * @param surface Pointer to the drawing surface.
	 * @param x The x-coordinate of the first point
	 * @param y The y-coordinate of the first point.
	 * @param w Width of the area into which the text will be aligned.
	 * @param a The text alignment.
	 * @param colour The text colour.
	 * @param scale The font scale.
	 * @param format The format string.
	 * @param args The arguments.
	 */
	static void render(IPaintable<TColour>* surface, uint32_t x, uint32_t y, uint32_t w, Alignment a, TColour colour, uint8_t scale, const char* format, va_list args)
	{
		uint32_t cx = x;
		uint32_t cy = y;
		const uint32_t cw = font6x8.width * scale;
		const uint32_t ch = font6x8.height * scale;

		char buffer[default_buffer_length];
		uint32_t len = PrintLite::vsprintf(buffer, format, args);

		if (w == 0)
			w = cw * len;

		if (a == Right)
		{
			uint32_t sw = text_width(buffer, scale);
			cx = x + w - sw;
		}
		else if (a == Centre)
		{
			uint32_t sw = text_width(buffer, scale);
			cx = x + (w - sw) / 2;
		}

		for (const char* s = buffer; *s != 0; s++)
		{
			uint8_t c = *s;
			if (c == '\r')
			{
				cx = x;
			}
			else if (c == '\n')
			{
				cy += font6x8.height * scale;
			}
			else
			{
				const uint8_t* glyph = font6x8.glyphs[c];
				for (uint32_t j=0; j<font6x8.height; j++)
					for (uint32_t i=0; i<font6x8.width+2-1; i++)
						if (glyph[j] & (1 << (i+2)))  // The right two pixels are empty (6 bits wide).
							for (uint8_t s=0; s < scale; s++)
								for (uint8_t t=0; t < scale; t++)
									surface->pixel(cx + i*scale+s, cy + j*scale+t, colour);
				cx += font6x8.width * scale;
			}
		}
	}


	/**
	 * Draws a string of characters.
	 * @notes This function does not allow newline characters.
	 * @param surface Pointer to the drawing surface.
	 * @param x The x-coordinate of the first point
	 * @param y The y-coordinate of the first point.
	 * @param w Width of the area into which the text will be aligned.
	 * @param a The text alignment.
	 * @param foreground The foreground colour.
	 * @param background The background colour.
	 * @param scale The font scale.
	 * @param format The format string.
	 */
	static void render_fast(IPaintable<TColour>* surface, uint32_t x, uint32_t y, uint32_t w, Alignment a, TColour foreground, TColour background, uint8_t scale, const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		char buffer[default_buffer_length];
		uint32_t len = PrintLite::vsprintf(buffer, format, args);

		if (w == 0)
			w = len * font6x8.width * scale;
		surface->start_region(x, y, w, font6x8.height*scale);
		surface->fill_region(background);
		render(surface, x, y, w, a, foreground, scale, format, args);
		va_end(args);
		surface->end_region();
	}


	/**
	 * Draws a string of characters.
	 * @notes This function does not allow newline characters.
	 * @param surface Pointer to the drawing surface.
	 * @param x The x-coordinate of the first point
	 * @param y The y-coordinate of the first point.
	 * @param w Width of the area into which the text will be aligned.
	 * @param a The text alignment.
	 * @param foreground The foreground colour.
	 * @param background The background colour.
	 * @param scale The font scale.
	 * @param format The format string.
	 */
	static void render_fast(IPaintable<TColour>* surface, uint32_t x, uint32_t y, TColour foreground, TColour background, uint8_t scale, const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		char buffer[default_buffer_length];
		uint32_t len = PrintLite::vsprintf(buffer, format, args);

		uint32_t w = len * font6x8.width * scale;
		surface->start_region(x, y, w, font6x8.height*scale);
		surface->fill_region(background);
		render(surface, x, y, w, Left, foreground, scale, format, args);
		va_end(args);
		surface->end_region();
	}


	/**
	 * Paints the background where characters will be printed to avoid overprinting.
	 * @param count The number of characters to black out.
	 */
	virtual void blackout(uint8_t count)
	{
//		TColour c = colour;
//		colour = background;
//		rectangle(cx, cy, font_scale * glyph_width * count, font_scale * glyph_height, true);
//		colour = c;
	}


	/**
	 * Sets the text value.
	 * @param str the text value.
	 * @returns The length of the string.
	 */
	uint32_t set_value(char* format, ...)
	{
		va_list p;
		va_start(p, format);
		uint32_t ret = PrintLite::vsprintf(buffer, format, p);
		va_end(p);
		return(ret);
	}


	/**
	 * Replaces the internal buffer.
	 * @param buf The new buffer.
	 * @param length The length of the new buffer.
	 */
	void set_buffer(char buf[], uint32_t length)
	{
		this->buffer = buf;
		this->buffer_length = length;
	}


	/**
	 * Gets the length of the internal buffer.
	 * @returns The buffer length.
	 */
	uint32_t get_buffer_length(void)
	{
		return buffer_length;
	}



protected:
	static uint32_t text_width(char* str, uint8_t scale)
	{
		return scale * strlen(str) * font6x8.width;
	}


	uint32_t x;
	uint32_t y;
	uint32_t w;
	TColour colour;
	Alignment alignment;
	char* str = "";
	uint8_t scale = 1;
	char buffer[default_buffer_length] = "";
	uint32_t buffer_length;
};

#endif /* LIB_STM32_TOOLBOX_GRAPHICS_TEXT_H_ */
