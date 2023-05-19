///	@file       devices/Rtc.h
///	@class      Rtc
///	@brief      Sets and gets the value of the hardware real-time clock.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_STM32_TOOLBOX_DEVICES_RTC_H_
#define INC_STM32_TOOLBOX_DEVICES_RTC_H_


class Rtc
{
public:
	Rtc(RTC_HandleTypeDef* hrtc) : hrtc_(hrtc) {}


	/**
	 * Gets the RTC value as a UNIX timestamp.
	 * @returns	The RTC value.
	 */
	uint32_t get_time()
	{
		RTC_TimeTypeDef rtc_time;
		RTC_DateTypeDef rtc_date;
		uint32_t epoch_time;

		// Get RTC time and date
		HAL_RTC_GetTime(hrtc_, &rtc_time, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(hrtc_, &rtc_date, RTC_FORMAT_BIN);

		// Convert RTC time and date to UNIX epoch timestamp
		epoch_time = (rtc_date.Year + 2000 - 1970) * 31536000; // Years to seconds
		for (int i = 1970; i < rtc_date.Year + 2000; i++) {
			if (is_leap_year(i)) {
				epoch_time += 86400; // Add leap day
			}
		}
		epoch_time += (rtc_date.Date - 1) * 86400; // Days to seconds
		epoch_time += rtc_time.Hours * 3600; // Hours to seconds
		epoch_time += rtc_time.Minutes * 60; // Minutes to seconds
		epoch_time += rtc_time.Seconds; // Seconds

		return epoch_time;
	}


	/**
	 * Sets the RTC to the specified value.
	 * @param	time The time as a UNIX timestamp.
	 */
	void set_time(uint32_t time)
	{
		RTC_TimeTypeDef rtc_time;
		RTC_DateTypeDef rtc_date;

		// Convert UNIX epoch timestamp to RTC time and date
		uint32_t remaining_time = time;
		rtc_time.Seconds = remaining_time % 60;
		remaining_time /= 60;
		rtc_time.Minutes = remaining_time % 60;
		remaining_time /= 60;
		rtc_time.Hours = remaining_time % 24;
		remaining_time /= 24;
		rtc_date.Date = remaining_time % 31 + 1;
		remaining_time /= 31;
		rtc_date.Month = remaining_time % 12 + 1;
		remaining_time /= 12;
		rtc_date.Year = remaining_time - 30;

		// Set RTC time and date
		HAL_RTC_SetTime(hrtc_, &rtc_time, RTC_FORMAT_BIN);
		HAL_RTC_SetDate(hrtc_, &rtc_date, RTC_FORMAT_BIN);
	}

private:
	RTC_HandleTypeDef* hrtc_;

	/**
	 * Determines whether the sepcified year is a leap year.
	 * @param	year The year.
	 * @returns	True if a leap year; otherwise false.
	 */
	bool is_leap_year(int year)
	{
		if (year % 4 != 0)
			return false;
		else if (year % 100 != 0)
			return true;
		else if (year % 400 != 0)
			return false;
		else
			return true;
	}
};




#endif /* INC_STM32_TOOLBOX_DEVICES_RTC_H_ */
