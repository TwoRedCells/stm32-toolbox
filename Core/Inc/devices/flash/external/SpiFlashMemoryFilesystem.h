///	@file       devices/flash/SpiFlashMemoryFilesystem.h
///	@class      SpiFlashMemory
///	@brief      An interface to use external SPI FLASH memory like a filesystem.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_STM32_TOOLBOX_DEVICES_FLASH_SPIFLASHMEMORYFILESYSTEM_H_
#define INC_STM32_TOOLBOX_DEVICES_FLASH_SPIFLASHMEMORYFILESYSTEM_H_

#include <math.h>
#include "tinycrypt/tiny_md5.h"
#include "SpiFlashMemory.h"


/**
 * @brief	An interface to use external SPI FLASH memory like a filesystem.
 */
class SpiFlashMemoryFilesystem : public SpiFlashMemory
{
public:
	static constexpr error ErrorFull = 0x8000;
	static constexpr error ErrorFragmented = 0x4000;
	static constexpr error ErrorFileNotFound = 0x2000;
	static constexpr error ErrorInvalidFileId = 0x1000;
	static constexpr error ErrorFileCorrupt = 0x0800;
	static constexpr uint32_t PageSize = 0x100;
	static constexpr uint32_t SectorSize = 0x1000;

	typedef uint32_t fileid;

	/**
	 * @brief	One entry in the directory.
	 */
	typedef struct DirectoryEntry
	{
		static constexpr uint32_t FILENAME_MAX_LENGTH = 80;

		uint32_t magic_number;   /// Indicates that this directory page is in use.
		fileid id;  /// A unique ID for this file.
		uint32_t sectors;  /// Total sectors this file uses.
		uint32_t index;  /// Index of this sector..
		uint32_t address;  /// Address of this sector.
		char filename[FILENAME_MAX_LENGTH];   /// Filename
		uint32_t length;  /// Length of the file in bytes.
		uint8_t md5[16];  /// MD5 hash of the whole file.
		uint8_t deleted;  /// 0xff if the file is deleted; otherwise 0x00.
		uint8_t _padding[132];

		static constexpr uint32_t MAGIC_NUMBER = 0x947a6e2e;
		static constexpr uint32_t FILE_DELETED = 0xff;
		static constexpr uint32_t FILE_NOT_DELETED = 0x00;

		/**
		 * @brief	Synactic sugar for evaluating file deletion.
		 * @returns	True is the file is deleted; otherwise false.
		 */
		bool is_deleted(void)
		{
			return deleted == FILE_DELETED;
		}

		/**
		 * @brief	Synactic sugar for evaluating file validity.
		 * @returns	True is the file is valid; otherwise false.
		 */
		bool is_valid(void)
		{
			return magic_number == MAGIC_NUMBER;
		}
	} DirectoryEntry;

	static constexpr uint32_t UsableSectorSize = SectorSize - sizeof(DirectoryEntry);

//	typedef struct Sector
//	{
//		DirectoryEntry directory;
//		uint8_t data[SectorSize - sizeof(DirectoryEntry)];
//	} Sector;

	typedef struct IndexEntry
	{
		uint32_t address;
		fileid id;
		uint32_t length;
		bool deleted;
	} IndexEntry;

	/**
	 * @brief	Checks if the filesystem is initialized, and initializes it if it isn't.
	 * @returns	True if successful; otherwise false.
	 */
	bool initialize(void)
	{
		RDID rdid = read_identification();
		if (rdid.capacity == 0)
			return false;
		capacity = pow(2, rdid.capacity);
		update_directory_stats();
		return true;
	}


	/**
	 * @brief	Wipes the filesystem.
	 */
	void wipe(void)
	{
		chip_erase();
		free_space = capacity;
	}


	/**
	 * @brief 	Gets the directory entry at the specified sector.
	 * @param	sector The address of the sector.
	 * @returns	The requested directory entry.
	 */
	DirectoryEntry* read_directory(uint32_t sector)
	{
		static DirectoryEntry entry;
		read(sector, &entry, PageSize);
		return &entry;
	}


	/**
	 * @brief Writes a file to the filesystem.
	 * @param filename The name of the file.
	 * @param data Pointer to the data to write.
	 * @param length Length of the file.
	 * @returns The error code, if any.
	 */
	error write_file(char* filename, void* data, uint32_t length)
	{
		// Overwrite the file if it exists.
		uint32_t id = get_fileid(filename);
		if (id != 0)
			remove(id);

		// Determine requirements and create template for directory entry.
		uint32_t sectors = length / UsableSectorSize + 1;
		uint32_t free_sector = get_free_sector();
		DirectoryEntry entry {
			.magic_number = DirectoryEntry::MAGIC_NUMBER,
			.id = last_id++,
			.sectors = sectors,
			.index = 0,
			.address = free_sector,
			.length = length,
			.deleted = DirectoryEntry::FILE_NOT_DELETED
		};
		tiny_md5((uint8_t*) data, length, entry.md5);
		strcpy((char*) entry.filename, (const char*) filename);

		// Write one sector at a time.
		error e;
		uint32_t remaining = length;
		uint32_t written = 0;
		while (remaining)
		{
			uint32_t size = remaining < UsableSectorSize ? remaining : UsableSectorSize;
			sector_erase(entry.address);
			e = write(entry.address + sizeof(DirectoryEntry), ((uint8_t*)data)+written, size);
			if (e != ErrorNone)
				return e;
			e = write(entry.address, &entry, sizeof(DirectoryEntry));
			if (e != ErrorNone)
				return e;

			entry.index++;
			entry.address = get_free_sector();
			written += size;
			free_space -= SectorSize;
			remaining -= size;
		}
		return ErrorNone;
	}


