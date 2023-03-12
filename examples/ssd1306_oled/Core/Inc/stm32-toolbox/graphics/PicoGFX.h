#ifndef _PICOGFX_H
#define _PICOGFX_H

#include "gfxfont.h"
#include "fonts/5x7font.h"
#include <stdint.h>
#include <math.h>

// A minimalist version of the Adafruit_GFX library that does not have any dependencies on AVR, Arduino etc.



#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)  { int16_t t = a;  a = b;  b = t; }
#endif


class PicoGFX
{
public:
	PicoGFX(int16_t w, int16_t h)
	{
		width = w;
		height = h;
		rotation = 0;
		cursor_y = cursor_x = 0;
		textsize_x = textsize_y = 1;
		text_color = textbgcolor = 0xFFFF;
		wrap = true;
		gfxFont = NULL;
	}



	/**
	 * @brief	Lowest-level draw operation, sets the colour of a single pixel.
	 * @param	x	X-coordinate.
	 * @param	y	Y-coordinate]
	 * @param	colour The colour.
	 */
	virtual void pixel(uint16_t x, uint16_t y, uint16_t colour) = 0;


	/**
	 * @brief	Draws a line segment between two points.
	 * @param	x0	X-coordinate of the first point.
	 * @param	y0	Y-coordinate of the first point.
	 * @param	x1	X-coordinate of the second point.
	 * @param	y1	Y-coordinate of the second point.
	 * @param	colour The colour of the line.
	 */
	virtual void line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t colour)
	{
		if (x0 == x1)
			vline(x0, y0, y1 - y0 + 1, color);
		else if (y0 == y1)
			hline(x0, y0, x1 - x0 + 1, color);
		else
		{
			int16_t steep = abs(y1 - y0) > abs(x1 - x0);
			if (steep)
			{
				_swap_int16_t(x0, y0);
				_swap_int16_t(x1, y1);
			}

			if (x0 > x1)
			{
				_swap_int16_t(x0, x1);
				_swap_int16_t(y0, y1);
			}

			int16_t dx, dy;
			dx = x1 - x0;
			dy = abs(y1 - y0);

			int16_t err = dx / 2;
			int16_t ystep;

			if (y0 < y1)
				ystep = 1;
			else
				ystep = -1;

			for (; x0 <= x1; x0++)
			{
				if (steep)
					pixel(y0, x0, color);
				else
					pixel(x0, y0, color);

				err -= dy;
				if (err < 0)
				{
					y0 += ystep;
					err += dx;
				}
			}
		}
	}


	/**
	 * @brief	Draws a horizontal line segment between two points.
	 * @param	x	X-coordinate of the first point.
	 * @param	y	Y-coordinate of the first point.
	 * @param	h	The height of the line segment.
	 * @param	colour The colour of the line.
	 */
	virtual void vline(uint16_t x, uint16_t y, uint16_t h, uint16_t colour)
	{
		for (int i=y; i < y+h; i++)
			pixel(x, i, color);
	}


	/**
	 * @brief	Draws a vertical line segment between two points.
	 * @param	x	X-coordinate of the first point.
	 * @param	y	Y-coordinate of the first point.
	 * @param	w	The width of the line segment.
	 * @param	colour The colour of the line.
	 */
	virtual void hline(uint16_t x, uint16_t y, uint16_t w, uint16_t colour)
	{
		for (int i=x; i < x+w; i++)
			pixel(i, y, color);
	}


	/**
	 * @brief	Draws the outline of a rectangle.
	 * @param 	x	X-coordinate of the first corner.
	 * @param	y	Y-coordinate of the first corner.
	 * @param	w	Width of the rectangle.
	 * @praam 	h	Height of the rectangle.
	 * @param	colour The colour of the outline.
	 */
	virtual void rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t colour)
	{
		hline(x, y, w, color);
		hline(x, y + h - 1, w, colour);
		vline(x, y, h, color);
		vline(x + w - 1, y, h, colour);
	}


	/**
	 * @brief	Draws a filled rectangle
	 * @param 	x	X-coordinate of the first corner.
	 * @param	y	Y-coordinate of the first corner.
	 * @param	w	Width of the rectangle.
	 * @praam 	h	Height of the rectangle.
	 * @param	outline The colour of the outline.
	 * @param	fill The colour of the fill..
	 */
	virtual void rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill)
	{
		for (int16_t i = x; i < x + w; i++)
			vline(i, y, h, fill);
		rectangle(x, y, w, h, outline);
	}


	/**
	 * @brief	Draws an arc.
	 * @param	x The X-coordinate of the centre of the arc.
	 * @param	y The Y-coordinate of the centre of the arc.
	 * @param	r The radius of the arc.
	 * @param	start The start angle of the arc.
	 * @param	end The end angle of the arc.
	 * @param	colour The colour of the arc.
	 */
	virtual void arc(uint16_t x, uint16_t y, uint16_t r, uint16_t start, uint16_t end, uint16_t colour)
	{

	}


	/**
	 * @brief	Draws a circle
	 * @param	x The X-coordinate of the centre of the circle.
	 * @param	y The Y-coordinate of the centre of the circle.
	 * @param	r The radius of the circle.
	 * @param	colour The colour of the arc.
	 */
	virtual void circle(uint16_t x, uint16_t y, uint16_t r, uint16_t start, uint16_t end, uint16_t outline, uint16_t fill)
	{
		for (int16_t _r = -r; _r < r; +r++)
		{

		}
	}


	// These exist only with Adafruit_GFX (no subclass overrides)
	void circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
	{
		int16_t f = 1 - r;
		int16_t ddF_x = 1;
		int16_t ddF_y = -2 * r;
		int16_t x = 0;
		int16_t y = r;

		pixel(x0, y0 + r, color);
		pixel(x0, y0 - r, color);
		pixel(x0 + r, y0, color);
		pixel(x0 - r, y0, color);

		while (x < y)
		{
			if (f >= 0)
			{
				y--;
				ddF_y += 2;
				f += ddF_y;
			}
			x++;
			ddF_x += 2;
			f += ddF_x;

			pixel(x0 + x, y0 + y, color);
			pixel(x0 - x, y0 + y, color);
			pixel(x0 + x, y0 - y, color);
			pixel(x0 - x, y0 - y, color);
			pixel(x0 + y, y0 + x, color);
			pixel(x0 - y, y0 + x, color);
			pixel(x0 + y, y0 - x, color);
			pixel(x0 - y, y0 - x, color);
		}
	}


	void circlehelper(uint16_t x0, uint16_t y0, uint16_t r, uint8_t cornername, uint16_t color)
	{
		int16_t f = 1 - r;
		int16_t ddF_x = 1;
		int16_t ddF_y = -2 * r;
		int16_t x = 0;
		int16_t y = r;

		while (x < y) {
			if (f >= 0) {
				y--;
				ddF_y += 2;
				f += ddF_y;
			}
			x++;
			ddF_x += 2;
			f += ddF_x;
			if (cornername & 0x4)
			{
				pixel(x0 + x, y0 + y, color);
				pixel(x0 + y, y0 + x, color);
			}
			if (cornername & 0x2)
			{
				pixel(x0 + x, y0 - y, color);
				pixel(x0 + y, y0 - x, color);
			}
			if (cornername & 0x8)
			{
				pixel(x0 - y, y0 + x, color);
				pixel(x0 - x, y0 + y, color);
			}
			if (cornername & 0x1)
			{
				pixel(x0 - y, y0 - x, color);
				pixel(x0 - x, y0 - y, color);
			}
		}
	}


	void circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color, uint16_t fill)
	{
		circle(x0, y0, r, color);
		vline(x0, y0 - r, 2 * r + 1, fill);
		circlehelper(x0, y0, r, 3, 0, color, fill);
	}

	void circlehelper(uint16_t x0, uint16_t y0, uint16_t r, uint8_t cornername, int16_t delta, uint16_t color, uint16_t fill)
	{
		circlehelper(x0, y0, r, cornername, color);
		int16_t f = 1 - r;
		int16_t ddF_x = 1;
		int16_t ddF_y = -2 * r;
		int16_t x = 0;
		int16_t y = r;
		int16_t px = x;
		int16_t py = y;

		delta++; // Avoid some +1's in the loop

		while (x < y)
		{
			if (f >= 0)
			{
				y--;
				ddF_y += 2;
				f += ddF_y;
			}
			x++;
			ddF_x += 2;
			f += ddF_x;

			px = x;
		}
	}


	void roundrectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t color)
	{
		int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
		if (r > max_radius)
			r = max_radius;
		// smarter version
		hline(x + r, y, w - 2 * r, color);         // Top
		hline(x + r, y + h - 1, w - 2 * r, color); // Bottom
		vline(x, y + r, h - 2 * r, color);         // Left
		vline(x + w - 1, y + r, h - 2 * r, color); // Right
		// draw four corners
		circlehelper(x + r, y + r, r, 1, color);
		circlehelper(x + w - r - 1, y + r, r, 2, color);
		circlehelper(x + w - r - 1, y + h - r - 1, r, 4, color);
		circlehelper(x + r, y + h - r - 1, r, 8, color);
	}

	void roundrectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h,  uint16_t r, uint16_t color, uint16_t fill)
	{
		roundrectangle(x, y, w, h, r, color, fill);
		int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
		if (r > max_radius)
			r = max_radius;
		// smarter version
		rectangle(x + r, y, w - 2 * r, h, fill);
		// draw four corners
		circlehelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color, fill);
		circlehelper(x + r, y + r, r, 2, h - 2 * r - 1, color, fill);
	}

	void character(uint16_t x, uint16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
	{
		character(x, y, c, color, bg, size, size);
	}

	void character(uint16_t x, uint16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y)
	{
		if (!gfxFont)
		{ // 'Classic' built-in font
			if ((x >= width) ||              // Clip right
					(y >= height) ||             // Clip bottom
					((x + 6 * size_x - 1) < 0) || // Clip left
					((y + 8 * size_y - 1) < 0))   // Clip top
				return;

			for (int8_t i = 0; i < 5; i++)
			{ // Char bitmap = 5 columns
				uint8_t line = font5x7[c * 5 + i];
				for (int8_t j = 0; j < 8; j++, line >>= 1)
				{
					if (line & 1)
					{
						if (size_x == 1 && size_y == 1)
							pixel(x + i, y + j, color);
						else
							rectangle(x + i * size_x, y + j * size_y, size_x, size_y, color, color);
					}
					else if (bg != color)
					{
						if (size_x == 1 && size_y == 1)
							pixel(x + i, y + j, bg);
						else
							rectangle(x + i * size_x, y + j * size_y, size_x, size_y, bg, bg);
					}
				}
			}
			if (bg != color)
			{ // If opaque, draw vertical line for last column
				if (size_x == 1 && size_y == 1)
					vline(x + 5, y, 8, bg);
				else
					rectangle(x + 5 * size_x, y, size_x, 8 * size_y, bg, bg);
			}
		}
		else
		{ // Custom font

			// Character is assumed previously filtered by write() to eliminate
			// newlines, returns, non-printable characters, etc.  Calling
			// drawChar() directly with 'bad' characters of font may cause mayhem!

			c -= gfxFont->first;
			GFXglyph *glyph = gfxFont->glyph + c;
			uint8_t *bitmap = gfxFont->bitmap;

			uint16_t bo = glyph->bitmapOffset;
			uint8_t w = glyph->width, h = glyph->height;
			int8_t xo = glyph->xOffset,	yo =glyph->yOffset;
			uint8_t xx, yy, bits = 0, bit = 0;
			int16_t xo16 = 0, yo16 = 0;

			if (size_x > 1 || size_y > 1)
			{
				xo16 = xo;
				yo16 = yo;
			}

			// Todo: Add character clipping here

			// NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
			// THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
			// has typically been used with the 'classic' font to overwrite old
			// screen contents with new data.  This ONLY works because the
			// characters are a uniform size; it's not a sensible thing to do with
			// proportionally-spaced fonts with glyphs of varying sizes (and that
			// may overlap).  To replace previously-drawn text when using a custom
			// font, use the getTextBounds() function to determine the smallest
			// rectangle encompassing a string, erase the area with fillRect(),
			// then draw new text.  This WILL infortunately 'blink' the text, but
			// is unavoidable.  Drawing 'background' pixels will NOT fix this,
			// only creates a new set of problems.  Have an idea to work around
			// this (a canvas object type for MCUs that can afford the RAM and
			// displays supporting setAddrWindow() and pushColors()), but haven't
			// implemented this yet.

			for (yy = 0; yy < h; yy++)
			{
				for (xx = 0; xx < w; xx++)
				{
					if (!(bit++ & 7))
					{
						bits = bitmap[bo++];
					}
					if (bits & 0x80)
					{
						if (size_x == 1 && size_y == 1)
						{
							pixel(x + xo + xx, y + yo + yy, color);
						}
						else
						{
							rectangle(x + (xo16 + xx) * size_x, y + (yo16 + yy) * size_y, size_x, size_y, color, color);
						}
					}
					bits <<= 1;
				}
			}
		}
	}


	/**************************************************************************/
	/*!
	      @brief  Helper to determine size of a character with current font/size.
	              Broke this out as it's used by both the PROGMEM- and RAM-resident
	              getTextBounds() functions.
	      @param  c     The ASCII character in question
	      @param  x     Pointer to x location of character. Value is modified by
	                    this function to advance to next character.
	      @param  y     Pointer to y location of character. Value is modified by
	                    this function to advance to next character.
	      @param  minx  Pointer to minimum X coordinate, passed in to AND returned
	                    by this function -- this is used to incrementally build a
	                    bounding rectangle for a string.
	      @param  miny  Pointer to minimum Y coord, passed in AND returned.
	      @param  maxx  Pointer to maximum X coord, passed in AND returned.
	      @param  maxy  Pointer to maximum Y coord, passed in AND returned.
	 */
	/**************************************************************************/
	void bounds(unsigned char c, uint16_t *x, uint16_t *y, uint16_t *minx, uint16_t *miny, uint16_t *maxx, uint16_t *maxy)
	{
		if (gfxFont)
		{
			if (c == '\n')
			{ // Newline?
				*x = 0;        // Reset x to zero, advance y by one line
				*y += textsize_y * gfxFont->yAdvance;
			}
			else if (c != '\r')
			{ // Not a carriage return; is normal char
				GFXglyph *glyph = gfxFont->glyph + c;
				if ((c >= gfxFont->first) && (c <= gfxFont->last))
				{ // Char present in this font?
					if (wrap && ((*x + ((glyph->xOffset + glyph->width) * textsize_x)) > width))
					{
						*x = 0; // Reset x to zero, advance y by one line
						*y += textsize_y * gfxFont->yAdvance;
					}
					int16_t tsx = (int16_t)textsize_x, tsy = (int16_t)textsize_y,
							x1 = *x + glyph->xOffset * tsx, y1 = *y + glyph->yOffset * tsy, x2 = x1 + glyph->width * tsx - 1,
							y2 = y1 + glyph->height * tsy - 1;
					if (x1 < *minx)
						*minx = x1;
					if (y1 < *miny)
						*miny = y1;
					if (x2 > *maxx)
						*maxx = x2;
					if (y2 > *maxy)
						*maxy = y2;
					*x += glyph->xAdvance * tsx;
				}
			}

		}
		else
		{ // Default font
			if (c == '\n')
			{        // Newline?
				*x = 0;               // Reset x to zero,
				*y += textsize_y * 8; // advance y one line
				// min/max x/y unchaged -- that waits for next 'normal' character
			}
			else if (c != '\r')
			{ // Normal char; ignore carriage returns
				if (wrap && ((*x + textsize_x * 6) > width))
				{ // Off right?
					*x = 0;                                       // Reset x to zero,
					*y += textsize_y * 8;                         // advance y one line
				}
				int x2 = *x + textsize_x * 6 - 1, // Lower-right pixel of char
						y2 = *y + textsize_y * 8 - 1;
				if (x2 > *maxx)
					*maxx = x2; // Track max x, y
				if (y2 > *maxy)
					*maxy = y2;
				if (*x < *minx)
					*minx = *x; // Track min x, y
				if (*y < *miny)
					*miny = *y;
				*x += textsize_x * 6; // Advance x one char
			}
		}
	}

	/**************************************************************************/
	/*!
	      @brief  Helper to determine size of a string with current font/size.
	              Pass string and a cursor position, returns UL corner and W,H.
	      @param  str  The ASCII string to measure
	      @param  x    The current cursor X
	      @param  y    The current cursor Y
	      @param  x1   The boundary X coordinate, returned by function
	      @param  y1   The boundary Y coordinate, returned by function
	      @param  w    The boundary width, returned by function
	      @param  h    The boundary height, returned by function
	 */
	/**************************************************************************/
	void bounds(const char *str, uint16_t x, uint16_t y, uint16_t *x1, uint16_t *y1, uint16_t *w, uint16_t *h)
	{
		uint8_t c; // Current character
		uint16_t minx = 0x7FFF, miny = 0x7FFF, maxx = -1, maxy = -1; // Bound rect
		// Bound rect is intentionally initialized inverted, so 1st char sets it

		*x1 = x; // Initial position is value passed in
		*y1 = y;
		*w = *h = 0; // Initial size is zero

		while ((c = *str++))
		{
			// charBounds() modifies x/y to advance for each character,
			// and min/max x/y are updated to incrementally build bounding rect.
			bounds(c, &x, &y, &minx, &miny, &maxx, &maxy);
		}

		if (maxx >= minx)
		{     // If legit string bounds were found...
			*x1 = minx;           // Update x1 to least X coord,
			*w = maxx - minx + 1; // And w to bound rect width
		}
		if (maxy >= miny)
		{ // Same for height
			*y1 = miny;
			*h = maxy - miny + 1;
		}
	}


	void setTextSize(uint8_t s)
	{
		setTextSize(s, s);
	}

	void setTextSize(uint8_t sx, uint8_t sy)
	{
		assert (sx > 0);
		assert (sy > 0);
		textsize_x = sx;
		textsize_y = sy;
	}

	void font(const GFXfont *f = NULL)
	{
		if (f)
		{          // Font struct pointer passed in?
			if (!gfxFont)
			{ // And no current font struct?
				// Switching from classic to new font behavior.
				// Move cursor pos down 6 pixels so it's on baseline.
				cursor_y += 6;
			}
		}
		else if (gfxFont)
		{ // NULL passed.  Current font struct defined?
			// Switching from new to classic font behavior.
			// Move cursor pos up 6 pixels so it's at top-left of char.
			cursor_y -= 6;
		}
		gfxFont = (GFXfont *)f;
	}

	/**********************************************************************/
	/*!
    @brief  Set text cursor location
    @param  x    X coordinate in pixels
    @param  y    Y coordinate in pixels
	 */
	/**********************************************************************/
	void cursor(uint16_t x, uint16_t y)
	{
		cursor_x = x;
		cursor_y = y;
	}

	/**********************************************************************/
	/*!
    @brief   Set text font color with transparant background
    @param   c   16-bit 5-6-5 Color to draw text with
    @note    For 'transparent' background, background and foreground
             are set to same color rather than using a separate flag.
	 */
	/**********************************************************************/
	void textcolor(uint16_t c) { text_color = textbgcolor = c; }

	/**********************************************************************/
	/*!
    @brief   Set text font color with custom background color
    @param   c   16-bit 5-6-5 Color to draw text with
    @param   bg  16-bit 5-6-5 Color to draw background/fill with
	 */
	/**********************************************************************/
	void textcolor(uint16_t c, uint16_t bg)
	{
		text_color = c;
		textbgcolor = bg;
	}

	/**********************************************************************/
	/*!
  @brief  Set whether text that is too long for the screen width should
          automatically wrap around to the next line (else clip right).
  @param  w  true for wrapping, false for clipping
	 */
	/**********************************************************************/
	void textwrap(bool w) { wrap = w; }

	void write(uint8_t c)
	{
		if (!gfxFont)
		{ // 'Classic' built-in font

			if (c == '\n')
			{              // Newline?
				cursor_x = 0;               // Reset x to zero,
				cursor_y += textsize_y * 8; // advance y one line
			}
			else if (c != '\r')
			{       // Ignore carriage returns
				if (wrap && ((cursor_x + textsize_x * 6) > width))
				{ // Off right?
					cursor_x = 0;                                       // Reset x to zero,
					cursor_y += textsize_y * 8; // advance y one line
				}
				character(cursor_x, cursor_y, c, text_color, textbgcolor, textsize_x, textsize_y);
				cursor_x += textsize_x * 6; // Advance x one char
			}

		}
		else
		{ // Custom font
			if (c == '\n')
			{
				cursor_x = 0;
				cursor_y += (int16_t)textsize_y * gfxFont->yAdvance;
			}
			else if (c != '\r')
			{
				uint8_t first = gfxFont->first;
				if (c >= first && c <= gfxFont->last)
				{
					GFXglyph *glyph = gfxFont->glyph + c - first;
					uint8_t w = glyph->width, h = glyph->height;
					if ((w > 0) && (h > 0)) { // Is there an associated bitmap?
						int16_t xo = glyph->xOffset;
						if (wrap && ((cursor_x + textsize_x * (xo + w)) > width))
						{
							cursor_x = 0;
							cursor_y += (int16_t)textsize_y *gfxFont->yAdvance;
						}
						character(cursor_x, cursor_y, c, text_color, textbgcolor, textsize_x, textsize_y);
					}
					cursor_x += glyph->xAdvance * textsize_x;
				}
			}
		}
	}

	void text(uint16_t x, uint16_t y, const char* str)
	{
		cursor(x, y);
		text(str);
	}

	void text(const char* str)
	{
		const char* c = str;
		while(*c != 0)
			write(*c++);
	}

	void spinner(uint16_t x, uint16_t y)
	{
		const static char glyphs[] = "-/|\\";
		static uint8_t pos = 0;

		character(x, y, glyphs[pos++], 0xffff, 0x0000, 2);
		if (pos == 4)
			pos = 0;
	}



	// get current cursor position (get rotation safe maximum values,
	// using: width() for x, height() for y)
	/************************************************************************/
	/*!
    @brief  Get text cursor X location
    @returns    X coordinate in pixels
	 */
	/************************************************************************/
	int16_t cursorx(void) const { return cursor_x; }

	/************************************************************************/
	/*!
    @brief      Get text cursor Y location
    @returns    Y coordinate in pixels
	 */
	/************************************************************************/
	int16_t cursory(void) const { return cursor_y; };

protected:
	uint16_t width;       ///< Display width as modified by current rotation
	uint16_t height;      ///< Display height as modified by current rotation
	uint16_t cursor_x;     ///< x location to start print()ing text
	uint16_t cursor_y;     ///< y location to start print()ing text
	uint16_t text_color;   ///< 16-bit background color for print()
	uint16_t textbgcolor; ///< 16-bit text color for print()
	uint8_t textsize_x;   ///< Desired magnification in X-axis of text to print()
	uint8_t textsize_y;   ///< Desired magnification in Y-axis of text to print()
	uint8_t rotation;     ///< Display rotation (0 thru 3)
	bool wrap;            ///< If set, 'wrap' text at right edge of display
	GFXfont *gfxFont;     ///< Pointer to special font
};


#endif // _ADAFRUIT_GFX_H
