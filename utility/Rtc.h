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
		DateTime now = get_datetime();
		return now.ToTimestamp();
	}


	/**
	 * Gets the RTC value as a UNIX timestamp.
	 * @returns	The RTC value.
	 */
	DateTime get_datetime()
	{
		RTC_TimeTypeDef rtc_time;
		RTC_DateTypeDef rtc_date;
		HAL_RTC_GetTime(hrtc_, &rtc_time, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(hrtc_, &rtc_date, RTC_FORMAT_BIN);
		double partial = ((double)(rtc_time.SecondFraction - rtc_time.SubSeconds)) / ((double)(rtc_time.SecondFraction+1));
		DateTime now(rtc_date.Year+2000, rtc_date.Month-1, rtc_date.Date-1, rtc_time.Hours, rtc_time.Minutes, rtc_time.Seconds, partial);
		return now;
	}


	/**
	 * Sets the RTC to the specified value.
	 * @param	time The time as a UNIX timestamp.
	 */
	void set_time(int32_t time)
	{
		DateTime now = time;
		set_time(now);
	}

	void set_time(DateTime time)
	{
		RTC_TimeTypeDef rtc_time = {
			.Hours = time.hour,
					.Minutes = time.minute,
					.Seconds = time.second
		};
		int dow = time.GetDayOfWeek();
		RTC_DateTypeDef rtc_date = {
					.WeekDay = (uint8_t) (dow == 0 ? 7 : dow+1),
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
