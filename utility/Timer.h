///	@file       utility/Timer.h
///	@class      Timer
///	@brief      A general purpose timer that can be used as a stopwatch or a means to coordinate asynchonous events.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE

#ifndef TIMER_H
#define TIMER_H

#define microseconds(x) (x*1)
#define milliseconds(x) (x*1000)
#define seconds(x) (x*1000000)
#define millis() (Timer::now()/1000)

#include "toolbox.h"

// osDelay-compatible function for when FreeRTOS is not being used.
#ifndef USING_FREERTOS
#define osDelay(x) Timer::Block(milliseconds(x))
#endif


/// <summary>
/// A general purpose timer that can be used as a stopwatch or a means to coordinate asynchonous events.
/// </summary>
/// <remarks>
/// The internals for this class use the DWT (data watchpoint and trace) timer supported in the kernel of most Cortex-M
/// microcontrollers. This timer ticks approximately once per microsecond, however each tick is only as accurate as the
/// resolution of the main clock speed of the processor. For example, a 72MHz processor will have an error of up to
/// 14 nanoseconds. Any critical timing should use a dedicated hardware timer tuned for its application.
///
/// Since the DWT will overflow every 2^32/cpu_frequency seconds (e.g. 52 seconds at 80 MHz.), the class maintains an
/// accumulator to allow the timer to run to a maximum of 2^32 microseconds, or about 71.5 minutes.
/// </remarks>
class Timer
{
public:
	/// <summary>
	/// Constructs an instance of the Timer class.
	/// </summary>
	Timer()
	{
		if (!initialized)
		{
			// Toggle TRC.
			CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
			CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
			// Toggle clock cycle counter.
			DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
			DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
			initialized = true;
		}
	}

	/// <summary>
	/// Constructs an instance of the Timer class.
	/// </summary>
	/// <param name="duration">The duration after which the timer is elapsed.</param>
	/// <remarks>The timer is typically used in this mode when timing the duration of a process.</remarks>
	/// <returns>True if the timer is running; otherwise false.</returns>
	Timer(uint32_t duration)
	{
		this->duration = duration;
		Timer();
	}


	/// <summary>
	/// Starts a timer without a predetermined interval.
	/// </summary>
	/// <remarks>The timer is typically used in this mode when timing the duration of a process.</remarks>
	/// <returns>True if the timer is running; otherwise false.</returns>
	void start(void)
	{
		started = now();
		alarm = started + duration;
	}


	/// <summary>
	/// Starts the timer with the given duration.
	/// </summary>
	/// <param name="duration">The duration after which the timer is elapsed.</param>
	void start(uint32_t duration)
	{
		this->duration = duration;
		start();
	}


	/// <summary>
	/// Sets the duration of the timer, without starting it.
	/// </summary>
	/// <param name="duration">The duration after which the timer is elapsed.</param>
	void set(uint32_t  duration)
	{
		this->duration = duration;
	}


	/// <summary>
	/// Restarts the timer with the same duration.
	/// </summary>
	void restart(void)
	{
		start(duration);
	}


	/// <summary>
	/// Resets the timer to a clean state. Does not restart the timer.
	/// </summary>
	void reset(void)
	{
		started = 0;
		alarm = 0;
	}


	/// <summary>
	/// Determines whether the timer has elapsed (reached its set duration).
	/// </summary>
	/// <returns>True if the timer has elapsed; otherwise false.</returns>
	bool is_elapsed(void)
	{
		return alarm != 0 && (now() >= alarm || now() < started);
	}


	/// <summary>
	/// Determines whether the timer is currently running.
	/// </summary>
	/// <remarks>The timer is considered running until it is stopped, even if it has elapsed.</remarks>
	/// <returns>True if the timer is running; otherwise false.</returns>
	bool is_started(void)
	{
		return started != 0;
	}


	/// <summary>
	/// Determines the elapsed time since the timer was started.
	/// </summary>
	/// <returns>True if the timer is running; otherwise false.</returns>
	uint32_t elapsed(void)
	{
		return now() - started;
	}


	/// <summary>
	/// Blocks the application until the specified interval has elapsed.
	/// </summary>
	/// <param name="duration">The time to block.</param>
	void block(uint32_t duration)
	{
		start(duration);
		while (!is_elapsed())
			asm("nop");
		reset();
	}


	/// <summary>
	/// Blocks the application until the specified interval has elapsed.
	/// </summary>
	/// <param name="duration">The time to block.</param>
	static void Block(uint32_t duration)
	{
		Timer t;
		t.block(duration);
	}


	/// <summary>
	/// Gets the current internal timestamp.
	/// </summary>
	/// <remarks>The value overflows every 2^32 microseconds or about 71 minutes.</remarks>
	/// <returns>The current timestamp.</returns>
	static uint32_t now(void)
	{
		static uint32_t last = 0;
		static uint32_t accumulator = 0;

		// n is the value of the DWT timer in microseconds.
		uint32_t n = DWT->CYCCNT / (HAL_RCC_GetHCLKFreq() / 1000000);
		// When DWT reaches 2^32 (every 30-60 seconds depending on clock speed), it will roll over to zero.
		// When that happens, we add 2^32 ticks (converted to microseconds) to the accumulator,
		// allowing us to return the value as a 32-bit integer
		if (n < last)
			accumulator += 0xffffffff / (HAL_RCC_GetHCLKFreq() / 1000000) + 1;
		last = n;
		// Now we can return the number of microseconds since the system started (until 2^32 microseconds).
		return n + accumulator;
	}

private:
	uint32_t started = 0;  /// The time when the timer was started.
	uint32_t alarm = 0;  /// The time when the alarm should be invoked.
	uint32_t duration = 0;  /// The duration of the timer.
	inline static bool initialized;  /// Whether or not the timer has been initialized.
};

#endif
