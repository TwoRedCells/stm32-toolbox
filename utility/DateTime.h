/*
 * DateTime.h
 *
 *  Created on: Jan 17, 2024
 *      Author: YvanRodriguez
 */

#ifndef LIB_STM32_TOOLBOX_UTILITY_DATETIME_H_
#define LIB_STM32_TOOLBOX_UTILITY_DATETIME_H_

#include <stdint.h>
#include "PrintLite.h"

struct DateTime
{
public:

	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	double partial;

	enum DaysOfWeek { Sunday, Monday, Tuesday, Wednesday, Thursday, Friday };

	/**
	 * @brief	Instantiates a DateTime instance using a UNIX timestamp.
	 * @param timestamp The timestamp.
	 */
	DateTime(int32_t timestamp)
	{
		uint32_t remaining = timestamp;
		second = remaining % 60;
		remaining /= 60;
		minute = remaining % 60;
		remaining /= 60;
		hour = remaining % 24;
		remaining /= 24;

		for (year = 1970; remaining > 365; year++)
			remaining -= (IsLeapYear(year) ? 366 : 365);

		for (month = 0; remaining > months[month]; month++)
			remaining -= months[month];

		day = remaining-1;
	}


	DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour=0, uint8_t minute=0, uint8_t second=0, double partial=0)
	{
		this->year = year;
		this->month = month;
		this->day = day;
		this->hour = hour;
		this->minute = minute;
		this->second = second;
		this->partial = partial;
	}

	/**
	 * @brief	Instantiates a DateTime instance using date and time components..
	 * @param year The year.
	 * @param month	The zero-based month.
	 * @param day The zero-based day.
	 * @param hour The zero-based hour.
	 * @param minute The zero-based minutes.
	 * @param second The zero-based second.
	 */

	static int32_t ToTimestamp(uint16_t year, uint8_t month, uint8_t day, uint8_t hour=0, uint8_t minute=0, uint8_t second=0)
	{
		uint32_t timestamp = (year - 1970) * 31536000; // Years to seconds
		for (int i = 1970; i < year; i++)
			if (IsLeapYear(i))
				timestamp += 86400; // Add leap day

		for (int i=0; i<month; i++)
			timestamp += months[i] * 86400;
		timestamp += day * 86400; // Days to seconds
		timestamp += hour * 3600; // Hours to seconds
		timestamp += minute * 60; // Minutes to seconds
		timestamp += second; // Seconds
		return timestamp;
	}

	int32_t ToTimestamp(void)
	{
		return ToTimestamp(year, month, day, hour, minute, second);
	}

	bool operator< (DateTime value)
	{
		return this->ToTimestamp() < value.ToTimestamp();
	}

	bool operator> (DateTime value)
	{
		return this->ToTimestamp() > value.ToTimestamp();
	}

	bool operator== (DateTime value)
	{
		return this->ToTimestamp() == value.ToTimestamp();
	}

	bool operator!= (DateTime value)
	{
		return this->ToTimestamp() != value.ToTimestamp();
	}

	DateTime AddSeconds(int value)
	{
		int32_t timestamp = this->ToTimestamp();
		return DateTime(timestamp + 1);
	}

	DateTime AddMinutes(int value)
	{
		int32_t timestamp = this->ToTimestamp();
		return DateTime(timestamp + 60);
	}

	DateTime AddHours(int value)
	{
		int32_t timestamp = this->ToTimestamp();
		return DateTime(timestamp + 60*60);
	}

	DateTime AddDays(int value)
	{
		int32_t timestamp = this->ToTimestamp();
		return DateTime(timestamp + 60*60*24);
	}

	const char* ToDateString(void)
	{
		PrintLite::vsprintf(buf, "%u-%02u-%02u", year, month, day);
		return buf;
	}

	const char* ToTimeString(void)
	{
		PrintLite::vsprintf(buf, "%02u:%02u:%02u", hour, minute, second);
		return buf;
	}

	const char* ToISOString(bool zulu=false)
	{
		PrintLite::vsprintf(buf, "%u-%02u-%02uT%02u:%02u:%02u%s", year, month+1, day+1, hour, minute, second, zulu?"Z":"");
		return buf;
	}

	const char* ToRawISOString(bool zulu=false)
	{
		PrintLite::vsprintf(buf, "%u%02u%02uT%02u%02u%02u%s", year, month+1, day+1, hour, minute, second, zulu?"Z":"");
		return buf;
	}

	DaysOfWeek GetDayOfWeek(void)
	{
		return (DaysOfWeek)((ToTimestamp() / 86400 + 4) % 7);
	}

	/**
	 * Determines whether the sepcified year is a leap year.
	 * @param	year The year.
	 * @returns	True if a leap year; otherwise false.
	 */
	static bool IsLeapYear(int year)
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

private:
	static const constexpr uint8_t months[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	char buf[25];
};



#endif /* LIB_STM32_TOOLBOX_UTILITY_DATETIME_H_ */
