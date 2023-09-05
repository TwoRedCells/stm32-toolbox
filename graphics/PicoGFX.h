#ifndef _PICOGFX_H
#define _PICOGFX_H


#include <stdint.h>
#include <math.h>
#include "utility/PrintLite.h"
#include "graphics/Font6x8.h"

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define circle circle_trigonometry
#define arc arc_trigonometry

/**
 * A minimalist graphics library for plotting shapes on a 2-dimensiona canvas.
 * @param TColour	The type to use to represent colours or shades for pixel values.
 * @param TCoordinate The type to use to represent coordinates, for example uint8_t pay be suitable for a 128x64 display; however uint32_t may execute faster on a 32-bit processor.
 */
template <class TColour, class TCoordinate>
class PicoGFX : public PrintLite
{
public:
	/**
	 * Constructor for the PicoGFX library.
	 * @param x	The width of the canvas in pixels.
	 * @param y	The height of the canvas in pixels.
	 */
	PicoGFX(TCoordinate w, TCoordinate h)
	{
		width = w;
		height = h;
	}


	/**
	 * Constructor for the PicoGFX library.
	 * @param x	The width of the canvas in pixels.
	 * @param y	The height of the canvas in pixels.
	 */
	PicoGFX(TCoordinate w, TCoordinate h, TColour* buffer, uint32_t buffer_size)
	{
		width = w;
		height = h;
		this->buffer = buffer;
		this->buffer_size = buffer_size;
	}


	/**
	 * Abstract function to plot a pixel to the underlying hardware devices. This must be overridden by a
	 * hardware-specific function that performs this task. All other graphic operations will call this to plot a shape.
	 * @param x	The x-coordinate.
	 * @param y The y-coordinate.
	 * @param colour The colour to set the pixel to.
	 */
	virtual void pixel(TCoordinate x, TCoordinate y, TColour colour) = 0;


	/**
	 * Abstract function to paint part of the display with pixels from a buffer. With some displays,
	 * painting a single pixel at a time might be very time consuming, yet you might not have enough
	 * memory to buffer the whole display. This will only ever be called
	 * if the buffer_size is set, and the buffer is large enough for a the current draw operation, otherwise
	 * the operation will fallback to single-pixel painting. If you do not want to implement this simply
	 * implement an empty method and do not specify a buffer. The implementing function should use any
	 * available loop-avoidance or hardware acceleration to paint the part of the display that is specified
	 * using the data from the buffer;
	 * @param x The x-coordinate.
	 * @param y The y-coordinate.
	 * @param w The width;
	 * @param h The height.
	 */
	virtual void paint(TCoordinate x, TCoordinate y, TCoordinate w, TCoordinate h) = 0;


	/**
	 * Draws a rectangle.
	 * @param x The upper-left x-coordinate.
	 * @param y The upper-left y-coordinate.
	 * @param w The width of the rectangle.
	 * @param h The height of the rectangle.
	 * @param fill True to fill the rectangle; otherwise false (default == false).	 *
	 */
	void rectangle(TCoordinate x, TCoordinate y, TCoordinate w, TCoordinate h, bool fill=false)
	{
		if (fill)
		{
			for (TCoordinate b=y; b<y+h; b++)
				for (TCoordinate a=x; a<x+w; a++)
					if (a < width && b < height)
						pixel(a, b, colour);
		}
		else
		{
			hline(x, y, w-1);
			hline(x, y + h-1, w-1);
			vline(x, y, h-1);
			vline(x + w-1, y, h-1);
		}
	}


	/**
	 * Draws a horizontal straight line segment.
	 * @param x The upper-left x-coordinate.
	 * @param y The upper-left y-coordinate.
	 * @param w The width of the line.
	 */
	void hline(TCoordinate x, TCoordinate y, TCoordinate w)
	{
		for (uint16_t a=x; a<x+w; a++)
			pixel(a, y, colour);
	}


	/**
	 * Draws a vertical straight line segment.
	 * @param x The upper-left x-coordinate.
	 * @param y The upper-left y-coordinate.
	 * @param h The height of the line.
	 */
	virtual void vline(TCoordinate x, TCoordinate y, TCoordinate h)
	{
		for (uint16_t b=y; b<y+h; b++)
			pixel(x, b, colour);
	}


