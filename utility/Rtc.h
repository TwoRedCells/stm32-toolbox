///	@file       devices/Rtc.h
///	@class      Rtc
///	@brief      Sets and gets the value of the hardware real-time clock.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#include "utility/DateTime.h"

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
	int32_t get_time()
	{
		RTC_TimeTypeDef rtc_time;
		RTC_DateTypeDef rtc_date;
		HAL_RTC_GetTime(hrtc_, &rtc_time, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(hrtc_, &rtc_date, RTC_FORMAT_BIN);
		DateTime now(rtc_date.Year+2000, rtc_date.Month-1, rtc_date.Date-1, rtc_time.Hours, rtc_time.Minutes, rtc_time.Seconds);
		return now.ToTimestamp();
	}


	/**
	 * Sets the RTC to the specified value.
	 * @param	time The time as a UNIX timestamp.
	 */
	void set_time(int32_t time)
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

		HAL_RTC_SetTime(hrtc_, &rtc_time, RTC_FORMAT_BIN);
		HAL_RTC_SetDate(hrtc_, &rtc_date, RTC_FORMAT_BIN);
	}

	void set_time(DateTime time)
	{
		RTC_TimeTypeDef rtc_time = {
			.Hours = time.hour,
					.Minutes = time.minute,
					.Seconds = time.second
		};
		RTC_DateTypeDef rtc_date = {
					.WeekDay = time.GetDayOfWeek(),
					.Month = (uint8_t) (time.month + 1),
					.Date = (uint8_t) (time.day + 1),
					.Year = (uint8_t) (time.year - 2000)
		};
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
