///	@file       devices/FlashFileSystem.h
///	@class      FlashFileSystem
///	@brief      Facilitates reading and writing from the MCU's FLASH memory.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#include <stdint.h>

#ifndef INC_DEVICES_FLASHFILESYSTEM_H_
#define INC_DEVICES_FLASHFILESYSTEM_H_


#if FLASH_LAYOUT_PAGES
#error "FLASH page layouts aren't supported yet."
#endif
#include "Directory.h"

/// <summary>
/// Facilitates reading and writing from the MCU's FLASH memory.
/// </summary>
/// <remarks>
/// Please read RM0383 chapter 3 before using this class. Depending on the model of your chip, the frequency at which
/// it is running, and the supply voltage, configuration may require adjustment.
///
/// Different models of STM32 MCUs use different memory management techniques. Some define a collection of pages, each
/// page being uniform in size, while others use non-uniformly sized sectors. These different schemes are handled
/// differently in code, so there isn't a one-size-fits-all approach that will work universally. Currently this class
/// only supports MCUs that use sectors, and it has only been tested with the STM32F411, which has 512kB of FLASH,
/// organized into 8 sectors.
/// </remarks>
class FlashFileSystem
{
public:
	/**
	 * @brief	Initializes the file system by writing a directory header to each sector.
	 * @returns	True on success; otherwise failure.
	 */
	virtual bool initialize(void) = 0;


	/**
	 * @brief	Erases all non-volatile memory.
	 * @returns True on success; otherwise false.
	 */
	virtual bool erase(void) = 0;


	/**
	 * @brief	Adds a file to the filesystem.
	 * @param	filename The unique name of the file.
	 * @param	data A pointer to the data to write.
	 * @param	length The number of bytes to write.
	 * @returns	True on success; otherwise false.
	 */
	virtual bool add(const char *filename, uint8_t* data, uint32_t length) = 0;


	/**
	 * @brief	Flags are files in this filesystem with the specified filename as deleted.
	 * @param	filename The name of the file to unlink.
	 * @returns True on success; false on error, or if the file does not exist.
	 * @note	This will not delete the file, only mark it as deleted. No space will be freed.
	 */
	virtual bool unlink(const char* filename) = 0;


	/**
	 * @brief	Gets the total number of files across the filesystem.
	 * @returns The number of files.
	 */
	virtual uint32_t get_file_count(void) = 0;

	/**
	 * @brief	Gets the total number of free bytes.
	 * @returns	The number of free bytes.
	 */
	virtual uint32_t get_free(void) = 0;
};

#endif /* INC_DEVICES_FLASHMEMORY_H_ */
