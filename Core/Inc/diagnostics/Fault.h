/**
 * \file       diagnostics/Fault.h
 * \class      Fault
 * \brief      A reporting mechanism for hardware and software faults, with digital indication output.
 */

#ifndef INC_DIAGNOSTICS_FAULT_H_
#define INC_DIAGNOSTICS_FAULT_H_

#include <stdint.h>
#include "hal/hal.h"
#include "devices/Led.h"

class Fault
{
public:
	static constexpr uint32_t None									= 0x00000000;
	static constexpr uint32_t BatteryCommunications 				= 0x00000001;
	static constexpr uint32_t ContactorFeedback 					= 0x00000002;
	static constexpr uint32_t BatteryOvercharged 					= 0x00000004;
	static constexpr uint32_t BatteryOvertemperature				= 0x00000008;
	static constexpr uint32_t BatteryRemainingCapacity				= 0x00000010;
	static constexpr uint32_t BatteryTerminateCharge				= 0x00000020;
	static constexpr uint32_t SystemOverTemperature					= 0x00000040;
	static constexpr uint32_t SystemUnderTemperature				= 0x00000080;
	static constexpr uint32_t FanTachmometer 						= 0x00000100;
	static constexpr uint32_t ChargePilotFeedback					= 0x00000200;
	static constexpr uint32_t Encoder								= 0x00000400;
	static constexpr uint32_t FanSpeedError        					= 0x00000800;
	static constexpr uint32_t InternalTemperatureCommunications		= 0x00001000;
	static constexpr uint32_t ExternalTemperatureCommunications		= 0x00002000;
	static constexpr uint32_t CANCommunicationsOpen					= 0x00004000;
	static constexpr uint32_t HardwareAbstractionLayerError			= 0x00008000;
	static constexpr uint32_t TaskAllocation						= 0x00010000;
	static constexpr uint32_t StackOverflow							= 0x00020000;
	static constexpr uint32_t QueueAllocation						= 0x00040000;
	static constexpr uint32_t UARTCommunications					= 0x00080000;
	static constexpr uint32_t CANCommunicationsTransmit				= 0x00100000;
	static constexpr uint32_t OperatingSystem						= 0x00200000;
	static constexpr uint32_t Watchdog   							= 0x00400000;
	static constexpr uint32_t FlashMemory							= 0x00800000;
	static constexpr uint32_t Reserved10							= 0x01000000;
	static constexpr uint32_t Reserved11							= 0x02000000;
	static constexpr uint32_t Reserved12							= 0x04000000;
	static constexpr uint32_t Reserved13							= 0x08000000;
	static constexpr uint32_t Reserved14							= 0x10000000;
	static constexpr uint32_t Reserved15							= 0x20000000;
	static constexpr uint32_t Reserved16							= 0x40000000;
	static constexpr uint32_t Reserved17							= 0x80000000;

	// When these occur, code should not attempt to recover.
	static constexpr uint32_t fatal_faults = TaskAllocation | StackOverflow;

	Fault(GPIO_TypeDef* led_port, uint16_t led_pin)
	{
	    led.setup(led_port, led_pin);
	}

	/**
	 * Raises a fault.
	 * @param fault The fault.
	 */
	void alert(uint32_t fault)
	{
		this->fault |= fault;
		if (fault & fatal_faults)
			self_destruct();
		else
			update_fault_led();
	}


	/**
	 * Clears a raised fault.
	 * @note There is no effect if the specified fault is not raised.
	 * @param fault The fault.
	 */
	void reset(uint32_t fault)
	{
		this->fault = this->fault & (0xffffffff ^ fault);
		update_fault_led();
	}


	/**
	 * Raises or clears a fault, based on the specified state.
	 * @param fault The fault.
	 * @param state True to raise the fault, false to clear it.
	 */
	void update(uint32_t fault, bool state)
	{
		if (state) alert (fault);
		else reset (fault);
	}

	uint32_t get(void)
	{
		return this->fault;
	}


private:
	uint32_t fault = 0;
	Led led;


	/**
	 * Illuminates the fault LED if there is an outstanding fault.
	 */
	void update_fault_led()
	{
		bool state = this->fault > 0;
		led.set(state);
	}


	/**
	 * Flashes the fault LED.
	 */
	void self_destruct()
	{
	    bool state;
		// Give the user some indication that something very bad happened.
		for (;;)
		{
			led.set(state);
			state = !state;
			osDelay(100);
		}
	}
};



#endif /* INC_DIAGNOSTICS_FAULT_H_ */
