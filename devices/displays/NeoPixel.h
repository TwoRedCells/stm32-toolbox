///	@file       devices/NeoPixel.h
///	@class      NeoPixel
///	@brief      Controls a strip of individually addressable LEDs.
///  Usage:     Create a PWM timer with DMA at 48MHz.
/// 	        NeoPixel neo(NUMBER_OF_PIXELS);
///             HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t*)neo.get_buffer(), neo.get_buffer_length());
///             neo.set(0x00ff0000);
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_NEOPIXEL_H_
#define INC_NEOPIXEL_H_

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "utility/Timer.h"

#if ENABLE_NEOPIXEL_BINARYFILE
#include "devices/displays/NeoPixelBinaryFile.h"
#endif

/**
 * Controls a strip of individually addressable LEDs.
 */
class NeoPixel
{
public:
	static const uint8_t LED_PATTERN_SOLID = 0;
	static const uint8_t LED_PATTERN_CLEAR = 4;
#if ENABLE_NEOPIXEL_BUILTIN_PATTERNS
	static const uint8_t LED_PATTERN_FLASH = 1;
	static const uint8_t LED_PATTERN_ALTERNATE = 2;
	static const uint8_t LED_PATTERN_CHASE = 3;
	static const uint8_t LED_PATTERN_FADE = 6;
	static const uint8_t LED_PATTERN_BLINK = 7;
	static const uint8_t LED_PATTERN_RAINBOW = 8;
#endif
#if ENABLE_NEOPIXEL_DEMO_PATTERN
	static const uint8_t LED_PATTERN_DEMO = 5;
#endif
#if ENABLE_NEOPIXEL_BINARYFILE
	static const uint8_t LED_PATTERN_BINARYFILE = 9;
#endif

#if LED_TYPE_RGB
	static const uint8_t LED_SIZE = 24;
	static const uint32_t RED = 0x00ff00;
	static const uint32_t GREEN = 0xff0000;
	static const uint32_t BLUE = 0x0000ff;
	static const uint32_t WHITE = 0x000000;
	static const uint32_t CYAN = 0xff00ff;
	static const uint32_t MAGENTA = 0x00ffff;
	static const uint32_t YELLOW = 0xffff00;
	static const uint32_t BLACK = 0x000000;
	static const uint8_t MAX_INTENSITY = 0xff;
#else
	static const uint8_t LED_SIZE = 32;
	static const uint32_t RED = 0x00ff0000;
	static const uint32_t GREEN = 0xff000000;
	static const uint32_t BLUE = 0x0000ff00;
	static const uint32_t WHITE = 0x000000ff;
	static const uint32_t CYAN = 0xff00ff00;
	static const uint32_t MAGENTA = 0x00ffff00;
	static const uint32_t YELLOW = 0xffff0000;
	static const uint32_t BLACK = 0x00000000;
	static const uint8_t MAX_INTENSITY = 0xff;
#endif

	// The timer runs from 0 to 119. These represent the duty cycle expected by the LED to represent a 0 or 1.
	const uint32_t PWM_HIGH = 76;
	const uint32_t PWM_LOW = 38;


	/**
	 * @brief Instantiates a NeoPixel strip.
	 * @param length The number of LEDs in the strip.
	 */
	NeoPixel(uint32_t length, uint32_t* buffer)
	{
		this->length = length;
		pixel_buffer = buffer;
		dma_buffer = buffer + length;

		// We use "empty" pixels at the end to achieve a timing pause.
		for (int i=0; i<LED_SIZE*2; i++)
			dma_buffer[i] = 0;

		// 64 empty bits at the end.
		for (int i=0; i<LED_SIZE*2; i++)
			dma_buffer[length*LED_SIZE+i] = 0;

		clear();
	}

	/**
	 * @brief Turns all LEDs in the strip off.
	 */
	void clear(void)
	{
		set(0x00000000);
		pattern = LED_PATTERN_SOLID;
	}


	/**
	 * @brief Sets the colour of an individual LED.
	 * @param index The zero-based index of the LED to set.
	 * @param colour The GBRW value of the colour.
	 */
	void set(uint32_t index, uint32_t colour)
	{
#if LED_TYPE_RGB
		pixel_buffer[index] = colour >> 8;
#else
		pixel_buffer[index] = colour;
#endif
		set_dma_buffer(index);
	}