	/**
	 * Fills the screen with a the current colour.
	 */
	virtual void fill(void)
	{
		for (TCoordinate y=0; y < height; y++)
			for(TCoordinate x=0; x < width; x++)
				pixel(x, y, colour);
	}


//
//	/**
//	 * Draws an arc.
//	 * @param x The upper-left x-coordinate.
//	 * @param y The upper-left y-coordinate.
//	 * @param r The radius.
//	 * @param s The start of the arc, in degrees.
//	 * @param e The end of the arc, in degrees.
//	 * @param t The thickness of the circle. A positive value thickens outside, a negative value thickens inside.
//	 * @note This uses the Midpoint Circle Algorithm, which uses integer arithmetic instead of trigonometric functions for efficiency.
//	 */
//	void arc_midpoint(TCoordinate x, TCoordinate y, TCoordinate r, uint16_t s=0, uint16_t e=360, int32_t t=0)
//	{
//		for (int32_t u=0; u < abs(t); u++)
//		{
//			TCoordinate _x = r;
//			TCoordinate _y = 0;
//			TCoordinate err = 0;
//
//			while (_x >= _y)
//			{
//				for (float angle = 0; angle < 360; ++angle)
//				{
//					float rad = angle * M_PI / 180.0f;
//					float x1 = x + _x * cos(rad);
//					float y1 = y - _y * sin(rad);
//
//					if (angle >= s && angle <= e)
//						pixel(x1, y1, colour);
//				}
//
//				_y += 1;
//				err += 1 + 2 * _y;
//				if (2 * (err - _x) + 1 > 0)
//				{
//					_x -= 1;
//					err += 1 - 2 * _x;
//				}
//			}
//			r += t>0 ? +1 : -1;
//		}
//	}


	/**
	 * Draws the outline of a circle.
	 * @param x	The centre of the circle along the x-axis.
	 * @param y The centre of the circle along the y-axis.
	 * @param r	The radius of the circle.
	 * @param t The thickness of the circle. A positive value thickens outside, a negative value thickens inside.
	 */
	void circle_trigonometry(TCoordinate x, TCoordinate y, TCoordinate r, int32_t t=0)
	{
		float points = r * 13;
		float _x = x;
		float _y = y;
		float _r = r;
		if (t==0) t = 1;

		for (float p=0; p<points; p++)
		{
			r = _r;
			for (TCoordinate s=0; s < abs(t); s++)
			{
				float h = 2.0 * 3.14159 / points * p;
				float i = cos(h) * r;
				float j = sin(h) * r;
				pixel(i + _x, j + _y, colour);
				pixel(-i + _x, j + _y, colour);
				r += t>0 ? +1 : -1;
			}
		}
	}

	void arc_trigonometry(TCoordinate x, TCoordinate y, TCoordinate r, uint16_t s, uint16_t e, int32_t t=0)
	{
		if (t==0) t = 1;

		float _r = r;
		for (TCoordinate u=0; u < abs(t); u++)
		{
			float inc = 180.0f / (2.0f * M_PI * _r);
			for (float a = s; a < e; a += inc)
			{
				float rad = a * M_PI / 180.0f;
				pixel(x + cos(rad) * _r, y + sin(rad) * _r, colour);
			}
			_r += t>0 ? +1 : -1;
		}
	}


//
//	/**
//	 * Draws the outline of a circle.
//	 * @param x	The centre of the circle along the x-axis.
//	 * @param y The centre of the circle along the y-axis.
//	 * @param r	The radius of the circle.
//	 * @param t The thickness of the circle. A positive value thickens outside, a negative value thickens inside.
//	 */
//	void circle_midpoint_basic(TCoordinate x, TCoordinate y, TCoordinate r, int32_t t=0)
//	{
//		for (int32_t u = 0; u < abs(t); u++)
//		{
//			int32_t _x = r;
//			int32_t _y = 0;
//			int32_t err = 0;
//
//			while (_x >= _y)
//			{
//				pixel(x + _x, y + _y, colour);
//				pixel(x + _y, y + _x, colour);
//				pixel(x - _y, y + _x, colour);
//				pixel(x - _x, y + _y, colour);
//				pixel(x - _x, y - _y, colour);
//				pixel(x - _y, y - _x, colour);
//				pixel(x + _y, y - _x, colour);
//				pixel(x + _x, y - _y, colour);
//
//				_y += 1;
//				err += 1 + 2 * _y;
//				if (2 * (err - _x) + 1 > 0)
//				{
//					_x -= 1;
//					err += 1 - 2 * _x;
//				}
//			}
//			r += t>0 ? +1 : -1;
//		}
//	}
//
//	/**
//	 * Draws the outline of a circle.
//	 * @param x	The centre of the circle along the x-axis.
//	 * @param y The centre of the circle along the y-axis.
//	 * @param r	The radius of the circle.
//	 * @param t The thickness of the circle. A positive value thickens outside, a negative value thickens inside.
//	 */
//	void circle_midpoint_antialiased(TCoordinate x, TCoordinate y, TCoordinate r, int32_t t=0)
//	{
//		for (int32_t u = 0; u < abs(t); u++)
//		{
//			int32_t _x = r;
//			int32_t _y = 0;
//			int32_t err = 0;
//
//			while (_x >= _y)
//			{
//				for (int32_t dx = -1; dx < 1; dx++)
//				{
//					for (int32_t dy = -1; dy < 1; dy++)
//					{
//						int32_t x1 = _x + dx;
//						int32_t y1 = _y + dy;
//						float dist = sqrt(x1*x1 + y1*y1);
//						float intensity = 1.0f - fabs(dist - r) / 2.0f;
//						if (intensity > 0)
//						{
//							pixel(x + x1, y + y1, colour);
//							pixel(x + y1, y + x1, colour);
//							pixel(x - y1, y + x1, colour);
//							pixel(x - x1, y + y1, colour);
//							pixel(x - x1, y - y1, colour);
//							pixel(x - y1, y - x1, colour);
//							pixel(x + y1, y - x1, colour);
//							pixel(x + x1, y - y1, colour);
//						}
//					}
//				}
//
//				_y += 1;
//				err += 1 + 2 * _y;
//				if (2 * (err - _x) + 1 > 0)
//				{
//					_x -= 1;
//					err += 1 - 2 * _x;
//				}
//			}
//			r += t>0 ? +1 : -1;
//		}
//	}




