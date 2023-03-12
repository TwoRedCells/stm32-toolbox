///	@file       diagnostics/Fault.h
///	@class      Fault
///	@brief      A reporting mechanism for hardware and software faults, with digital indication output
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_DIAGNOSTICS_FAULT_H_
#define INC_DIAGNOSTICS_FAULT_H_

#include "stm32/stm32.h"
#if FAULT_ENABLE_LED_SUPPORT
#include "stm32-toolbox/devices/Led.h"
#endif

/// <summary>
/// A reporting mechanism for hardware and software faults, with digital indication output
/// </summary>
/// <remarks>
/// Fault is an abstract class and should be subclassed to be use. When subsclassing, up to 32 individual faults can be
/// defined. The class should then be (generally) instantiated within global scope. Throughout your code, faults can be
/// set or cleared, and the current fault state can be checked at any time. Optionally, a FAULT LED can be
/// automatically illuminated when a fault is present.
/// </remarks>
class Fault
{
public:
	// Subclass this class and add your own faults.
	static constexpr uint64_t None									= 0x0000000000000000;
	static constexpr uint64_t DhcpUnavailable						= 0x0000000100000000;
	static constexpr uint64_t HardFault								= 0x0000000200000000;
	static constexpr uint64_t TaskAllocation						= 0x0000000400000000;
	static constexpr uint64_t QueueAllocation						= 0x0000000800000000;
	static constexpr uint64_t StackOverflow							= 0x0000001000000000;

	// When these occur, code should not attempt to recover.
	static constexpr uint32_t fatal_faults = TaskAllocation | StackOverflow;

#if FAULT_ENABLE_LED_SUPPORT
	/// <summary>
	/// If FAULT LED feature is enabled, points the class to an instance of the <see cref="Led">Led</see> class that should
	/// be illuminated when a fault is present.
	/// </summary>
	/// <param name="led">Led instance</param>
	void set_led(Led* led)
	{
		this->led = led;
		led->off();
	}
#endif

	/// <summary>
	/// Raises a fault.
	/// </summary>
	/// <param name="fault">The fault to raise.</param>
	void raise(uint64_t fault)
	{
		this->fault |= fault;
		update_fault_led();
	}


	/// <summary>
	/// Clears a raised fault.
	/// </summary>
	/// <remarks>There is no effect if the specified fault is not raised.</remarks>
	/// <param name="fault">The fault to clear.</param>
	void clear(uint64_t fault)
	{
		this->fault = this->fault & (0xffffffff ^ fault);
		update_fault_led();
	}


	/// <summary>
	/// Raises or clears a fault, based on the specified state.
	/// </summary>
	/// <param name="fault">The fault to alter.</param>
	/// <param name="state">The new state (true or raise; false to clear).</param>
	void update(uint64_t fault, bool state)
	{
		if (state) raise (fault);
		else clear (fault);
	}


	/// <summary>
	/// Gets the current fault state.
	/// </summary>
	/// <returns>The bitfield representing the current fault state, or zero if no faults are present.</returns>
	uint32_t get(void)
	{
		return this->fault;
	}


	/// <summary>
	/// Determines whether the specified fault is currently present.
	/// </summary>
	/// <returns>True if the fault is present; otherwise false.</returns>
	bool is_present(uint64_t fault)
	{
		return this->fault & fault;
	}


protected:
	uint64_t fault = 0;
#if FAULT_ENABLE_LED_SUPPORT
	Led* led;
#endif

	/// <summary>
	/// Illuminates the fault LED if there is an outstanding fault.
	/// </summary>
	void update_fault_led()
	{
		if (fault & fatal_faults)
			led->flash();
		else
			led->set(this->fault > 0);
	}
};

#endif /* INC_DIAGNOSTICS_FAULT_H_ */