	/**
	 * @brief Sets the colour of all the LEDs in the strip.
	 * @param colour The GBRW value of the colour.
	 */
	void set(uint32_t colour)
	{
		for (uint32_t i=0; i<length; i++)
			set(i, colour);
	}


	/**
	 * @brief Gets a pointer to the pixel buffer.
	 * @return A pointer to the pixel buffer.
	 */
	uint32_t* get_dma_buffer(void)
	{
		return dma_buffer;
	}


	/**
	 * @brief Gets the length of the pixel buffer, in bytes.
	 * @return The length of the pixel buffer.
	 */
	uint32_t get_dma_buffer_length(void)
	{
		// We add two empty spots to the beginning and end.
		return (length+4) * LED_SIZE;
	}


	/**
	 * @brief Sets the colour of all the LEDs in the strip and enters the solid pattern state.
	 * @param colour The GBRW value of the colour.
	 */
	void solid(uint32_t colour)
	{
		pattern = LED_PATTERN_SOLID;
		set(colour);
	}

#if ENABLE_NEOPIXEL_BUILTIN_PATTERNS
	/**
	 * @brief Flashes all of the LEDs the same colour repeatedly.
	 * @param colour The GBRW value of the colour.
	 * @param on_time The duration to keep the LEDS lit.
	 * @param off_time The duration to turn the LEDs off.
	 */
	void flash(uint32_t colour, uint32_t on_time, uint32_t off_time)
	{
		pattern = LED_PATTERN_FLASH;
		this->colour = colour;
		this->ontime = milliseconds(on_time);
		this->offtime = milliseconds(off_time);
	}


	/**
	 * @brief Alternates flashing of a colour between two LED strips.
	 * @param colour The GBRW value of the colour.
	 * @param speed The duration to wait between alternating sides.
	 * @param other A pointer to another NeoPixel instance.
	 */
	void alternate(uint32_t colour, uint32_t speed, NeoPixel* other)
	{
		pattern = LED_PATTERN_ALTERNATE;
		this->colour = colour;
		this->ontime = speed;
		this->offtime = 0;
		this->other = other;
		timer.start(milliseconds(speed));
		set(colour);
	}


	/**
	 * @brief Runs a chase or marquee pattern.
	 * @param colour The GBRW value of the colour.
	 * @param speed The duration to wait between movements.
	 */
	void chase(uint32_t colour, uint32_t speed)
	{
		clear();
		pattern = LED_PATTERN_CHASE;
		this->colour = colour;
		this->ontime = milliseconds(speed);
		this->offtime = 0;
	}


	/**
	 * @brief Fades the specified colour in and out.
	 * @param colour The GBRW value of the colour.
	 * @param speed The duration to wait between movements.
	 */
	void fade(uint32_t colour, uint32_t speed)
	{
		clear();
		pattern = LED_PATTERN_FADE;
		this->colour = colour;
		this->ontime = milliseconds(speed);
		this->offtime = 0;
		state = 0;
	}


	/**
	 * @brief Fades the specified colour in and out, starting at the middle and expanding to the outside.
	 * @param colour The GBRW value of the colour.
	 * @param speed The duration to wait between movements.
	 */
	void blink(uint32_t colour, uint32_t speed)
	{
		clear();
		pattern = LED_PATTERN_BLINK;
		this->colour = colour;
		this->ontime = milliseconds(speed);
		this->offtime = 0;
		state = 0;
	}


	/**
	 * @brief Initiates a colourful sequence.
	 */
	void rainbow(uint32_t speed)
	{
		pattern = LED_PATTERN_RAINBOW;
		this->ontime = milliseconds(speed);
		this->offtime = 0;
		state = 0;
	}
#endif

#if ENABLE_NEOPIXEL_DEMO_PATTERN
	/**
	 * @brief Initiates a demo sequence for testing that the hardware is operating correctly.
	 */
	void demo(void)
	{
		pattern = LED_PATTERN_DEMO;
		state = 0;
	}
#endif

#if ENABLE_NEOPIXEL_BINARYFILE
	/*
	 * @brief Initiates sequence defined by received .bin file
	 */
	void binaryfile(NeoPixelBinaryFile *binaryfile, uint8_t channel)
	{
		clear();
		pattern = LED_PATTERN_BINARYFILE;
		binary_file = binaryfile;
		binary_channel = channel;
		binary_pixel_count = binary_file->file_header->pixel_count;
		binary_channel_count = binary_file->file_header->channel_count;
		binary_frame_count = binary_file->file_header->frame_count;
		binary_refresh_time = binary_file->file_header->refresh_time;
		binary_frame_length = sizeof(NeoPixelBinaryFile::FrameHeader) + binary_channel_count * sizeof(NeoPixelBinaryFile::Pixel) * binary_pixel_count;
		binary_frame_index = 0;
	}
#endif


