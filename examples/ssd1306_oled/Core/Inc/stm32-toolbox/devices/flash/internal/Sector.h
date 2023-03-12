///	@file       devices/flash/Sector.h
///	@class      Sector
///	@brief      A sector is the smallest unit of non-volatile memory that can be erased at once. It can hold multiple files.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_STM32_TOOLBOX_DEVICES_FLASH_SECTOR_H_
#define INC_STM32_TOOLBOX_DEVICES_FLASH_SECTOR_H_

#include "stm32/stm32.h"
#include "Directory.h"
#include "FlashMemory.h"


/**
 * @brief A unit of FLASH memory on an MCU that uses sectors.
 */
class Sector : public FlashMemory
{
public:
	/**
	 * @brief	Instantiates a Sector object.
	 * @param	index The index that identifies this instance.
	 * @param 	start The first memory location of this sector.
	 * @param	end The last memory location of this sector.
	 */
	Sector(uint8_t index, void* start, uint32_t length)
	{
		this->index = index;
		this->start = start;
		this->length = length;
		directory = (Directory*) start;
	}


	/**
	 * @brief Initializes this sector by writing its directory header.
	 */
	bool initialize(void) override
	{
		Directory local;
		local.header.magic_number = Directory::FLASH_DIRECTORY_MAGIC_NUMBER;

		if (!erase())
			return false;

		return write((uint32_t*) &local, (uint32_t*) directory, sizeof(DirectoryHeader));
	}


	/**
	 * @brief Erases one or more sectors of memory.
	 * @returns True on success; false on failure.
	 * @note Any FLASH that has not been written to after an erase operation can be written to.
	 */
	bool erase(uint8_t count)
	{
		bool result = true;
		FLASH_EraseInitTypeDef erase_struct = {
				.TypeErase   = FLASH_TYPEERASE_SECTORS,
				.Sector = index,
				.NbSectors = count,
				.VoltageRange = FLASH_VOLTAGE_RANGE_3
		};

		unlock();
		clear_error_flags();
		uint32_t error = 0;
		if (HAL_FLASHEx_Erase(&erase_struct, &error) != HAL_OK)
		{
			//uint32_t errorcode = HAL_FLASH_GetError();  // Break here for debugging.
			result = false;
		}
		lock();
		return result;
	}


	/**
	 * @brief Erases the entire sector of FLASH memory.
	 * @returns True on success; false on failure.
	 * @note Any FLASH that has not been written to after an erase operation can be written to.
	 */
	bool erase(void) override
	{
		return erase(1);
	}


	/**
	 * @brief Writes the contents of RAM to FLASH memory.
	 * @param data A pointer to the data in FLASH.
	 * @param target A pointer to the destination memory.
	 * @param length The number of bytes to copy.
	 * @returns True on success; false on failure.
	 */
	bool write(void* data, void* target, uint32_t length) override
	{
		uint32_t* d = (uint32_t*) data;
		uint32_t* t = (uint32_t*) target;
		bool result = true;
		unlock();
		clear_error_flags();
		uint32_t words = (length+3) / sizeof(uint32_t);  // Round up to nearest word.
		for (uint32_t i=0; i<words; i++)
		{
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (uint32_t)(t+i), *(d+i)) != HAL_OK)
			{
				//uint32_t errorcode = HAL_FLASH_GetError();
				result = false;
			}
		}
		lock();
		return result;
	}


	/**
	 * Adds a file to the filesystem.
	 * @param filename The name of the file.
	 * @param data A pointer to the file data.
	 * @param length The length of the file.
	 */
	bool add(const char *filename, uint8_t* data, uint32_t length) override
	{
		// Prepare directory entry.
		DirectoryEntry entry = {
				.magic_number = DirectoryEntry::FLASH_FILE_MAGIC_NUMBER,
				.sector_index = index,
				.length = length,
				.location = get_next_file_address(),
				.not_deleted = DirectoryEntry::FILE_NOT_DELETED,
		};
		strcpy(entry.filename, filename);
		tiny_md5(data, length, entry.md5);

		unlock();
		uint16_t count = get_file_count();

		// Write the file.
		if (!write((uint32_t*)data, (uint32_t*)entry.location, entry.length))
			goto fail;

		// Write the directory entry.
		if (!write_directory_entry(&entry))
			goto fail;

		return verify((uint32_t*)data, (uint32_t*)entry.location, entry.length) &&
				verify((uint32_t*)&entry, (uint32_t*)&directory->entries[count], sizeof(DirectoryEntry));

		fail:
		lock();
		return false;
	}


	/**
	 * @brief	Flags are files in this memory with the specified filename as deleted.
	 * @param	entry The file to unlink
	 * @returns True on success; false on error, or if the file does not exist.
	 * @note	This may not necessarily delete the file, but will flag it as deleted.
	 */
	bool unlink(DirectoryEntry* entry) override
	{
		if (entry == nullptr)
			return false;

		DirectoryEntry copy = *entry;
		copy.not_deleted = DirectoryEntry::FILE_DELETED;
		if (!write_directory_entry(entry, &copy))
			return false;

		return true;
	}


	/**
	 * @brief	Gets the usable capacity of this sector.
	 * @returns	The usable capacity in bytes.
	 */
	uint32_t get_capacity(void)
	{
		return length - sizeof(Directory);
	}


	/**
	 * @brief	Gets the total size of this sector.
	 * @returns	The size in bytes.
	 */
	uint32_t get_size(void)
	{
		return length;
	}


	/**
	 * @brief	Gets the start of the address space.
	 * @returns	The starting address.
	 */
	void* get_start(void)
	{
		return start;
	}

private:

	/**
	 * @brief Clears the FLASH error flags, required before erasing or writing to FLASH.
	 */
	void clear_error_flags(void)
	{
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_WRPERR);
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPERR);
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGAERR);
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGPERR);
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGSERR);
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_BSY);
	}

	uint8_t index;
	void* start;
	uint32_t length;
};

#endif /* INC_STM32_TOOLBOX_DEVICES_FLASH_SECTOR_H_ */
