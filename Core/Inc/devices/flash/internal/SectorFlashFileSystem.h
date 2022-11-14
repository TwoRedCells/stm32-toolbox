///	@file       devices/flash/SectorFlashFilesystem.h
///	@class      SectorFlashFilesystem
///	@brief      Manages a filesystem comprised of one or more sectors.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_STM32_TOOLBOX_DEVICES_FLASH_SECTORFLASHFILESYSTEM_H_
#define INC_STM32_TOOLBOX_DEVICES_FLASH_SECTORFLASHFILESYSTEM_H_

#include "FlashFileSystem.h"
#include "SectorCollection.h"
#include "Directory.h"


/**
 * @brief	Manages a filesystem comprised of one or more sectors.
 */
class SectorFlashFileSystem : FlashFileSystem
{
public:
	/**
	 * @brief	Instantiates a sector file system using the specified sector layout.
	 * @param	sectors A collection describing the locations and sizes of sectors.
	 */
	SectorFlashFileSystem(SectorCollection sectors)
	{
		this->sectors = sectors;
		initialize();
	}


	/**
	 * @brief	Initializes the file system by writing a directory header to each sector.
	 * @returns	True on success; otherwise failure.
	 */
	bool initialize(void) override
	{
		bool result = true;
		for (uint8_t i=0; i< sectors.count; i++)
			if (!sectors.sectors[i].is_initialized())
				if(!sectors.sectors[i].initialize())
					result = false;
		return result;
	}


	/**
	 * @brief	Erases all non-volatile memory.
	 * @returns True on success; otherwise false.
	 */
	bool erase(void) override
	{
		bool result = true;
		for (uint8_t i=0; i< sectors.count; i++)
			if(!erase(i))
				result = false;
		return result;
	}


	/**
	 * @brief	Erases the specified sector.
	 * @param	index The sector to erase.
	 * @returns True on success; otherwise false.
	 */
	bool erase(uint8_t index)
	{
		return sectors.sectors[index].erase();
	}


	/**
	 * @brief	Adds a file to the filesystem.
	 * @param	filename The unique name of the file.
	 * @param	data A pointer to the data to write.
	 * @param	length The number of bytes to write.
	 * @returns	True on success; otherwise false.
	 */
	bool add(const char *filename, uint8_t* data, uint32_t length)  override
	{
#if FLASH_ALLOW_OVERWRITING
		while (search(filename) != nullptr)
			unlink(filename);
#else
		while (search(filename) != nullptr)
			return false;  // Disallow duplicates.

#endif

		for (uint8_t i=0; i< sectors.count; i++)
			if (sectors.sectors[i].get_free() > length)
				return sectors.sectors[i].add(filename, data, length);
		return false;
	}


	/**
	 * @brief	Gets the total number of files across the filesystem.
	 * @returns The number of files.
	 */
	uint32_t get_file_count(void)
	{
		uint32_t count = 0;
		for (uint8_t i=0; i< sectors.count; i++)
			count += sectors.sectors[i].get_file_count();
		return count;
	}


	/**
	 * @brief	Gets the total capacity of the filesystem, including existing files.
	 * @returns The filesystem capacity.
	 */
	uint32_t get_capacity(void)
	{
		uint32_t capacity = 0;
		for (uint8_t i=0; i < sectors.count; i++)
			capacity += (uint32_t) (sectors.sectors[i].get_capacity());
		return capacity;
	}


	/**
	 * @brief	Gets the total number of free bytes.
	 * @returns	The number of free bytes.
	 */
	uint32_t get_free(void)
	{
		uint32_t count = 0;
		for (uint8_t i=0; i< sectors.count; i++)
			count += sectors.sectors[i].get_free();
		return count;
	}


	/**
	 * @brief	Returns an array of DirectoryEntry objects.
	 * @returns	Directoy entries.
	 */
	DirectoryEntry** get_directory_entries(void)
	{
		static DirectoryEntry** entries = nullptr;
		if (entries != nullptr)
			free (entries);

		uint32_t size = get_file_count();
		entries = (DirectoryEntry**) malloc(size * sizeof(DirectoryEntry*));
		uint32_t entry = 0;
		for (uint8_t i=0; i< sectors.count; i++)
			for (uint32_t j=0; j<sectors.sectors[i].get_file_count(); j++)
				entries[entry++] = &sectors.sectors[i].get_directory()->entries[j];
		return entries;
	}


	/**
	 * @brief Searches the directory for a file with the specified name.
	 * @param filename The name of the file.
	 * @returns If found, a pointer to the matching `DirectoryEntry`; otherwise `nullptr`.
	 */
	DirectoryEntry* search(const char* filename)
	{
		DirectoryEntry* entry = nullptr;
		for (uint8_t i=0; i< sectors.count; i++)
		{
			entry = sectors.sectors[i].search(filename);
			if (entry != nullptr)
				return entry;
		}
		return nullptr;
	}



	/**
	 * @brief	Flags are files in this filesystem with the specified filename as deleted.
	 * @param	filename The name of the file to unlink.
	 * @returns True on success; false on error, or if the file does not exist.
	 * @note	This will not delete the file, only mark it as deleted. No space will be freed.
	 */
	bool unlink(const char* filename) override
	{
		DirectoryEntry* entry = nullptr;
		for (uint8_t i=0; i< sectors.count; i++)
		{
			entry = sectors.sectors[i].search(filename);
			if (entry != nullptr)
				return sectors.sectors[i].unlink(entry);
		}
		return false;
	}



private:
	SectorCollection sectors;
};

#endif /* INC_STM32_TOOLBOX_DEVICES_FLASH_SECTORFLASHFILESYSTEM_H_ */
