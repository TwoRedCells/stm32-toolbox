///	@file       devices/NeoPixelBinaryFile.h
///	@class      NeoPixelBinaryFile
///	@brief      File format for encoded binary representation of a NeoPixel pattern.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_STM32_TOOLBOX_DEVICES_NEOPIXELBINARYFILE_H_
#define INC_STM32_TOOLBOX_DEVICES_NEOPIXELBINARYFILE_H_

#include <memory.h>

/*
 * @brief 	Plays NeoPixel from binary file (data structure) received from server.
 */
class NeoPixelBinaryFile
{
public:
	/* Neopixel structure that mimics .bin file structure */

	/* File Header - 32 bytes */
	struct FileHeader
	{
		uint32_t pixel_count;		// Number of NeoPixels on each strip
		uint32_t frame_count;		// Number of frames
		uint32_t refresh_time;		// Refresh time in ms
		uint32_t channel_count;		// Channel == NeoPixel strip. Ex. 2 channels for left & right Neopixel panels
		uint32_t global_data4;		// For future use
		uint32_t global_data5;		// ''
		uint32_t global_data6;		// ''
		uint32_t global_data7;		// ''
	} *file_header;

	/* Frame Header - 16 bytes */
	struct FrameHeader
	{
		uint32_t index;		// Incremental number per frame
		uint32_t data1;		// For future use
		uint32_t data2;		// ''
		uint32_t data3;		// ''
	};

	struct Pixel
	{
		uint8_t r;	// Light intensity of RED LED
		uint8_t g;	// Light intensity of GREEN LED
		uint8_t b;	// Light intensity of BLUE LED
		uint8_t w;	// Light intensity of WHITE LED

		Pixel(uint32_t rgbw)
		{
			r = rgbw >> 24 & 0xff;
			g = rgbw >> 16 & 0xff;
			b = rgbw >> 8 & 0xff;
			w = rgbw & 0xff;
		}
	};


	/*
	 * @brief 	Generates an example file with a simple pattern.
	 * @param 	pixels The number of pixels for the pattern.
	 * @param	channels The number of channels.
	 * @note 	This allocates memory that is never freed. Therefore it should never be called more than once.
	 *       	The memory can be manually freed by calling `free_example()`.
	 */
	void generate_example(uint16_t pixels, uint8_t channels)
	{
		static constexpr uint32_t frames = 4;

		// Allocate memory.
		uint32_t required_size = calculate_size(frames, pixels, channels);
		binary = (uint8_t*) malloc(required_size);
		memset(binary, 0, required_size);

		// Populate file header.
		file_header = (FileHeader*) binary;
		file_header->pixel_count = pixels;		// Number of pixels
		file_header->frame_count = frames;		// Number of frames
		file_header->refresh_time = 1000;	    // Refresh time (ms)
		file_header->channel_count = channels;	// Number of channels

		// Generate a pattern.
		Pixel colours[4] = { Pixel(0xff000000), Pixel(0x00ff0000), Pixel(0x0000ff00), Pixel(0x000000ff)};
		for (uint32_t f = 0; f < file_header->frame_count; f++)
		{
			FrameHeader* header = (FrameHeader*) get_frame(f);
			header->index = f;

			for (uint32_t c = 0; c < file_header->channel_count; c++)
			{
				for (uint32_t p = 0; p < file_header->pixel_count; p++)
				{
					Pixel* pixel = get_pixel(f, c, p);
					*pixel = colours[f];
				}
			}
		}
	}


	/**
	 * @brief 	Resets all pixels in the file to zero (off/black).
	 */
	void clear_binary_pixels(void)
	{
		for(uint8_t i = 0; i < file_header->frame_count; i++)
		{
			for(uint8_t j=0; j < file_header->pixel_count; j++)
			{
				Pixel* pixel = get_pixel(i, 0, j);
				pixel->r = 0x00;
				pixel->g = 0x00;
				pixel->b = 0x00;
				pixel->w = 0x00;
			}
		}
	}


	/**
	 * @brief 	Gets the specified pixel.
	 * @param 	frame The frame index.
	 * @param 	pixel The pixel index.
	 */
	Pixel* get_pixel(uint32_t frame, uint8_t channel, uint32_t pixel)
	{
		return (Pixel*) (((uint8_t*)get_frame(frame)) + sizeof(FrameHeader) + channel * file_header->pixel_count * sizeof(Pixel) + pixel * sizeof(Pixel));
	}


	/**
	 * @brief	Validates the length of the specified binary file.
	 * @param 	binary Pointer to the pattern file.
	 * @param 	length Actual length of the binary file.
	 */
	bool validate(uint8_t* binary, uint32_t length)
	{
		FileHeader* header = (FileHeader*) binary;
		return length == calculate_size(header->frame_count, header->pixel_count, header->channel_count);
	}


	/**
	 * @brief 	Parses the supplied binary file into the components of the class.
	 * @param 	binary Pointer to the memory containing the pattern as raw binary data.
	 */
	void load(uint8_t* binary)
	{
		this->binary = binary;
		file_header = (FileHeader*) binary;
	}


	/**
	 * @brief	Get the underlying raw binary data.
	 * @returns	Pointer to the data.
	 */
	uint8_t* get_binary(void)
	{
		return binary;
	}

private:
	/**
	 * @brief 	Calculates the amount of memory required to store and instance of the specified size.
	 * @param 	frames The number of frames.
	 * @param 	pixels The number of pixels.
	 * @param	channels The number of channels.
	 * @returns	The required memory.
	 */
	uint32_t calculate_size(uint32_t frames, uint32_t pixels, uint8_t channels)
	{
		return sizeof(FileHeader) + frames * (sizeof(FrameHeader) + pixels * channels * sizeof(Pixel));
	}


	/**
	 * @brief 	Gets the specified frame.
	 * @param 	frame The frame index.
	 */
	FrameHeader* get_frame(uint32_t frame)
	{
		return (FrameHeader*) (binary + sizeof(FileHeader) + frame * (sizeof(FrameHeader) + file_header->pixel_count * file_header->channel_count * sizeof(Pixel)));
	}


	uint8_t* binary;
	bool allocated = false;
};


#endif /* INC_STM32_TOOLBOX_DEVICES_NEOPIXELBINARYFILE_H_ */
