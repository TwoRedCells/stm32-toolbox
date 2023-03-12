///	@file       devices/flash/Directory.h
///	@class      Directory
///	@brief      The structures that are building blocks for a directory in a filesystem.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_STM32_TOOLBOX_DEVICES_FLASH_DIRECTORY_H_
#define INC_STM32_TOOLBOX_DEVICES_FLASH_DIRECTORY_H_

#include <stdint.h>
#include <string.h>
#include "stm32-toolbox/constants.h"
#include "tinycrypt/tiny_md5.h"
#include "DirectoryEntry.h"
#include "DirectoryHeader.h"


typedef struct Directory
{
	DirectoryHeader header;
	DirectoryEntry entries[FLASH_DIRECTORY_SIZE];

	static constexpr uint32_t FLASH_DIRECTORY_MAGIC_NUMBER = 0x93f61b39;  // If this isn't the first thing in the directory then it probably doesn't exist.


	/**
	 * @brief	Searches the directory for the specified filename.
	 * @param	filename The name of the file.
	 * @returns	A pointer to the file's DirectoryEntry if found; otherwise nullptr.
	 */
	virtual DirectoryEntry* search(const char* filename)
	{
		for (uint16_t i=0; i < FLASH_DIRECTORY_SIZE; i++)
			if (entries[i].is_active() && !strcmp(filename, entries[i].filename))
				return &entries[i];
		return nullptr;
	}

} Directory;

#endif /* INC_STM32_TOOLBOX_DEVICES_FLASH_DIRECTORY_H_ */
