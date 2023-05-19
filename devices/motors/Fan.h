///	@file       devices/Fan.h
///	@class      Fan
///	@brief      Encapsulates communications with DS18x20 temperature measurement devices.
/// @note       See the DS18B20 datasheet for details how this class is implemented.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_OUTPUTS_FAN_H_
#define INC_OUTPUTS_FAN_H_

#include <math.h>
#include "stm32/stm32.h"
#include "stm32-toolbox/constants.h"
#include "stm32-toolbox/devices/Pwm.h"
#include "stm32-toolbox/utility/Timer.h"


class Fan : public Pwm
{
public:
	/**
	 * Prepares the fan for control.
	 * @param htim Handle to the timer allocated for PWM control.
	 */
	void setup(TIM_HandleTypeDef *htim)
	{
		this->htim = htim;
		duty = PERIOD/6;  // Initial duty.
		tach_timer.start(FAN_SPEED_UPDATE_INTERVAL);
	}


	/**
	 * Feedback loop to adjusts the fan speed so that the measured speed matches the target speed.
	 */
	void loop(void)
	{
		if (tach_timer.is_elapsed())
		{
			if (enabled)
			{
				float multiplier = (float)seconds(1)/FAN_SPEED_UPDATE_INTERVAL * 60.0;  // to RPM
				measured_speed = tach_count/POLES * multiplier;
				tach_count = 0;
				uint32_t diff = abs(measured_speed - target_speed);
				uint32_t incr = diff / FAN_SPEED_SEEK_DILIGENCE;
				duty = measured_speed > target_speed ? duty - incr : duty + incr;
				set_duty_cycle(duty);
			}
			else
			{
				set_target_speed(0);
				set_duty_cycle(0);
			}
			tach_timer.restart();
		}
	}


	/**
	 * Sets the target speed.
	 * @param speed The target speed.
	 */
	void set_target_speed(uint16_t speed)
	{
		target_speed = speed;
	}


	/**
	 * Gets the target speed of the fan (RPM).
	 * @return The target speed.
	 */
	uint16_t get_target_speed(void)
	{
		return target_speed;
	}


	/**
	 * Gets the current duty cycle (between 0.0 and 1.0).
	 * @return
	 */
	float get_duty_cycle(void)
	{
		return (float)duty / (float)PERIOD;
	}


	/**
	 * Gets the error ratio between target speed and measured speed.
	 * @return
	 */
	float get_error(void)
	{
		float t = (float)target_speed;
		float m = (float)measured_speed;
		return abs(m/(t+0.001)-1);  // Avoid division by zero.
	}


	/**
	 * Invoked by the hardware interrupt to indicate the fan has turned 180 degrees.
	 * @note This should not be called by user code.
	 */
	void interrupt(void)
	{
		tach_count += 25;
	}


	/**
	 * Gets the measured speed of the fan (RPM).
	 * @return The measured speed of the fan.
	 */
	uint16_t get_measured_speed(void)
	{
		return measured_speed;
	}


	/**
	 * Turns the fan on.
	 */
	void on(void)
	{
		enabled = true;
	}


	/**
	 * Turns the fan off.
	 */
	void off(void)
	{
		enabled = false;
	}


	/**
	 * Sets the enabled state on or off.
	 */
	void set(bool state)
	{
		enabled = state;
	}


	/**
	 * Gets the fan's enabled state.
	 * @returns true if enabled; otherwise false.
	 */
	bool get(void)
	{
		return enabled;
	}


	/**
	 * Sets the duty cycle
	 * @param duty The duty cycle, between 0.0 and 1.0.
	 */
	void set_duty_cycle(float duty)
	{
		if (duty <= 0) duty = 0;
		else if (duty > PERIOD) duty = PERIOD;
		this->duty = duty;
		PWM(htim, TIM_CHANNEL_3, PERIOD, duty);
		PWM(htim, TIM_CHANNEL_4, PERIOD, duty);
	}

private:
	uint16_t measured_speed = 0;
	uint16_t target_speed = 0;
	float duty;
	TIM_HandleTypeDef *htim;
	Timer tach_timer;
	uint16_t tach_count = 0;
	const uint32_t FAN_SPEED_UPDATE_INTERVAL = milliseconds(2000);
	const float PERIOD = 9000;//0.0001;
	const uint16_t POLES = 2;
	bool enabled = true;
};

#endif /* INC_OUTPUTS_FAN_H_ */
