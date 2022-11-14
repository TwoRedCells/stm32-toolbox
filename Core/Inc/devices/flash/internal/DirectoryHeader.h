///	@file       devices/flash/DirectoryHeader.h
///	@struct     DirectoryHeader
///	@brief      The first value in a Directory, identifies that a Directory is in memory.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_STM32_TOOLBOX_DEVICES_FLASH_DIRECTORYHEADER_H_
#define INC_STM32_TOOLBOX_DEVICES_FLASH_DIRECTORYHEADER_H_


/**
 * @brief	The first value in a Directory, identifies that a Directory is in memory.
 */
typedef struct
{
	uint32_t magic_number;
	uint32_t padding[3];
} DirectoryHeader;

#endif /* INC_STM32_TOOLBOX_DEVICES_FLASH_DIRECTORYHEADER_H_ */
