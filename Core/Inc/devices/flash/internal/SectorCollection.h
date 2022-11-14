///	@file       devices/flash/SectorCollection.h
///	@class      SectorCollection
///	@brief      Describes a collection of one or more Sectors.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_STM32_TOOLBOX_DEVICES_FLASH_SECTORCOLLECTION_H_
#define INC_STM32_TOOLBOX_DEVICES_FLASH_SECTORCOLLECTION_H_

#include "Sector.h"


/**
 * @brief	Describes a collection of one or more Sectors.
 */
class SectorCollection
{
public:
	uint8_t count;	/// Number of sectors in the collection.
	Sector *sectors;  /// Pointer to sectors.
};

#endif /* INC_STM32_TOOLBOX_DEVICES_FLASH_SECTORCOLLECTION_H_ */
