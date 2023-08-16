///	@file       utility/Revision.h
///	@class      Revision
///	@brief      Determines the board revision based on a voltage divider.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef LIB_STM32_TOOLBOX_UTILITY_REVISION_H_
#define LIB_STM32_TOOLBOX_UTILITY_REVISION_H_


class Revision
{
public:
	/**
	 * Constructs this instance.
	 * @param	hadc Handle to the ADC device.
	 */
	Revision(ADC_HandleTypeDef* hadc)
	{
		this->hadc = hadc;
	}


	/**
	 * Gets the raw revision from the ADC, an integer between 0 and 4096.
	 * @returns The raw value.
	 */
	uint16_t get_raw(void)
	{
		// Prepare ADC for reading.
		HAL_ADCEx_Calibration_Start(hadc, ADC_SINGLE_ENDED);
		uint32_t factor = HAL_ADCEx_Calibration_GetValue(hadc, ADC_SINGLE_ENDED);
		HAL_ADCEx_Calibration_SetValue(hadc, ADC_SINGLE_ENDED, factor);
		HAL_ADC_Start(hadc);
		HAL_ADC_PollForConversion(hadc, 100);
		uint16_t value = HAL_ADC_GetValue(hadc);
		HAL_ADC_Stop(hadc);
		return value;
	}


	/**
	 * Gets the revision.
	 * @returns -1 if invalid, otherwise the zero-based revision (0=A, 1=B, etc.)
	 */
	int8_t get(void)
	{
		uint16_t value = get_raw();
		if (value < 75 || value > 3975)
			return -1;
		return (value + 75) / 150;
	}


	/**
	 * Gets the revision as a letter, where A=1, B=2, etc.
	 * @returns The letter revision.
	 */
	char get_letter(void)
	{
		return '@' + get();
	}

private:
	ADC_HandleTypeDef* hadc;
};

#endif /* LIB_STM32_TOOLBOX_UTILITY_REVISION_H_ */