	/**
	 * @brief Called in a loop to allow the class to update dynamic (e.g. flashing) patterns.
	 */
	void loop(void)
	{
#if ENABLE_NEOPIXEL_DEMO_PATTERN
		if (pattern == LED_PATTERN_DEMO)
			loop_demo();
#endif
#if ENABLE_NEOPIXEL_BUILTIN_PATTERNS
		else if (pattern == LED_PATTERN_FLASH)
			loop_flash();
		else if (pattern == LED_PATTERN_ALTERNATE)
			loop_alternate();
		else if (pattern == LED_PATTERN_CHASE)
			loop_chase();
		else if (pattern == LED_PATTERN_FADE)
			loop_fade();
		else if (pattern == LED_PATTERN_BLINK)
			loop_blink();
		else if (pattern == LED_PATTERN_RAINBOW)
			loop_rainbow();
#endif
#if ENABLE_NEOPIXEL_BINARYFILE
		if (pattern == LED_PATTERN_BINARYFILE)
			loop_binaryfile();
#endif
	}


private:
	/**
	 * @brief Writes the value of the selected index from the pixel buffer to the DMA buffer.
	 * @param index The index to copy.
	 */
	void set_dma_buffer(uint32_t index)
	{
		for (int j=0; j<LED_SIZE; j++)
			dma_buffer[(index+2)*LED_SIZE+j-1] = (pixel_buffer[index] >> (LED_SIZE-j)) & 0x01 ? PWM_HIGH : PWM_LOW;

		dma_buffer[LED_SIZE+0x1f] = 0;
	}


	void loop_demo(void)
	{
		uint32_t colours[] = { 0xff000000, 0x00ff0000, 0x00ffff00, 0x0000ff00, 0x0000ffff, 0x000000ff, 0x00000000 };
		uint8_t len = sizeof(colours)/sizeof(colours[0]);

		if (timer.is_elapsed() || !timer.is_started())
		{
			if (state == len*10)
			{
				timer.reset();
				pattern = LED_PATTERN_SOLID;
				return;
			}

			set(colours[state % len]);
			timer.start(milliseconds(500));
			state++;
		}
	}


#if ENABLE_NEOPIXEL_BUILTIN_PATTERNS
	void loop_flash()
	{
		{
			if (timer.is_elapsed() || !timer.is_started())
			{
				if (!state)
				{
					set(colour);
					timer.start(ontime);
				}
				else
				{
					set(BLACK);
					timer.start(offtime);
				}
				state = state ? 0 : 1;
			}
		}
	}


	void loop_alternate(void)
	{
		if (timer.is_elapsed() || !timer.is_started())
		{
			clear();
			other->alternate(colour, ontime, this);
		}
	}


	void loop_chase(void)
	{
		if (timer.is_elapsed() || !timer.is_started())
		{
			set(state++, 0x00000000);
			if (state == length)
				state = 0;
			set(state, colour);
			timer.start(ontime);
		}
	}


	void loop_fade(void)
	{
		static int8_t direction = +1;

		if (timer.is_elapsed() || !timer.is_started())
		{
			if (state == MAX_INTENSITY)
				direction = -1;
			else if (state == 0)
				direction = +1;
			float g = (float)(colour >> 24) * state/0x100;
			float w = (float)(colour & 0xff) * state/0x100;
			float b = (float)((colour >> 8) & 0xff) * state/0x100;
			float r = (float)((colour >> 16) & 0xff) * state/0x100;
			uint32_t wbgr = (uint32_t)g << 24 | (uint32_t)r << 16 | (uint32_t)b << 8 | (uint32_t)w;
			state += direction;
			set(wbgr);
			timer.start(ontime);
		}
	}


