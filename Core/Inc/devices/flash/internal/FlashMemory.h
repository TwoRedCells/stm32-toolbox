///	@file       devices/flash/FlashMemory.h
///	@class      FlashMemory
///	@brief      Abstract class for non-volatile memory.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_STM32_TOOLBOX_DEVICES_FLASH_FLASHMEMORY_H_
#define INC_STM32_TOOLBOX_DEVICES_FLASH_FLASHMEMORY_H_

#include <string.h>
#include "tinycrypt/tiny_md5.h"


/**
 * @brief	Abstract class for non-volatile memory.
 */
class FlashMemory
{
public:
	/**
	 * @brief	Initializes the memory by writing its directory.
	 * @returns	True on success; otherwise false.
	 */
	virtual bool initialize(void) = 0;


	/**
	 * @brief	Erases all memory managed by this object.
	 * @returns	True on success; otherwise false.
	 */
	virtual bool erase(void) = 0;


	/**
	 * @brief 	Writes the contents of RAM to FLASH memory.
	 * @param 	data A pointer to the data in FLASH.
	 * @param 	target A pointer to the destination memory.
	 * @param 	length The number of bytes to copy.
	 * @returns True on success; false on failure.
	 */
	virtual bool write(void* data, void* target, uint32_t length) = 0;


	/**
	 * @brief	Adds a file to the filesystem.
	 * @param 	filename The name of the file.
	 * @param 	data A pointer to the file data.
	 * @param 	length The length of the file.
	 */
	virtual bool add(const char *filename, uint8_t* data, uint32_t length) = 0;


	/**
	 * @brief	Flags are files in this memory with the specified filename as deleted.
	 * @param	entry The file to unlink.
	 * @note	This may not necessarily delete the file, but will flag it as deleted.
	 */
	virtual bool unlink(DirectoryEntry* entry) = 0;


	/**
	 * @brief	Gets the total capacity of this area.
	 * @returns	The capacity in bytes.
	 */
	virtual uint32_t get_capacity(void) = 0;


	/**
	 * @brief 	Unocks the FLASH controller.
	 */
	void unlock(void)
	{
#ifdef INC_FREERTOS_H
		osKernelLock();
#endif
		HAL_FLASH_Unlock();
		__disable_irq();
	}


	/**
	 * @brief 	Locks the FLASH controller.
	 */
	void lock(void)
	{
		HAL_FLASH_Lock();
#ifdef INC_FREERTOS_H
		osKernelUnlock();
#endif
		__enable_irq();
	}


	/**
	 * @brief	Gets the number of populated directory entries in the directory.
	 * @returns The number of files.
	 */
	uint32_t get_file_count(void)
	{
		for (int i=0; i < FLASH_DIRECTORY_SIZE; i++)
			if (directory->entries[i].magic_number != DirectoryEntry::FLASH_FILE_MAGIC_NUMBER)
				return i;
		return FLASH_DIRECTORY_SIZE;
	}


	/**
	 * @brief 	Searches the directory for a file with the specified name.
	 * @param 	filename The name of the file.
	 * @returns If found, a pointer to the matching `DirectoryEntry`; otherwise `nullptr`.
	 */
	DirectoryEntry* search(const char* filename)
	{
		for (uint32_t i=0; i < get_file_count(); i++)
			if (!strcmp(filename, directory->entries[i].filename))
				return &directory->entries[i];
		return nullptr;
	}


	/**
	 * @brief 	Verifies that the contents of FLASH match the contents in RAM.
	 * @param 	data A pointer to the data in FLASH.
	 * @param 	target A pointer to the destination memory.
	 * @param 	length The number of bytes to copy.
	 * @returns True on success; false on failure.
	 */
	bool verify(void* data, void* target, uint32_t length)
	{
		uint32_t* d = (uint32_t*)data;
		uint32_t* t = (uint32_t*)target;
	    uint8_t words = length / sizeof(uint32_t);
	    for (int i=0; i<words; i++)
			if (*(t + i) != *(d + i))
				return false;
	    return true;
	}


	/**
	 * @brief 	Copies a block of data from FLASH RAM to another location.
	 * @param 	data A pointer to the data in FLASH.
	 * @param 	target A pointer to the destination memory.
	 * @param 	length The number of bytes to copy.
	 * @returns True on success; false on failure.
	 */
	void read(void* data, void *target, uint32_t length)
	{
		uint32_t* d = (uint32_t*)data;
		uint32_t* t = (uint32_t*)target;
	    uint8_t words = length / sizeof(uint32_t);
	    for (int i=0; i<words; i++)
	    	*(d+i) = *(t+i);
	}


	/**
	 * @brief 	Determines whether the file system has been initialized in FLASH memory.
	 * @returns True if initialized; otherwise false.
	 */
	bool is_initialized(void)
	{
		return directory->header.magic_number == Directory::FLASH_DIRECTORY_MAGIC_NUMBER;
	}


	/**
	 * @brief 	Returns the amount of space used.
	 * @returns	The amount of space used.
	 */
	uint32_t get_usage(void)
	{
		uint32_t total = 0;
		for (uint32_t i=0; i<get_file_count(); i++)
			total += directory->entries[i].length;
		return total;
	}


	/**
	 * @brief 	Gets a pointer to the directory object.
	 * @returns The directory pointer.
	 */
	Directory* get_directory(void)
	{
		return directory;
	}


	/**
	 * @brief 	Returns the amount of free storage space.
	 * @returns	The amount of free space.
	 */
	uint32_t get_free(void)
	{
		return get_capacity() - get_usage();
	}

protected:
	/**
	 * @brief 	Gets the memory location at which the next file should be written.
	 * @returns A pointer to the address.
	 */
	uint8_t* get_next_file_address(void)
	{
		uint32_t len = get_file_count();
		if (len == 0) return (uint8_t*) directory + sizeof(Directory);
		DirectoryEntry *pos = &directory->entries[len-1];
		return pos->location + pos->length + (pos->length % sizeof(uint32_t));
	}


	/**
	 * @brief 	Writes the specified `DirectoryEntry` object to the directory.
	 * @param 	entry A pointer to a populated `DirectoryEntry` instance.
	 * @returns True on success; otherwise false.
	 */
	bool write_directory_entry(DirectoryEntry* entry)
	{
		DirectoryEntry* ptr = &directory->entries[get_file_count()];
		return write((uint32_t*)entry, (uint32_t*)ptr, sizeof(DirectoryEntry));
	}


	/**
	 * @brief 	Overwrites the specified `DirectoryEntry` object to the directory.
	 * @param 	entry A pointer to a populated `DirectoryEntry` instance.
	 * @param 	copy The structure to replace it with
	 * @returns True on success; otherwise false.
	 */
	bool write_directory_entry(DirectoryEntry* entry, DirectoryEntry* copy)
	{
		return write((uint32_t*)copy, (uint32_t*)entry, sizeof(DirectoryEntry));
	}


	/**
	 * @brief 	Checks whether there is already a file in the directory with the given filename.
	 * @param 	filename The filename to search for.
	 * @returns False if a duplicate file is detectedl otherwise True.
	 */
	bool validate_filename(const char* filename)
	{
		for (uint32_t i=0; i < get_file_count(); i++)
			if (!strcmp(filename, directory->entries[i].filename))
				return false;
		return true;
	}

protected:
	Directory* directory;
};

#endif /* INC_STM32_TOOLBOX_DEVICES_FLASH_FLASHMEMORY_H_ */
