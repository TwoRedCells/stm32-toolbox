///	@file       devices/flash/DirectoryEntry.h
///	@struct     DirectoryEntry
///	@brief      Represents one record (file) in a filesystem directory.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_STM32_TOOLBOX_DEVICES_FLASH_DIRECTORYENTRY_H_
#define INC_STM32_TOOLBOX_DEVICES_FLASH_DIRECTORYENTRY_H_


#include <stdint.h>
#include <memory.h>
#include "tinycrypt/tiny_md5.h"


/**
 * @brief A record in the directory.
 */
typedef struct DirectoryEntry
{
	uint32_t magic_number;  /// Unique identifier for this file.
	uint8_t sector_index;   /// The index of the sector in which the file is stored.
	char filename[INTERNAL_FLASH_FILENAME_LENGTH];    /// The name of the file to be stored.
	uint32_t length;        /// Length of the file in bytes.
	uint8_t* location;     /// Location of the stored file.
	uint8_t not_deleted;		/// Deleted when equal to 0x00.
	uint8_t md5[16];		/// Hash of the file when saved.
	uint8_t _padding[66];	/// End on word boundary (and space for future).

	static constexpr uint32_t FLASH_FILE_MAGIC_NUMBER = 0x7a3b4411;
	static constexpr uint8_t FILE_DELETED = 0x00;
	static constexpr uint8_t FILE_NOT_DELETED = 0xff;


	/**
	 * @brief	Determines whether this DirectoryEntry is in use.
	 * @returns	True if the entry is in use; otherwise false.
	 */
	bool is_active(void)
	{
		return magic_number == FLASH_FILE_MAGIC_NUMBER;
	}


	/**
	 * @brief	Determines whether this DirectoryEntry is deleted (unlinked).
	 * @returns	True if the entry is unlinked; otherwise false.
	 */
	bool is_deleted(void)
	{
		return not_deleted == FILE_DELETED;
	}


	/**
	 * @brief	Checks whether the calculated hash of the file matches the stored hash.
	 */
	bool validate_md5(void)
	{
		uint8_t hash[16];
		tiny_md5(location, length, hash);
		return !memcmp(hash, md5, 16);
	}


	/**
	 * @brief	Calculates the MD5 hash and sets the member value.
	 */
	void calculate_md5(void)
	{
		tiny_md5(location, length, md5);
	}

} DirectoryEntry;

#endif /* INC_STM32_TOOLBOX_DEVICES_FLASH_DIRECTORYENTRY_H_ */
