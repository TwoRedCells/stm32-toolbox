///	@file       diagnostics/Revision.h
///	@class      Revision
///	@brief      Determines the PCB revision based on a resistor divider connected to an ADC pin.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef LIB_STM32_TOOLBOX_UTILITY_REVISION_H_
#define LIB_STM32_TOOLBOX_UTILITY_REVISION_H_

#include <stdint.h>

class Revision
{
	// Each value approximates the expected ADC value that would correspond with a revision.
	static constexpr uint16_t ranges[27] = {
			141, 269, 397, 525, 652, 781, 906, 1034, 1161, 1289, 1416,
			1544, 1672, 1799, 1927, 2054, 2181, 2308, 2436, 2563, 2691,
			2818, 2946, 3073, 3201, 3328, 3455
	};

public:
	/**
	 * Gets the revision.
	 * @param hadc A pointer to the ADC handle.
	 * @returns The revision, or -1 if unavailable.
	 */
	static int16_t get(ADC_HandleTypeDef* hadc)
	{
		HAL_ADC_Start(hadc);
		HAL_ADC_PollForConversion(hadc, 1000);
		uint32_t value = HAL_ADC_GetValue(hadc);
		HAL_ADC_Stop(hadc);

		for (uint8_t i=0; i<sizeof(ranges); i++)
			if (value > ranges[i]-75 && value < ranges[i]+75)
				return i;
	}
};

#endif /* LIB_STM32_TOOLBOX_UTILITY_REVISION_H_ */
