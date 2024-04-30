///	@file       utility/Range.h
///	@class      Range
///	@brief      Expresses an immutable range of integers
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE



#ifndef LIB_STM32_TOOLBOX_UTILITY_RANGE_H_
#define LIB_STM32_TOOLBOX_UTILITY_RANGE_H_

template <class T>
class Range
{
public:
	Range(T first, T last)
	{
		assert(last >= first);
		this->f = first;
		this->l = last;
	}

	T first(void)
	{
		return f;
	}

	T last(void)
	{
		return l;
	}

	T difference(void)
	{
		return l - f;
	}

	bool in(T value)
	{
		return value >= f && value <= l;
	}

private:
	T f, l;
};



#endif /* LIB_STM32_TOOLBOX_UTILITY_RANGE_H_ */
