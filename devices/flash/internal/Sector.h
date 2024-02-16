///	@file       devices/flash/Sector.h
///	@class      Sector
///	@brief      A sector is the smallest unit of non-volatile memory that can be erased at once. It can hold multiple files.
/// @note		An MCU will support Page or Sector flash, but not both. Check your datasheet.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_STM32_TOOLBOX_DEVICES_FLASH_SECTOR_H_
#define INC_STM32_TOOLBOX_DEVICES_FLASH_SECTOR_H_

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
	}


	/**
	 * @brief Erases the entire sector of FLASH memory.
	 * @returns True on success; false on failure.
	 * @note Any FLASH that has not been written to after an erase operation can be written to.
	 */
	bool erase(void) override
	{
		bool result = true;
		FLASH_EraseInitTypeDef erase_struct = {
				.TypeErase   = FLASH_TYPEERASE_SECTORS,
				.Sector = index,
				.NbSectors = 1,
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