	/**
	 * @brief	Gets the index of the first available empty sector.
	 */
	uint32_t get_free_sector(void)
	{
		uint32_t sector = 0x000000;
		DirectoryEntry* entry = iterate_directory(true);
		while (entry != nullptr && entry->is_valid())
		{
			sector += SectorSize;
			entry = iterate_directory();
		}
		return sector;
	}


	/**
	 * @brief	Gets the id of the file matching the specified filename.
	 * @param	filename The name of the file.
	 * @returns The file id.
	 */
	fileid get_fileid(char* filename)
	{
		DirectoryEntry* entry = search(filename);
		return entry->id;
	}


	/**
	 * @brief	Reads the specified file.
	 * @param	filename The name of the file.
	 * @param	data Pointer to address to save the file.
	 * @param	length The length of the file.
	 * @returns	The error code, if any.
	 */
	error read_file(char* filename, void* data, uint32_t length)
	{
		fileid id = get_fileid(filename);
		if (id == 0)
			return ErrorFileNotFound;
		return read_file(id, data, length);
	}



	/**
	 * @brief	Searches the directory for the specified file.
	 * @param	filename The name of the file to find.
	 * @returns	Pointer to the directory entry, or nullptr.
	 */
	DirectoryEntry* search(char* filename)
	{
		DirectoryEntry* entry = iterate_directory(true);
		while (entry != nullptr)
		{
			if (!strcmp(entry->filename, filename) && entry->index == 0 && entry->deleted == DirectoryEntry::FILE_NOT_DELETED)
				return entry;
			entry = iterate_directory();
		}
		return nullptr;
	}


	/**
	 * @brief	Reads the specified file.
	 * @param	id The id of the file.
	 * @param	data Pointer to address to save the file.
	 * @param	length The length of the file.
	 * @returns	The error code, if any.
	 */
	error read_file(fileid id, void* data, uint32_t length)
	{
		if (id == 0)
			return ErrorInvalidFileId;

		uint32_t remaining = length;
		uint8_t hash[16], md5[16];
		DirectoryEntry* entry = iterate_directory(true);
		while (entry != nullptr)
		{
			if (entry->id == id)
			{
				uint8_t* target = ((uint8_t*)data) + entry->index * UsableSectorSize;
				uint32_t size = entry->index == entry->sectors-1 ? length % UsableSectorSize : UsableSectorSize;
				read(entry->address + sizeof(DirectoryEntry), target, size);
				if (entry->index == 0)
					memcpy(md5, entry->md5, 16);
				remaining -= size;
			}
			if (remaining == 0)
				break;
			entry = iterate_directory();
		}

		tiny_md5((uint8_t*) data, length, hash);  // Recalculate hash and see if it matches expected.
		if (memcmp(hash, entry->md5, 16))
			return ErrorFileCorrupt;

		return ErrorNone;
	}


	/**
	 * @brief	Iterates through each entry in the directory.
	 * @param	reset If true, the iterator will be reset to the first directory entry.
	 * @returns	The next directory entry.
	 */
	DirectoryEntry* iterate_directory(bool reset=false)
	{
		static uint32_t sector = 0;
		if (reset) sector = 0;

		if (sector >= capacity)
			return nullptr;
		DirectoryEntry* entry = read_directory(sector);
		sector += SectorSize;
		if (!entry->is_valid())
			entry->address = sector;
		return entry;
	}


	/**
	 * @brief	Iterates through each entry in the directory.
	 * @param	reset If true, the iterator will be reset to the first directory entry.
	 * @returns	The next directory entry.
	 */
	DirectoryEntry* iterate_files(bool reset=false)
	{
		DirectoryEntry* entry = iterate_directory(reset);
		while (entry != nullptr)
		{
			if (entry->index == 0 && entry->deleted == DirectoryEntry::FILE_NOT_DELETED)
				return entry;
			entry = iterate_directory();
		}
		return nullptr;
	}


	/**
	 * @brief	Updates tracking information for this volume.
	 */
	void update_directory_stats(void)
	{
		free_space = capacity;
		DirectoryEntry* entry = iterate_directory(true);
		for (uint32_t i=0; i < capacity / SectorSize; i++)
		{
			if (entry->is_valid())
			{
				if(entry->id > last_id)
					last_id = entry->id + 1;
				free_space -= SectorSize;
			}
			entry = iterate_directory();
		}
	}


	/**
	 * @brief	Deletes the specified file by unlinking it from the directory.
	 * @param 	id The file id.
	 * @returns	The error code, if any.
	 */
	void remove(fileid id)
	{
		DirectoryEntry* entry = iterate_directory(true);
		while (entry != nullptr)
		{
			if (entry->is_valid() && entry->id == id)
			{
				sector_erase(entry->address);
				free_space -= SectorSize;
			}
			entry = iterate_directory();
		}
	}


	/**
	 * @brief	Gets the number of free bytes.
	 * @returns The number of free bytes.
	 */
	uint32_t get_free(void)
	{
		return free_space;
	}


	/**
	 * @brief	Gets the number of used bytes.
	 * @returns The number of used bytes.
	 */
	uint32_t get_used(void)
	{
		return capacity - free_space;
	}

private:
	uint32_t capacity = 0;
	uint8_t buffer[PageSize];
	uint32_t free_space;
	uint32_t last_id = 1;
};

#endif /* INC_STM32_TOOLBOX_DEVICES_FLASH_SPIFLASHMEMORYFILESYSTEM_H_ */