	/**
	 * Writes a string at the specifiec coordinate.
	 * @param x	The centre of the circle along the x-axis.
	 * @param y The centre of the circle along the y-axis.
	 * @param str The string to write.
	 */
	void string(TCoordinate x, TCoordinate y, const char* str)
	{
		this->cx = x;
		this->cy = y;

		for (const char* s=str; *s != 0; s++)
		{
			uint8_t c = *s;
			write(c);
		}
	}

	/**
	 * Writes a string at the cursor position.
	 * @param str The string to write.
	 */
	void string(const char* str)
	{
		string(cx, cy, str);
	}


	/**
	 * Sets the current colour.
	 * @param colour The new colour.
	 */
	void set_colour(TColour colour)
	{
		this->colour = colour;
	}


	/**
	 * Sets font scaling
	 * @param scale The new scale.
	 */
	void scale_font(float scale)
	{
		this->font_scale = scale;
	}


	/**
	 * Moves the cursor to the specified position.
	 * @param x	The new x-coordinate.
	 * @param y	The new y-coordinate.
	 */
	void move(TCoordinate x, TCoordinate y)
	{
		this->cx = x;
		this->cy = y;
	}


	/**
	 * Sets the background colour.
	 * @param value The colour to set as the background.
	 */
	void set_background(TColour value)
	{
		this->background = value;
	}


	/**
	 * Paints the background where characters will be printed to avoid overprinting.
	 * @param count The number of characters to black out.
	 */
	virtual void blackout(uint8_t count)
	{
		TColour c = colour;
		colour = background;
		rectangle(cx, cy, font_scale * glyph_width * count, font_scale * glyph_height, true);
		colour = c;
	}


private:
	/**
	 * Draws a character using the current font and colour at the cursor position.
	 * @param ch The character to draw.
	 */
	size_t write(uint8_t ch) override
	{
		if (ch == '\r')
		{
			cx = 0;
		}
		else if (ch == '\n')
		{
			cy += font6x8.height * font_scale;
		}
		else
		{
			TCoordinate w = (glyph_width-2) * font_scale;
			TCoordinate h = (glyph_height) * font_scale;
			uint32_t required_buffer = w * h;
			bool fast = required_buffer <= buffer_size;
			if (fast) clear_fast(required_buffer);

			uint8_t* glyph = font6x8.glyphs[ch];
			for (TCoordinate j=0; j<glyph_height; j++)
				for (TCoordinate i=0; i<glyph_width-1; i++)
					if (glyph[j] & (1 << i+2))  // The right two pixels are empty (6 bits wide).
						for (uint8_t s=0; s < font_scale; s++)
							for (uint8_t t=0; t < font_scale; t++)
							{
								if (fast) pixel_fast(i*font_scale+s, j*font_scale+t, w, h, colour);
								else pixel(cx + i*font_scale+s, cy + j*font_scale+t, colour);
							}
			if (fast) paint(cx, cy, w, h);
//			cx += font6x8.width * font_scale;
			cx += 6 * font_scale;
		}
		return 1;
	}


	/**
	 * Write a value to the pixel buffer.
	 * @param rx The x-coordinate relative to the window being painted.
	 * @param ry The y-coordinate relative to the window being painted.
	 * @param w The width of the window.
	 * @param h The height of the window.
	 */
	void pixel_fast(TCoordinate rx, TCoordinate ry, TCoordinate w, TCoordinate h, TColour hue)
	{
		buffer[ry*w + rx] = hue;
	}


	/**
	 * Sets the specified number of pixels in the pixel buffer to the background colour.
	 * @param pixels The number of pixels.
	 */
	void clear_fast(uint32_t pixels)
	{
		for (uint32_t i=0; i < pixels; i++)
			buffer[i] = background;
	}


protected:
	TCoordinate width;       /// Display width as modified by current rotation.
	TCoordinate height;      /// Display height as modified by current rotation.
	TCoordinate cx;     /// x location to start print()ing text.
	TCoordinate cy;     /// y location to start print()ing text.
	TColour colour;		/// The current colour.
	Font6x8 font6x8;
	uint8_t font_scale = 1;
	TColour background;
	TColour* buffer;
	uint32_t buffer_size;
	static constexpr uint8_t glyph_width = 8;
	static constexpr uint8_t glyph_height = 8;
};

#endif
