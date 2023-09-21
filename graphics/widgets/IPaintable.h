///	@file       graphics/widgets/IPaintable.h
///	@class      IWidget
///	@brief      Interface for drawing surface.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef LIB_STM32_TOOLBOX_GRAPHICS_IPAINTABLE_H_
#define LIB_STM32_TOOLBOX_GRAPHICS_IPAINTABLE_H_

template <class TColour>
class IPaintable
{
public:
	/**
	 * Paints a pixel on the canvas.
	 * @param x The x-coordinate.
	 * @param y They-coordinate.
	 * @param c The colour.
	 */
	virtual void pixel(uint32_t x, uint32_t y, TColour c)
	{
		// Abstract class. Do nothing.
	}


	/**
	 * Sets a region for fast painting.
	 * @param x The upper-left x-coordinate.
	 * @param y The upper-left y-coordinate.
	 * @param w The width.
	 * @param h The height.
	 */
	virtual bool start_region(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
	{
		// Can't fit region in the buffer.
		if (w * h * sizeof(TColour) > get_buffer_size())
			return false;

		// Nested regions aren't allowed.
		if (in_transaction)
			return false;

		region_x = x;
		region_y = y;
		region_w = w;
		region_h = h;
		in_transaction = true;
		return true;
	}


	/**
	 * Tiles a drawing region by calling start_region() and end_region dynamically, based on available memory.
	 * @notes Call tile_region from a while loop and put drawing commands inside the loop.
	 * @param x The upper-left x-coordinate.
	 * @param y The upper-left y-coordinate.
	 * @param w The width.
	 * @param h The height.
	 * @returns True if there are more tiles to draw; otherwise false.
	 */
	bool tile_region(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
	{
		static bool tiling = false;
		static uint32_t tile_count = 0;
		static uint32_t tile_rasters = 0;
		static uint32_t current_tile = 0;

		if (tiling)
		{
			end_region();
			if (current_tile == tile_count)
			{
				tiling = false;
				current_tile = 0;
				tile_count = 0;
				tile_rasters = 0;
				return false;
			}
		}

		if (!tiling)
		{
			uint32_t required_buffer = w * h * sizeof(TColour);
			uint32_t actual_buffer = get_buffer_size();
			tile_count = required_buffer / actual_buffer + 1;
			tile_rasters = h / tile_count;
			current_tile = 0;
			tiling = true;
		}

		start_region(x, y+current_tile*tile_rasters, w, tile_rasters);
		current_tile++;
		return true;;
	}


	/**
	 * Ends a fast painting transaction.
	 */
	virtual void end_region(void)
	{
		region_x = 0;
		region_y = 0;
		region_w = 0;
		region_h = 0;
		in_transaction = false;
	}


	/**
	 * Fills the region with a solid colour.
	 * @param The colour.
	 */
	virtual void fill_region(TColour colour)
	{
	}


	/**
	 * Gets the amount of graphics buffer available, in bytes.
	 */
	virtual uint32_t get_buffer_size(void) = 0;

protected:
	uint32_t region_x = 0;
	uint32_t region_y = 0;
	uint32_t region_w = 0;
	uint32_t region_h = 0;
	bool in_transaction = false;
};

#endif
