/**
 * \file       devices/FlashMemory.h
 * \class      FlashMemory
 * \brief      Facilitates reading/writing with flash memory.
 */

#include <stdint.h>
#include "globals.hpp"

#ifndef INC_DEVICES_FLASHMEMORY_H_
#define INC_DEVICES_FLASHMEMORY_H_

#define ADDR_FLASH_PAGE    (0x0800fc00)   // See linker definition file.

class FlashMemory
{
public:
	void write(uint32_t* data, uint32_t length)
	{
	    uint32_t PAGEError = 0;

	    // Unlock.
	    HAL_FLASH_Unlock();

	    // Erase.
	    FLASH_EraseInitTypeDef EraseInitStruct;
	    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	    EraseInitStruct.PageAddress = ADDR_FLASH_PAGE;
	    EraseInitStruct.NbPages     = 1;

	    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	    {
	        HAL_FLASH_Lock();
	        fault.alert(Fault::FlashMemory);
	        return;
	    }

	    // Write.
	    uint8_t words = length / 4;
	    for (int i=0; i<words; i++)
	    {
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (uint32_t)FLASH_DATA+i, *(data+i)) != HAL_OK)
			{
				HAL_FLASH_Lock();
				fault.alert(Fault::FlashMemory);
				return;
			}
	    }

	    // Lock.
	    HAL_FLASH_Lock();

	    // Verify.
	    for (int i=0; i<words; i++)
	    {
			if (*(__IO uint32_t *)(FLASH_DATA+i) != *(data+i))
			{
				fault.alert(Fault::FlashMemory);
				return;
			}
	    }
		fault.reset(Fault::FlashMemory);
	}


	void read(uint32_t* data, uint32_t length)
	{
	    uint8_t words = length / 4;
	    for (int i=0; i<words; i++)
	    {
	    	*(data+i) = *(__IO uint32_t *)(FLASH_DATA+i);
	    }
	}

private:
	uint32_t *FLASH_DATA = (uint32_t*)ADDR_FLASH_PAGE;
};

#endif /* INC_DEVICES_FLASHMEMORY_H_ */
