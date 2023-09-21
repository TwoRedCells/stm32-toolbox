///	@file       diagnostics/Watchdog.h
///	@class      Watchdog
///	@brief      Implements independent hardware watchdog (IHWD) abstraction.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_UTILITY_WATCHDOG_H_
#define INC_UTILITY_WATCHDOG_H_

#include "diagnostics/Fault.h"

/// <summary>
/// Implements independent hardware watchdog (IHWD) abstraction.
/// </summary>
/// <remarks>
/// The independent hardware watchdog is used to reset your microcontroller when the code that it is executing fails
/// to respond normally. This class provides a simple interface to implement this workflow, particularly on RTOS
/// designs with multiple tasks.
///
/// In order to implement the watchdog, first define a bitfielded set of up to 32 things that must occur in order to
/// deem the firmware as correctly operational, typically one per execution loop. For example an application with a
/// an LED task and a fan task, we would define:
/// <code>
/// #define LedTaskHealthy 0x01
/// #define FanTaskHealthy 0x02
/// </code>
/// The watchdog will reset the MCU if both of these (LedTaskHealthy|FanTaskHealthy) do not check in within the
/// specified grace period.
/// </remarks>
class Watchdog
{
public:

	/// <summary>
	/// Configures the watchdog.
	/// </summary>
	/// <param name="expected_flags">The ORed valuethat is expected during healthy operation.</param>
	/// <param name="prescaler">The prescalar constant to use for the watchdog timer e.g. IWDG_PRESCALER_128</param>
	void setup(uint32_t expected_flags, uint32_t prescaler=IWDG_PRESCALER_128)
	{
		expected = expected_flags;
		hiwdg.Init.Prescaler = prescaler;
	}


	/// <summary>
	/// Starts the watchdog.
	/// </summary>
	void start(void)
	{
		hiwdg.Instance = IWDG;
#ifdef MCE_STM32L4
		hiwdg.Init.Window = 4095;
#endif
		hiwdg.Init.Reload = 4095;
		hiwdg.Init.Window = 0xfff;  // Allow taunt() to be called as often as we like.
		HAL_IWDG_Init(&hiwdg);
	}


	/// <summary>
	/// Executed within an execution loop to inform the watchdog that it is responsive.
	/// </summary>
	/// <param name="flag">The flag of the task that is checking in.</param>
	void feed(uint32_t flag)
	{
		flags |= flag;
	}


	/// <summary>
	/// Refreshes the watchdog only if all tasks have checked in.
	/// </summary>
	/// <remarks>Must be called periodically to prevent activation of the watchdog.</remarks>
	void taunt(void)
	{
		if (flags == expected)
		{
			HAL_IWDG_Refresh(&hiwdg);
			flags = 0;
		}
	}


	/// <summary>
	/// Satisfies the watchdog without checking flags.
	/// </summary>
	/// <remarks>Can be used to prevent the watchdog from triggering during necessary tight loops.</remarks>
	void ignore(void)
	{
		HAL_IWDG_Refresh(&hiwdg);
	}

private:
	IWDG_HandleTypeDef hiwdg;
	volatile uint32_t flags = 0;
	uint32_t expected = 0;
	bool paused = false;
};


#endif /* INC_UTILITY_WATCHDOG_H_ */
