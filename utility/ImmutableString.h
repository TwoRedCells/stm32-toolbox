/**
 * @file 		utility/ImmutableString.h
 * @class		ImmutableString
 * @author 		Yvan Rodrigues <yvan.r@radskunkworks.com>
 * @brief 		Convenience wrapper for immutable strings.
 * @date 		2024-02-15
 * @copyright 	Copyright (c) 2024 Robotic Assistance Devices
 */

#ifndef UTILITY_IMMUTABLESTRING_H
#define UTILITY_IMMUTABLESTRING_H

#include <string.h>
#include <stdlib.h>


class ImmutableString
{
public:
	ImmutableString(const char* s)
	{
		this->s = s;
		l = strlen(s);
	}

	ImmutableString& operator=(const char* s)
	{
		this->s = s;
		l = strlen(s);
		return *this;
	}

	const char* operator=(ImmutableString string)
	{
		return string.s;
	}


	bool operator==(ImmutableString o)
	{
		return !strcmp(s, o.s);
	}

	bool operator!=(ImmutableString o)
	{
		return strcmp(s, o.s);
	}

	char operator[](size_t p)
	{
		if (l == 0 || p > l-1)
			return 0;
		return s[p];
	}

	explicit operator const char*() const {
		return s;
	}

	size_t length(void)
	{
		return l;
	}

	const char* raw(void)
	{
		return s;
	}

	bool is_empty(void)
	{
		return l == 0;
	}

	float ToFloat(void)
	{
		return atof(s);
	}

	double ToDouble(void)
	{
		return atof(s);
	}

	uint8_t ToUInt8(void)
	{
		return (uint8_t) atoi(s);
	}

	uint16_t ToUInt16(void)
	{
		return (uint16_t) atoi(s);
	}

	uint32_t ToUInt32(void)
	{
		return (uint32_t) atoi(s);
	}

	int8_t ToInt8(void)
	{
		return (int8_t) atoi(s);
	}

	int16_t ToInt16(void)
	{
		return (int16_t) atoi(s);
	}

	int32_t ToInt32(void)
	{
		return (int32_t) atoi(s);
	}

	ImmutableString Token(const char* token)
	{
		this->token = token;
		return strtok((char*)s, token);
	}

	ImmutableString Token(void)
	{
		return strtok(nullptr, token);
	}

private:
	const char* s;
	const char* token;
	size_t l;
};

#ifndef _S
#define _S(x) ImmutableString(x)
#endif

#endif
