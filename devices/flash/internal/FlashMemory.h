///	@file       devices/flash/FlashMemory.h
///	@class      FlashMemory
///	@brief      Abstract class for non-volatile memory.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_STM32_TOOLBOX_DEVICES_FLASH_FLASHMEMORY_H_
#define INC_STM32_TOOLBOX_DEVICES_FLASH_FLASHMEMORY_H_



/**
 * @brief	Abstract class for non-volatile memory.
 */
class FlashMemory
{
public:
//	/**
//	 * @brief	Initializes the memory by writing its directory.
//	 * @returns	True on success; otherwise false.
//	 */
//	virtual bool initialize(void) = 0;


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
	 * @brief	Gets the total capacity of this area.
	 * @returns	The capacity in bytes.
	 */
	virtual uint32_t get_capacity(void) = 0;


	/**
	 * @brief 	Unocks the FLASH controller.
	 */
	void unlock(void)
	{
#ifdef USING_FREERTOS
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
#ifdef USING_FREERTOS
		osKernelUnlock();
#endif
		__enable_irq();
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
};

#endif /* INC_STM32_TOOLBOX_DEVICES_FLASH_FLASHMEMORY_H_ */
