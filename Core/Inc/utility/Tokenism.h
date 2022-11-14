/**
 * @file		utility/Tokenism.h
 * @class		Tokenism
 * @brief		Easily parse tokens from strings using delimiters.
 * @note		This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
 * 				firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
 * @copyright	 See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE
 */

#ifndef INC_STM32_TOOLBOX_UTILITY_TOKENISM_H_
#define INC_STM32_TOOLBOX_UTILITY_TOKENISM_H_

#include <memory.h>

class Tokenism
{
public:
	/**
	 * @brief Creates an instance of the `Tokenism` class.
	 * @param value The string to tokenize.
	 * @param delimiters The delimiters.
	 */
	Tokenism(char* value, char* delimiters)
	{
		this->original = value;
		this->delimiters = delimiters;
		this->copy = (char*) malloc(strlen(value));
	}


	/**
	 * @brief Creates an instance of the `Tokenism` class.
	 * @param value The string to tokenize.
	 * @param delimiters The delimiters.
	 */
	Tokenism(const char* value, char* delimiters)
	{
		this->original = (char*) value;
		this->delimiters = delimiters;
		this->copy = (char*) malloc(strlen(value));
	}

	~Tokenism()
	{
		free(this->copy);
	}


	/**
	 * @brief Gets the Nth token found using the specified delimiter(s).
	 * @param n The index of the token to find.
	 * @returns The token if found, otherwise nullptr.
	 */
	char* at(uint8_t n)
	{
		strcpy(copy, original);
		char* token;
		token = strtok(copy, delimiters);

		for (uint8_t i=0; i<n; i++)
			token = strtok(nullptr, delimiters);
		return token;
	}


	/**
	 * @brief Checks whether the Nth token found matches the specified string.
	 * @param n The index of the token to find.
	 * @param match The string to match.
	 * @returns true if the token matches the string; otherwise false.
	 */
	bool at_equals(uint8_t n, char* match)
	{
		char* value = at(n);
		if (value == nullptr) return false;
		return !strcmp(value, match);
	}


	/**
	 * @brief Gets the token following the specified token.
	 * @param needle The string to search for.
	 * @param occurrance The zero-based index of which occurrence of the token to select. 1 is the second occurrance (default is 0).
	 * @returns The token if found, otherwise nullptr.
	 */
	char* after(const char* needle, uint8_t occurrance=0)
	{
		strcpy(copy, original);
		char* token;
		token = strtok(copy, delimiters);
		for (;;)
		{
			if (token == nullptr)
				return nullptr;
			if (!strcmp(token, needle))
			{
				if (occurrance-- == 0)
					return strtok(nullptr, delimiters);
			}
			token = strtok(nullptr, delimiters);
		}
	}


	/**
	 * @brief Checks if the value contains the specified token.
	 * @param needle The string to search for.
	 * @returns True if the token is found, otherwise false;
	 */
	bool contains(const char* needle)
	{
		strcpy(copy, original);
		char* token;
		token = strtok(copy, delimiters);
		for (;;)
		{
			if (token == nullptr)
				return false;
			if (!strcmp(token, needle))
				return true;
			token = strtok(nullptr, delimiters);
		}
	}

private:
	char* copy;
	char* original;
	char* delimiters;
};

#endif /* INC_STM32_TOOLBOX_UTILITY_TOKENISM_H_ */
