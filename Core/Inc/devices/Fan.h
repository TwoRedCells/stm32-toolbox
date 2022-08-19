/**
 * \file       devices/Fan.h
 * \class      Fan
 * \brief      Fan controller with PWM output and open-drain feedback.
 */

#ifndef INC_OUTPUTS_FAN_H_
#define INC_OUTPUTS_FAN_H_

#include <math.h>
#include "devices/Pwm.h"
#include "utility/Timer.h"
#include "diagnostics/Fault.h"

extern Fault fault;


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
		MX_TIM3_Init();
		tach_timer.start(SPEED_UPDATE_INTERVAL);
	}


	/**
	 * Feedback loop to adjusts the fan speed so that the measured speed matches the target speed.
	 */
	void loop(void)
	{
        if (tach_timer.is_elapsed() && enabled)
        {
            uint32_t multiplier = seconds(1)/SPEED_UPDATE_INTERVAL * 60;  // to RPM
            measured_speed = tach_count/POLES * multiplier;
            tach_count = 0;
            tach_timer.restart();
            uint32_t diff = abs(measured_speed - target_speed);
            uint32_t incr = diff / FAN_SPEED_SEEK_DILIGENCE;
            duty = measured_speed > target_speed ? duty - incr : duty + incr;
            PWM(htim, TIM_CHANNEL_3, PERIOD, duty);
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
        tach_count++;
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



private:
	uint16_t measured_speed = 0;
	uint16_t target_speed = 0;
	float duty;
	TIM_HandleTypeDef *htim;
	Timer tach_timer;
	uint16_t tach_count = 0;
    const uint32_t SPEED_UPDATE_INTERVAL = milliseconds(1000);
    const float PERIOD = 9000;
    const uint16_t POLES = 4;
    bool enabled = true;

	/**
	 * @brief TIM3 Initialization Function
	 */
	void MX_TIM3_Init(void)
	{
		TIM_ClockConfigTypeDef sClockSourceConfig = {0};
		TIM_MasterConfigTypeDef sMasterConfig = {0};
		TIM_OC_InitTypeDef sConfigOC = {0};

		htim->Instance = TIM3;
		htim->Init.Prescaler = 11;
		htim->Init.CounterMode = TIM_COUNTERMODE_UP;
		htim->Init.Period = 0xffff;
		htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
		if (HAL_TIM_Base_Init(htim) != HAL_OK)
		{
		    log_e(STR_FAN_TIMER);
			fault.alert(Fault::HardwareAbstractionLayerError);
		}
		sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
		if (HAL_TIM_ConfigClockSource(htim, &sClockSourceConfig) != HAL_OK)
		{
            log_e(STR_FAN_TIMER);
            fault.alert(Fault::HardwareAbstractionLayerError);
		}
		if (HAL_TIM_PWM_Init(htim) != HAL_OK)
		{
            log_e(STR_FAN_TIMER);
            fault.alert(Fault::HardwareAbstractionLayerError);
		}
		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		if (HAL_TIMEx_MasterConfigSynchronization(htim, &sMasterConfig) != HAL_OK)
		{
            log_e(STR_FAN_TIMER);
            fault.alert(Fault::HardwareAbstractionLayerError);
		}
		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = 1000;
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		if (HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
		{
            log_e(STR_FAN_TIMER);
            fault.alert(Fault::HardwareAbstractionLayerError);
		}
		if (HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
		{
            log_e(STR_FAN_TIMER);
            fault.alert(Fault::HardwareAbstractionLayerError);
		}

		HAL_TIM_MspPostInit(htim);
	}
};

#endif /* INC_OUTPUTS_FAN_H_ */