	void loop_blink(void)
	{
		static int8_t direction = +1;

		if (timer.is_elapsed() || !timer.is_started())
		{
			if (state == MAX_INTENSITY)
				direction = -1;
			else if (state == 0)
				direction = +1;
			float g = (float)(colour >> 24) * state/0x100;
			float w = (float)(colour & 0xff) * state/0x100;
			float b = (float)((colour >> 8) & 0xff) * state/0x100;
			float r = (float)((colour >> 16) & 0xff) * state/0x100;
			uint32_t wbgr = (uint32_t)g << 24 | (uint32_t)r << 16 | (uint32_t)b << 8 | (uint32_t)w;
			uint16_t centre = length / 2;
			uint16_t pixels_lit = (uint16_t)((float)(length+1) / 2 / MAX_INTENSITY * state);
			for (uint16_t i=0; i<length; i++)
				set(i, abs(centre-i) < pixels_lit ? wbgr : BLACK);
			state += direction;
			timer.start(ontime);
		}
	}

	uint32_t rgbw_to_grbw(uint8_t r, uint8_t g, uint8_t b, uint8_t w=0)
	{
		return (uint32_t)r << 16 | g << 24 | b << 8 | w;
	}


	void loop_rainbow(void)
	{
		static int8_t direction = +1;

		if (timer.is_elapsed() || !timer.is_started())
		{
			if (state == 0xff)
				direction = -1;
			else if (state == 0)
				direction = +1;

			state += direction;

			uint8_t pos = 0xff - state;

			if (pos < 85)
				set(rgbw_to_grbw(0xff-pos*3, 0, pos*3));
			else if (state < 170)
			{
				pos -= 85;
				set(rgbw_to_grbw(0, pos*3, 0xff-pos*3));
			}
			else
			{
				pos -= 170;
				set(rgbw_to_grbw(pos*3, 0xff-pos*3, 0));
			}
			//			uint32_t rgb = hsl_to_rgb(state, 1.0, (float)MAX_INTENSITY/0xff/2.0);
			//			uint32_t rgb = hsl_to_rgb(state, 1.0, 0.45);
			//			uint32_t grbw = wrgb_to_grbw(rgb);
			//set(grbw);
			timer.start(ontime);
		}
	}
#endif

#if ENABLE_NEOPIXEL_BINARYFILE
	NeoPixelBinaryFile *binary_file;
	uint8_t binary_channel;
	uint32_t binary_frame_index;
	uint32_t binary_pixel_count;
	uint32_t binary_channel_count;
	uint32_t binary_frame_count;
	uint32_t binary_refresh_time;
	uint32_t binary_frame_length;

	void loop_binaryfile(void)
	{
		// The memory may have needed to be freed in order to complete another operation.
		if (binary_file->get_binary() == nullptr)
			return;

		if (timer.is_elapsed() || !timer.is_started())
		{
			uint32_t* frame_data = (uint32_t*)(
					binary_file->get_binary() + sizeof(NeoPixelBinaryFile::FileHeader)
					+ binary_frame_index * binary_frame_length
					+ sizeof(NeoPixelBinaryFile::FrameHeader) + binary_channel * sizeof(NeoPixelBinaryFile::Pixel) * binary_pixel_count);

			for (uint32_t j = 0; j < binary_pixel_count; j++)
				set(j, frame_data[j]);

			if (binary_frame_index++ == binary_frame_count - 1)
				binary_frame_index = 0;

			timer.start(milliseconds(binary_refresh_time));
		}
	}
#endif


private:
	uint32_t length;  /// The number of pixels.
	uint32_t* dma_buffer;  /// The DMA buffer.
	uint8_t pattern;  // The current mode (pattern type).
	Timer timer;  /// A timer used to control dynamic patterns.
	uint32_t ontime, offtime;  // The on-time/off-time for flashing pattersn.
	uint32_t colour;  /// The colour to display.
	uint8_t state = 0;  // Used to keep internal state, may be used differently by different patterns.
	uint32_t* pixel_buffer;;  /// The pixel buffer, each pixel in WBRG format.
	NeoPixel *other;  /// Another instance to coordinate with.
};

#endif /* INC_NEOPIXEL_H_ */
