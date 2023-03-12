/*
 * OneTimeProgrammable.h
 *
 *  Created on: Oct. 13, 2022
 *      Author: YvanRodriguez
 */

#ifndef INC_STM32_TOOLBOX_DEVICES_FLASH_ONETIMEPROGRAMMABLE_H_
#define INC_STM32_TOOLBOX_DEVICES_FLASH_ONETIMEPROGRAMMABLE_H_

#include <stdint.h>


class OneTimeProgrammable
{
public:
	OneTimeProgrammable(uint32_t base_block)
	{
		this->base_block = base_block;
	}


	bool unlock(void)
	{
		clear_error_flags();
		return HAL_FLASH_Unlock() != HAL_OK;
	}


	bool lock(void)
	{
		return HAL_FLASH_Lock() != HAL_OK;
	}


	bool write(uint8_t index, void* data, uint8_t length)
	{
		if (length > block_size)  // Too big.
			return false;

		clear_error_flags();
		uint32_t target = base_block + index * block_size;
		for (int i=0; i< length; i++)
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, target+i, *(((uint8_t*)data)+i));
		return true;
	}

	void read(uint8_t index, void* data, uint8_t length)
	{
		uint32_t target = base_block + index * block_size;
		memcpy(data, (uint32_t*)target, length);
	}


	bool block(uint8_t index)
	{
		// Lock the OTP block by writing 0x00 to its lock location.
		uint32_t target = base_block + block_qty * block_size + index;
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, target, 0x00);

		return true;
	}


	bool is_blocked(uint8_t index)
	{
		uint32_t target = base_block + block_qty * block_size + index;
		uint8_t* flag = (uint8_t*) target;
		return *flag == 0;
	}


	bool verify(uint8_t index, void* data, uint8_t length)
	{
		if (length > block_size)  // Too big.
			return false;

		uint8_t* target = (uint8_t*)(base_block + index * block_size);
		for (int i=0; i< length; i++)
			if (*(target+i) != *(((uint8_t*)data)+i))
				return false;
		return true;
	}


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


private:
	uint32_t base_block;
	static constexpr uint8_t block_size = 0x20;
	static constexpr uint8_t block_qty = 0x10;
};


#endif /* INC_STM32_TOOLBOX_DEVICES_FLASH_ONETIMEPROGRAMMABLE_H_ */
